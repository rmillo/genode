/*
 * \brief  Nitpicker QPA plugin
 * \author Christian Prochaska
 * \date   2013-05-08
 */

/*
 * Copyright (C) 2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _QNITPICKERINTEGRATIONPLUGIN_H_
#define _QNITPICKERINTEGRATIONPLUGIN_H_

#include <qpa/qplatformintegrationplugin.h>
#include "qnitpickerintegration.h"

QT_BEGIN_NAMESPACE

class QNitpickerIntegrationPlugin : public QPlatformIntegrationPlugin
{
	Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QPA.QPlatformIntegrationFactoryInterface.5.3" FILE "nitpicker.json")
public:
    QPlatformIntegration *create(const QString&, const QStringList&) Q_DECL_OVERRIDE;
};

QT_END_NAMESPACE

#endif /* _QNITPICKERINTEGRATIONPLUGIN_H_ */
