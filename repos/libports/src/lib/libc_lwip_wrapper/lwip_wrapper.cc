/**
 * \brief  Lwip funtions to handle IP address in QT Network applications
 * \author Reinier Millo Sánchez
 * \date   2015-12-05
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <lwip/opt.h>
#include <lwip/netif.h>
#include <stdio.h>

#include "lwip_wrapper.h"

using namespace Genode;

char* get_lwip_ip_addr()
{
	char *ip_dir=(char*)malloc(16*sizeof(char));
    if(netif_is_up(netif_default)){
		u32_t addr = netif_default->ip_addr.addr;
		sprintf(ip_dir, "%d.%d.%d.%d", ip4_addr1_16(&addr),ip4_addr2_16(&addr),ip4_addr3_16(&addr),ip4_addr4_16(&addr));
	}else{
		sprintf(ip_dir,"127.0.0.1");
	}
	return ip_dir;
}
