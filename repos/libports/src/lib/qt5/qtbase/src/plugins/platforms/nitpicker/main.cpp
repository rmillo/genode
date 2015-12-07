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

#include "qnitpickerintegrationplugin.h"

QT_BEGIN_NAMESPACE

QPlatformIntegration *QNitpickerIntegrationPlugin::create(const QString& system, const QStringList& paramList)
{
    Q_UNUSED(paramList);
    if (!system.compare(QLatin1String("nitpicker"), Qt::CaseInsensitive)){
        return new QNitpickerIntegration;
    }

    return 0;
}

Q_IMPORT_PLUGIN(QNitpickerIntegrationPlugin)

QT_END_NAMESPACE
