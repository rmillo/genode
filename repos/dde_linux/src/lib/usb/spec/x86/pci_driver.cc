/**
 * \brief  PCI device handling
 * \author Sebastian Sumpf
 * \date   2016-04-25
 */

/*
 * Copyright (C) 2016 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include <base/allocator.h>
#include <base/lock.h>
#include <base/signal.h>
#include <util/misc_math.h>

#include <lx_emul.h>

#include <lx_kit/irq.h>
#include <lx_kit/pci_dev_registry.h>
#include <lx_kit/mapped_io_mem_range.h>

#include <lx_emul/impl/io.h>

/**
 * Quirks
 */
extern "C" void __pci_fixup_quirk_usb_early_handoff(void *data);


/**
 * List of pci devices from platform driver
 */
class Pci_dev_list
{
	private:

		struct Element : public Lx_kit::List<Element>::Element
		{
			Platform::Device_capability cap;

			Element(Platform::Device_capability cap) : cap(cap) { }
		};

		Lx_kit::List<Element> _pci_caps;

	public:

		Pci_dev_list()
		{
			/*
			 * Functor that is called if the platform driver throws a
			 * 'Out_of_metadata' exception.
			 */
			auto handler = [&] () {
				Genode::env()->parent()->upgrade(Lx::pci()->cap(),
				                                 "ram_quota=4096"); };

			/*
			 * Obtain first device, the operation may exceed the session quota.
			 * So we use the 'retry' mechanism.
			 */
			Platform::Device_capability cap;
			auto attempt = [&] () { cap = Lx::pci()->first_device(); };
			Genode::retry<Platform::Session::Out_of_metadata>(attempt, handler);

			/*
			 * Iterate over the devices of the platform session.
			 */
			while (cap.valid()) {

				_pci_caps.insert(new (Genode::env()->heap()) Element(cap));

				 /* try next one. Upgrade session * quota on demand.*/
				auto attempt = [&] () {
					cap = Lx::pci()->next_device(cap);
				};

				Genode::retry<Platform::Session::Out_of_metadata>(attempt, handler);
			}
		}

		template <typename FUNC>
		void for_each_pci_device(FUNC const &func)
		{
			for (Element *e = _pci_caps.first(); e; e = e->next())
				func(e->cap);
		}
};


Pci_dev_list *pci_dev_list()
{
	static Pci_dev_list _list;
	return &_list;
}


extern "C" int pci_register_driver(struct pci_driver *driver)
{
	driver->driver.name = driver->name;

	pci_device_id const *id_table = driver->id_table;
	if (!id_table)
		return -ENODEV;

	using namespace Genode;

	bool found = false;

	auto lamda = [&] (Platform::Device_capability cap) {

		Platform::Device_client client(cap);

		/* request device ID from platform driver */
		unsigned const class_code = client.class_code();

		/* look if we find the device ID in the driver's 'id_table' */
		pci_device_id const *matching_id = nullptr;
		for (pci_device_id const *id = id_table; id->device; id++) {

			lx_log(DEBUG_PCI,"idclass: %x idclassm: %x devclass %x", id->class_,
			       id->class_mask, class_code);

			/* check for drivers that support any device for a given class */
			if (id->device != (unsigned)PCI_ANY_ID || !id->class_mask)
				continue;

			if ((id->class_ & id->class_mask) == (class_code & id->class_mask)) {
				matching_id = id;
				break;
			}
		}

		/* skip device that is not handled by driver */
		if (!matching_id)
			return false;

		/* create 'pci_dev' struct for matching device */
		Lx::Pci_dev *pci_dev = new (env()->heap()) Lx::Pci_dev(cap);

		/* enable ioremap to work */
		Lx::pci_dev_registry()->insert(pci_dev);

		/* register driver at the 'pci_dev' struct */
		pci_dev->dev.driver = &driver->driver;

		/*
		 * This quirk handles device handoff from BIOS, since the BIOS may still
		 * access the USB controller after bootup. For this the ext cap regsiter of
		 * the PCI config space is checked
		 */
		__pci_fixup_quirk_usb_early_handoff(pci_dev);

		/* call probe function of the Linux driver */
		if (driver->probe(pci_dev, matching_id)) {

			/* if the probing failed, revert the creation of 'pci_dev' */
			pci_dev_put(pci_dev);
			return false;
		}

		found = true;

		/* multiple device support continue */
		return true;
	};

	pci_dev_list()->for_each_pci_device(lamda);

	return found ? 0 : -ENODEV;
}


