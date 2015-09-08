/*
 * \brief  Gpio driver for the Odroid-x2
 * \author Alexy Gallardo Segura <alexy@uclv.cu>
 * \author Humberto Lopéz Leon <humberto@uclv.cu>
 * \author Reinier Millo Sánchez <rmillo@uclv.cu>
 * \date   2015-07-03
 */


/*
 * Copyright (C) 2012 Ksys Labs LLC
 * Copyright (C) 2012-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DRIVER_H_
#define _DRIVER_H_

/* Genode includes */
#include <drivers/board_base.h>
#include <gpio/driver.h>
#include <irq_session/connection.h>
#include <timer_session/connection.h>

/* local includes */
#include <gpio.h>

static int verbose = 1;
namespace Gpio{ class Odroid_x2_driver;}


class Gpio::Odroid_x2_driver : public Driver
{
	private:
		Server::Entrypoint &_ep;
		Reg					_reg1;
		Reg					_reg2;
		Reg					_reg3;
		Reg					_reg4;
		Genode::Irq_connection                 _irq;
		Genode::Signal_rpc_member<Odroid_x2_driver>  _dispatcher;
		Genode::Signal_context_capability      _sig_cap[MAX_PINS];
		bool                                   _irq_enabled[MAX_PINS];
		bool                                   _async;

		void _handle(unsigned)
		{
			handle_irq();
		}

		void handle_irq(){
			PDBG("IRQ\n");

		}

		Gpio::Reg *_gpio_reg(int gpio_pin)
		{
			int pos=gpio_bank_index(gpio_pin,true);
			switch(pos){
				case 0 ... 13:
					return &_reg1;
				case 14 ... 38:
					return &_reg2;
				case 39:
					return &_reg3;
				case 40 ... 46:
					return &_reg4;
				default:
					PERR("no Gpio_bank for pin %d available", gpio_pin);
					return 0;
			}
		}

		int _gpio_index(int gpio)       { return gpio & 0x1f; }

		Odroid_x2_driver(Server::Entrypoint &ep)
		:	_ep(ep),
		 	_reg1(0x11400000,1000),
		 	_reg2(0x11000000,1000),
		 	_reg3(0x03860000,1000),
		 	_reg4(0x106E0000,1000),
		 	_irq(104),
		 	_dispatcher(ep,*this,&Odroid_x2_driver::_handle),
		 	_async(false)

		{
			_irq.sigh(_dispatcher);
			_irq.ack_irq();
		}


	public:

		static Odroid_x2_driver& factory(Server::Entrypoint &ep);


		/******************************
		 **  Gpio::Driver interface  **
		 ******************************/

		void direction(unsigned gpio_pin, bool input)
		{
			int pos_gpio= gpio_bank_index(gpio_pin,true);
			int sum_gpio= gpio_bank_index(gpio_pin,false);
			Genode::off_t offset = _bankOffset[pos_gpio];
			int gpio=gpio_pin-sum_gpio;

			if ( verbose){
				PDBG("gpio=%d",gpio);
				PDBG("gpio_pin=%d",gpio_pin);
				PDBG("gpio_input=%d",input ? 0: 0x1);
				PDBG("gpio_pos=%d",pos_gpio);
				PDBG("gpio_sum=%d",sum_gpio);
				PDBG("gpio_off_set=%d",(int)offset);
			}

			Reg* reg=  _gpio_reg(gpio_pin);
			reg->setDirection(gpio,input,offset);
		}

		void write(unsigned gpio_pin, bool level)
		{
			int pos_gpio= gpio_bank_index(gpio_pin,true);
			int sum_gpio= gpio_bank_index(gpio_pin,false);
			Genode::off_t offset = _bankOffset[pos_gpio];
			int gpio=gpio_pin-sum_gpio;

			if ( verbose){
				PDBG("gpio=%d",gpio);
				PDBG("gpio_pin=%d",gpio_pin);
				PDBG("gpio_level=%d",level ? 0: 0x1);
				PDBG("gpio_pos=%d",pos_gpio);
				PDBG("gpio_sum=%d",sum_gpio);
				PDBG("gpio_off_set=%d",(int)offset);
			}
			Reg* reg=  _gpio_reg(gpio_pin);
			reg->writePin(gpio,level,offset);
		}

