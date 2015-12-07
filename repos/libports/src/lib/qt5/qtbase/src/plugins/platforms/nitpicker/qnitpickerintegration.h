/*
 * \brief  QNitpickerIntegration
 * \author Christian Prochaska
 * \date   2013-05-08
 */

/*
 * Copyright (C) 2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */


#ifndef _QNITPICKERINTEGRATION_H_
#define _QNITPICKERINTEGRATION_H_

#include <QOpenGLContext>

#include <qpa/qplatformintegration.h>
#include <qpa/qplatformscreen.h>

#include "qnitpickerscreen.h"
#include "qsignalhandlerthread.h"

QT_BEGIN_NAMESPACE

class QNitpickerIntegration : public QPlatformIntegration
{
	private:

		QSignalHandlerThread      _signal_handler_thread;

		QNitpickerScreen         *_nitpicker_screen;
	    QAbstractEventDispatcher *_event_dispatcher;

		/*
		 * A reference to the signal receiver gets passed to newly created
		 * objects, for example in 'createPlatformWindow()'. Since this is
		 * a const member function, the signal receiver cannot be a member
		 * variable of QNitpickerIntegration.
		 */
		static Genode::Signal_receiver &_signal_receiver();

	public:

		QNitpickerIntegration();
		~QNitpickerIntegration();

		bool hasCapability(QPlatformIntegration::Capability cap) const Q_DECL_OVERRIDE;

		QPlatformWindow *createPlatformWindow(QWindow *window) const Q_DECL_OVERRIDE;
		QPlatformBackingStore *createPlatformBackingStore(QWindow *window) const Q_DECL_OVERRIDE;
		QAbstractEventDispatcher *createEventDispatcher() const Q_DECL_OVERRIDE;

		QPlatformFontDatabase *fontDatabase() const Q_DECL_OVERRIDE;

		QPlatformOpenGLContext *createPlatformOpenGLContext(QOpenGLContext *context) const Q_DECL_OVERRIDE;

		QVariant styleHint(QPlatformIntegration::StyleHint hint) const;
};

QT_END_NAMESPACE

#endif /* _QNITPICKERINTEGRATION_H_ */
