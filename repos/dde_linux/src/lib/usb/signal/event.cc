/*
 * \brief  Signal context for completions and events
 * \author Sebastian Sumpf <sebastian.sumpf@genode-labs.com>
 * \date   2012-05-23
 */

/*
 * Copyright (C) 2012-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include <signal.h>
#include <lx_emul.h>

#include <lx_kit/scheduler.h>
#include <lx_kit/work.h>

static Signal_helper *_signal = 0;

/**
 * Context for events
 */
class Event_context
{
	private:

		Genode::Signal_rpc_member<Event_context> _dispatcher;

		void _handle(unsigned) {
			Routine::schedule_all(); }

		Event_context()
		: _dispatcher(_signal->ep(), *this, &Event_context::_handle) {
		  _signal->sender().context(_dispatcher); }

	public:

		static Event_context *e()
		{
			static Event_context _e;
			return &_e;
		}

		void submit() {
			_signal->sender().submit(); }

		char const *debug() { return "Event_context"; }
};


void Event::init(Server::Entrypoint &ep) {
	_signal = new (Genode::env()->heap()) Signal_helper(ep); }

#if 0
/**
 * Delayed work
 */
class Work : public Genode::List<Work>::Element
{
	private:

		void *_work;

		enum Type { NORMAL, DELAYED, TASKLET } _type;

		static Genode::List<Work> *_list()
		{
			static Genode::List<Work> _l;
			return &_l;
		}

	public:

		Work(delayed_work *work)   : _work(work), _type(DELAYED) { }
		Work(work_struct  *work)   : _work(work), _type(NORMAL)  { }
		Work(tasklet_struct *work) : _work(work), _type(TASKLET) { }

		template <typename WORK>
		static void schedule(WORK *work)
		{
			if (work->pending)
				return;

			work->pending = 1;
			_list()->insert(new (Genode::env()->heap()) Work(work));
		}

		static void exec()
		{
			while (_list()->first()) {

				Work *w = _list()->first();
				_list()->remove(w);

				switch (w->_type) {

				case NORMAL:
					{
						work_struct *work = static_cast<work_struct *>(w->_work);
						work->func(work);
						work->pending = 0;
					}
					break;

				case DELAYED:
					{
						delayed_work *work = static_cast<delayed_work *>(w->_work);
						work->work.func(&(work)->work);
						work->pending = 0;
					}
					break;

				case TASKLET:
					{
						tasklet_struct *tasklet = static_cast<tasklet_struct *>(w->_work);
						tasklet->func(tasklet->data);
						tasklet->pending = 0;
					}
					break;
				}
				destroy(Genode::env()->heap(), w);
			}
		}
};
#endif

/************************
 ** linux/completion.h **
 ************************/

//oid __wake_up() { Routine::schedule_all(); }



#if 0
void init_completion(struct completion *work)
{
	lx_log(DEBUG_COMPLETION, "New completion %p", work);
	work->done = 0;
}


void complete(struct completion *work)
{
	lx_log(DEBUG_COMPLETION, "%p", work);
	work->done = 1;

	/* send signal */
	Event_context::e()->submit();
}
#endif
#if 0
void complete_and_exit(struct completion *work, long code)
{
	lx_log(DEBUG_COMPLETION, "%p", work);
	complete(work);
	Routine::remove();
}
#endif 



#if 0
static unsigned long
__wait_completion_timeout(struct completion *work, unsigned long timeout)
{
	unsigned long _j = jiffies + timeout;
	while (!work->done) {
		__wait_event();

		if (_j <= jiffies) {
			lx_log(1, "Timeout");
			return 0;
		}
	}

	work->done = 0;

	return _j - jiffies;
}


unsigned long wait_for_completion_timeout(struct completion *work,
                                          unsigned long timeout)
{
	lx_log(DEBUG_COMPLETION, "%p state: %u timeout: %lu", work, work->done, timeout);
	return __wait_completion_timeout(work, timeout);
}


int wait_for_completion_interruptible(struct completion *work)
{
	lx_log(DEBUG_COMPLETION, "%p state: %u", work, work->done);

	__wait_completion(work);
	return 0;
}


long wait_for_completion_interruptible_timeout(struct completion *work,
                                               unsigned long timeout)
{
	lx_log(DEBUG_COMPLETION, "%p state: %u", work, work->done);
	__wait_completion(work);
	return 1;
}


void wait_for_completion(struct completion *work)
{
	lx_log(DEBUG_COMPLETION, "%p state: %u", work, work->done);
	__wait_completion(work);
}
#endif

/*******************
 ** linux/timer.h **
 *******************/


#if 0
int wake_up_process(struct task_struct *tsk)
{
	Routine::schedule_all();
	return 0;
}
#endif

/***********************
 ** linux/workquque.h **
 ***********************/

#if 0
int schedule_delayed_work(struct delayed_work *work, unsigned long delay)
{
	Work::schedule(work);
	return 0;
}
#endif
#if 0
int schedule_work(struct work_struct *work)
{
	Work::schedule(work);
	return 1;
}
#endif
#if 0
bool queue_delayed_work(struct workqueue_struct *wq,
                        struct delayed_work *dwork, unsigned long delay)
{
	Work::schedule(dwork);
	return true;
}
#endif

/***********************
 ** linux/interrupt.h **
 ***********************/


/***********************
 ** linux/workqueue.h **
 ***********************/



/**********
 ** init **
 **********/

#if 0
void Event::loop()
{
	while (true)
		__wait_event();
}
#endif
