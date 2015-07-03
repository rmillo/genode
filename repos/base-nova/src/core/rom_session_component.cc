/*
 * \brief  Core implementation of the ROM session interface
 * \author Norman Feske
 * \date   2006-07-06
 */

/*
 * Copyright (C) 2006-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include <base/printf.h>
#include <util/arg_string.h>
#include <rom_session_component.h>
#include <root/root.h>

/* core includes */
#include <platform.h>

/* Nova includes */
#include "nova_util.h"

bool verbose = true;

using namespace Genode;

static addr_t remap_rom(addr_t src_core, size_t src_size) {

	Thread_base  *myself = Thread_base::myself();
	Nova::Utcb   *  utcb = reinterpret_cast<Nova::Utcb *>(myself->utcb());

	size_t const rom_size = round_page(src_size);
	void *rom_ptr = 0;
	bool res = platform()->region_alloc()->alloc(rom_size, &rom_ptr);

	if (!res)
		return src_core;

	addr_t const rom_addr = reinterpret_cast<addr_t>(rom_ptr);
	res = map_local(utcb, src_core, rom_addr,
	                rom_size >> 12, Nova::Rights(true, false, true));

	if (!res)
		return rom_addr;

	unmap_local(utcb, rom_addr, rom_size);
	platform()->region_alloc()->free(rom_ptr, rom_size);

	return src_core;
}

Rom_session_component::Rom_session_component(Rom_fs         *rom_fs,
                                             Rpc_entrypoint *ds_ep,
                                             const char     *args)
:
	_rom_module(_find_rom(rom_fs, args)),
	_ds(_rom_module ? _rom_module->size() : 0,
	    _rom_module ? remap_rom(_rom_module->addr(), _rom_module->size()) : 0, CACHED, false, 0),
	_ds_ep(ds_ep)
{
	/* ROM module not found */
	if (!_rom_module)
		throw Root::Invalid_args();

	_ds_cap = static_cap_cast<Rom_dataspace>(_ds_ep->manage(&_ds));

	if (verbose)
		printf("remap_rom '%s' cap=%lx %lx->%lx\n", _fname, _ds_cap.local_name(),
		       _rom_module->addr(), _ds.map_src_addr());
}


Rom_session_component::~Rom_session_component()
{
	if (_rom_module && _rom_module->addr() != _ds.map_src_addr()) {
		Thread_base  *myself = Thread_base::myself();
		Nova::Utcb   *  utcb = reinterpret_cast<Nova::Utcb *>(myself->utcb());

		if (verbose)
			PINF("revoke_rom '%s' cap=%lx %lx->%lx", _fname, _ds_cap.local_name(), _rom_module->addr(), _ds.map_src_addr());

		unmap_local(utcb, _ds.map_src_addr(), _ds.size() >> 12);
		platform()->region_alloc()->free(reinterpret_cast<void *>(_ds.map_src_addr()), _ds.size());
	}

	_ds_ep->dissolve(&_ds);
}