void pci_dev_put(struct pci_dev *pci_dev)
{
	Genode::destroy(Genode::env()->heap(), pci_dev);
}


extern "C" const char *pci_name(const struct pci_dev *pdev)
{
	/* simply return driver name */
	return "dummy";
}


extern "C" size_t pci_resource_start(struct pci_dev *dev, unsigned bar)
{
	if (bar >= DEVICE_COUNT_RESOURCE)
		return 0;

	return dev->resource[bar].start;
}


extern "C" size_t pci_resource_end(struct pci_dev *dev, unsigned bar)
{
	if (bar >= DEVICE_COUNT_RESOURCE)
		return 0;

	return dev->resource[bar].end;
}


extern "C" size_t pci_resource_len(struct pci_dev *dev, unsigned bar)
{
	size_t start = pci_resource_start(dev, bar);

	if (!start)
		return 0;

	return (dev->resource[bar].end - start) + 1;
}


extern "C" unsigned int pci_resource_flags(struct pci_dev *dev, unsigned bar)
{
	if (bar >= DEVICE_COUNT_RESOURCE)
		return 0;

	return dev->resource[bar].flags;
}


extern "C" int pci_bus_read_config_byte(struct pci_bus *bus, unsigned int, int where, u8 *val)
{
	Lx::Pci_dev *dev = (Lx::Pci_dev *)bus;
	dev->config_read(where, val);
	return 0;
}


extern "C" int pci_bus_read_config_word(struct pci_bus *bus, unsigned int, int where, u16 *val)
{
	Lx::Pci_dev *dev = (Lx::Pci_dev *)bus;
	dev->config_read(where, val);
	return 0;
}


extern "C" int pci_bus_read_config_dword(struct pci_bus *bus, unsigned int, int where, u32 *val)
{
	Lx::Pci_dev *dev = (Lx::Pci_dev *)bus;
	dev->config_read(where, val);
	return 0;
}


extern "C" int pci_bus_write_config_byte(struct pci_bus *bus, unsigned int, int where, u8 val)
{
	Lx::Pci_dev *dev = (Lx::Pci_dev *)bus;
	dev->config_write(where, val);
	return 0;
}


extern "C" int pci_bus_write_config_word(struct pci_bus *bus, unsigned int, int where, u16 val)
{
	Lx::Pci_dev *dev = (Lx::Pci_dev *)bus;
	dev->config_write(where, val);
	return 0;
}


extern "C" int pci_bus_write_config_dword(struct pci_bus *bus, unsigned int, int where, u32 val)
{
	Lx::Pci_dev *dev = (Lx::Pci_dev *)bus;
	dev->config_write(where, val);
	return 0;
}


extern "C" void *pci_ioremap_bar(struct pci_dev *dev, int bar)
{
	using namespace Genode;

	if (bar >= DEVICE_COUNT_RESOURCE || bar < 0)
		return 0;

	return Lx::ioremap(pci_resource_start(dev, bar),
	                   pci_resource_len(dev, bar),
	                   Genode::UNCACHED);
}

/***********************
 ** linux/interrupt.h **
 ***********************/

int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,
                const char *name, void *dev)
{
	for (Lx::Pci_dev *pci_dev =  Lx::pci_dev_registry()->first(); pci_dev; pci_dev = pci_dev->next())
		if (pci_dev->irq == irq) {
			Lx::Irq::irq().request_irq(pci_dev->client(), handler, dev);
			return 0;
		}

	return -ENODEV;
}