		bool read(unsigned gpio_pin)
		{
			int pos_gpio= gpio_bank_index(gpio_pin,true);
			int sum_gpio= gpio_bank_index(gpio_pin,false);
			Genode::off_t offset = _bankOffset[pos_gpio];
			int gpio=gpio_pin-sum_gpio;
			if ( verbose){
				PDBG("gpio=%d",gpio);
				PDBG("gpio_pin=%d",gpio_pin);
				PDBG("gpio_pos=%d",pos_gpio);
				PDBG("gpio_sum=%d",sum_gpio);
				PDBG("gpio_off_set=%d",(int)offset);
			}

			Reg* reg=  _gpio_reg(gpio_pin);
			return reg->readPin(gpio,offset) ;

		}

		void debounce_enable(unsigned gpio, bool enable){	PWRN("Not supported!");}

		void debounce_time(unsigned gpio, unsigned long us){	PWRN("Not supported!");	}

		void falling_detect(unsigned gpio_pin)
		{
			if (verbose) PDBG("gpio=%d", gpio_pin);

			int pos_gpio= gpio_bank_index(gpio_pin,true);
			int sum_gpio= gpio_bank_index(gpio_pin,false);
			Genode::off_t offset = _irqOffset[pos_gpio];
			int gpio=gpio_pin-sum_gpio;

			Reg* reg=  _gpio_reg(gpio_pin);
			reg->setEnableTriggers(gpio,offset,FALLING);
		}

		void rising_detect(unsigned gpio_pin)
		{
			if (verbose) PDBG("gpio=%d", gpio_pin);

			int pos_gpio= gpio_bank_index(gpio_pin,true);
			int sum_gpio= gpio_bank_index(gpio_pin,false);
			Genode::off_t offset = _irqOffset[pos_gpio];
			int gpio=gpio_pin-sum_gpio;

			Reg* reg=  _gpio_reg(gpio_pin);
			reg->setEnableTriggers(gpio,offset,RISING);

		}

		void high_detect(unsigned gpio_pin)
		{
			if (verbose) PDBG("gpio=%d", gpio_pin);
			int pos_gpio= gpio_bank_index(gpio_pin,true);
			int sum_gpio= gpio_bank_index(gpio_pin,false);
			Genode::off_t offset = _irqOffset[pos_gpio];
			int gpio=gpio_pin-sum_gpio;

			Reg* reg=  _gpio_reg(gpio_pin);
			reg->setEnableTriggers(gpio,offset,HIGH);

		}

		void low_detect(unsigned gpio_pin)
		{
			if (verbose) PDBG("gpio=%d", gpio_pin);

			int pos_gpio= gpio_bank_index(gpio_pin,true);
			int sum_gpio= gpio_bank_index(gpio_pin,false);
			Genode::off_t offset = _irqOffset[pos_gpio];
			int gpio=gpio_pin-sum_gpio;

			Reg* reg=  _gpio_reg(gpio_pin);
			reg->setEnableTriggers(gpio,offset,LOW);

		}

		void irq_enable(unsigned gpio_pin, bool enable)
		{
			if (verbose) PDBG("gpio=%d enable=%d", gpio_pin, enable);
			_irq_enabled[gpio_pin] = enable;
		}

		void ack_irq(unsigned gpio_pin)
		{
			if (verbose) PDBG("gpio=%d", gpio_pin);
			int pos_gpio= gpio_bank_index(gpio_pin,true);

			_irq.ack_irq();
		}

		void register_signal(unsigned gpio_pin,
		                     Genode::Signal_context_capability cap)
		{
			if (verbose) PDBG("gpio=%d", gpio_pin);
			_sig_cap[gpio_pin] = cap;

		}

		void unregister_signal(unsigned gpio_pin)
		{
			if (verbose) PDBG("gpio=%d", gpio_pin);
			Genode::Signal_context_capability cap;
			_sig_cap[gpio_pin] = cap;

		}

		int gpio_bank_index(int pin, bool pos)
		{
			int i=0 ,sum = 0;

			while(i<MAX_BANKS && ((sum + _bankSizes[i]) <= pin))
			{
				sum +=  1 + _bankSizes[i++];
			}
			return pos ? i : sum;
		}

		bool gpio_valid(unsigned gpio) { return gpio < (MAX_PINS); }
};


Gpio::Odroid_x2_driver& Gpio::Odroid_x2_driver::factory(Server::Entrypoint &ep)
{
	static Odroid_x2_driver driver(ep);
	return driver;
}

#endif /* _DRIVER_H_ */
