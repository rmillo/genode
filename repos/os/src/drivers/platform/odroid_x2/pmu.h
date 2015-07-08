/*
 * \brief  Regulator driver for power management unit of Exynos4412 SoC
 * \author Alexy Gallardo Segura <alexy@uclv.cu>
 * \author Humberto Lopez Leon <humberto@uclv.cu>
 * \author Reinier Millo Sanchez <rmillo@uclv.cu>
 * \date   2015-07-08
 */

/*
 * Copyright (C) 2015 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _PMU_H_
#define _PMU_H_

#include <regulator/consts.h>
#include <regulator/driver.h>
#include <drivers/board_base.h>
#include <os/attached_mmio.h>

using namespace Regulator;


class Pmu : public Regulator::Driver,
            public Genode::Attached_mmio
{
	private:

		template <unsigned OFFSET>
		struct Control : Register <OFFSET, 32>
		{
			struct Enable : Register<OFFSET, 32>::template Bitfield<0, 1> { };
		};

		template <unsigned OFFSET>
		struct Configuration : Register <OFFSET, 32>
		{
			struct Local_pwr_cfg : Register<OFFSET, 32>::template Bitfield<0, 3> { };
		};

		template <unsigned OFFSET>
		struct Status : Register <OFFSET, 32>
		{
			struct Stat : Register<OFFSET, 32>::template Bitfield<0, 3> { };
		};

		template <unsigned OFFSET>
		struct Sysclk_configuration : Register <OFFSET, 32>
		{
			struct Local_pwr_cfg : Register<OFFSET, 32>::template Bitfield<0, 1> { };
		};

		template <unsigned OFFSET>
		struct Sysclk_status : Register <OFFSET, 32>
		{
			struct Stat : Register<OFFSET, 32>::template Bitfield<0, 1> { };
		};

		struct Hdmi_phy_control : Register<0x700, 32>
		{
			struct Enable    : Bitfield<0, 1> { };
			struct Div_ratio : Bitfield<16, 10> { };
		};

		typedef Control<0x0704> Usbdrd_phy_control;
		typedef Control<0x0708> Usbhost_phy1_control;
		typedef Control<0x70c> Usbhost_phy2_control; 


		void _enable(unsigned long id)
		{
			switch (id) {
			case PWR_USB20:	
				write<Usbdrd_phy_control::Enable>(1);
				write<Usbhost_phy1_control::Enable>(1);
				write<Usbhost_phy2_control::Enable>(1);
				break;
			default:
				PWRN("Unsupported for %s", names[id].name);
			}
		}

		void _disable(unsigned long id)
		{
			switch (id) {
			case PWR_USB20:
				write<Usbdrd_phy_control::Enable>(0);
				write<Usbhost_phy1_control::Enable>(0);
				write<Usbhost_phy2_control::Enable>(0);
				break;
			default:
				PWRN("Unsupported for %s", names[id].name);
			}
		}

	public:

		/**
		 * Constructor
		 */
		Pmu() : Genode::Attached_mmio(Genode::Board_base::PMU_MMIO_BASE,
		                              Genode::Board_base::PMU_MMIO_SIZE)
		{
			write<Usbdrd_phy_control ::Enable>(0);
			write<Usbhost_phy1_control::Enable>(0);
			write<Usbhost_phy2_control::Enable>(0);
		}


		/********************************
		 ** Regulator driver interface **
		 ********************************/

		void level(Regulator_id id, unsigned long level)
		{
			switch (id) {
			default:
				PWRN("Unsupported for %s", names[id].name);
			}
		}

		unsigned long level(Regulator_id id)
		{
			switch (id) {
			default:
				PWRN("Unsupported for %s", names[id].name);
			}
			return 0;
		}

		void state(Regulator_id id, bool enable)
		{
			if (enable)
				_enable(id);
			else
				_disable(id);
		}

		bool state(Regulator_id id)
		{
			switch (id) {
			case PWR_USB20:
				return read<Usbdrd_phy_control::Enable>();
			default:
				PWRN("Unsupported for %s", names[id].name);
			}
			return true;
		}
};

#endif /* _PMU_H_ */
