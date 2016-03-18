/*
 * \brief  Timer
 * \author Sebastian Sumpf
 * \author Josef Soentgen
 * \author Norman Feske
 * \date   2014-10-10
 */

/*
 * Copyright (C) 2014-2016 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _LX_KIT__TIMER_H_
#define _LX_KIT__TIMER_H_

/* Genode includes */
#include <os/server.h>

namespace Lx {

	class Timer;

	/**
	 * Return singleton instance of timer
	 *
	 * \param ep          entrypoint used handle timeout signals,
	 *                    to be specified at the first call of the function,
	 *                    which implicitly initializes the timer
	 * \param jiffies_ptr pointer to jiffies counter to be periodically
	 *                    updated
	 */
	Timer &timer(Server::Entrypoint *ep          = nullptr,
	             unsigned long      *jiffies_ptr = nullptr);

	void timer_update_jiffies();
}

struct timer_list;

class Lx::Timer
{
	public:

		/**
		 * Add new linux timer
		 */
		virtual void add(struct ::timer_list *timer) = 0;

		/**
		 * Delete linux timer
		 */
		virtual int del(struct ::timer_list *timer) = 0;

		/**
		 * Initial scheduling of linux timer
		 */
		virtual int schedule(struct ::timer_list *timer, unsigned long expires) = 0;

		/**
		 * Schedule next linux timer
		 */
		virtual void schedule_next() = 0;

		/**
		 * Check if the timer is currently pending
		 */
		virtual bool pending(struct ::timer_list const *timer) = 0;

		/**
		 * Check if the timer is already present
		 */
		virtual bool find(struct ::timer_list const *timer) const = 0;

		/**
		 * Update jiffie counter
		 */
		virtual void update_jiffies() = 0;
};


#endif /* _LX_KIT__TIMER_H_ */
