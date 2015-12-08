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

/* Qt includes */
#include <QtGui/private/qguiapplication_p.h>
#include "qgenodeclipboard.h"
#include "qnitpickerglcontext.h"
#include "qnitpickerintegration.h"
#include "qnitpickerplatformwindow.h"
#include "qnitpickerscreen.h"
#include "qnitpickerwindowsurface.h"
#include "qgenericunixeventdispatcher_p.h"
#include "qbasicfontdatabase_p.h"

QT_BEGIN_NAMESPACE

static const bool verbose = false;

Genode::Signal_receiver &QNitpickerIntegration::_signal_receiver()
{
	static Genode::Signal_receiver _inst;
	return _inst;
}

QNitpickerIntegration::QNitpickerIntegration()
: _signal_handler_thread(_signal_receiver()),
  _nitpicker_screen(new QNitpickerScreen()),
  _event_dispatcher(createUnixEventDispatcher())
{
    screenAdded(_nitpicker_screen);
    _signal_handler_thread.start();
}

QNitpickerIntegration::~QNitpickerIntegration()
{
    delete _nitpicker_screen;
}

bool QNitpickerIntegration::hasCapability(QPlatformIntegration::Capability cap) const
{
	switch (cap) {
		case ThreadedPixmaps: return true;
		default: return QPlatformIntegration::hasCapability(cap);
	}
}


QPlatformWindow *QNitpickerIntegration::createPlatformWindow(QWindow *window) const
{
	if (verbose)
		qDebug() << "QNitpickerIntegration::createPlatformWindow(" << window << ")";

    QRect screen_geometry = _nitpicker_screen->geometry();
    QPlatformWindow *w = new QNitpickerPlatformWindow(window,
                                         _signal_receiver(),
                                         screen_geometry.width(),
                                         screen_geometry.height());
    w->requestActivateWindow();
    return w;
}


QPlatformBackingStore *QNitpickerIntegration::createPlatformBackingStore(QWindow *window) const
{
	if (verbose)
		qDebug() << "QNitpickerIntegration::createPlatformBackingStore(" << window << ")";
    return new QNitpickerWindowSurface(window);
}


QAbstractEventDispatcher *QNitpickerIntegration::createEventDispatcher() const
{
	if (verbose)
		qDebug() << "QNitpickerIntegration::guiThreadEventDispatcher()";
	return _event_dispatcher;
}


QPlatformFontDatabase *QNitpickerIntegration::fontDatabase() const
{
    static QBasicFontDatabase db;
    return &db;
}


QPlatformOpenGLContext *QNitpickerIntegration::createPlatformOpenGLContext(QOpenGLContext *context) const
{
    return new QNitpickerGLContext(context);
}

QVariant QNitpickerIntegration::styleHint(QPlatformIntegration::StyleHint hint) const
{
    if (hint == QPlatformIntegration::ShowIsFullScreen)
        return true;

    return QPlatformIntegration::styleHint(hint);
}

QT_END_NAMESPACE
