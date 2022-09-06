/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a BQM plugin to reduce red eyes
 *
 * Copyright (C) 2018-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_RED_EYE_CORRECTION_PLUGIN_H
#define DIGIKAM_RED_EYE_CORRECTION_PLUGIN_H

// Local includes

#include "dpluginbqm.h"

#define DPLUGIN_IID "org.kde.digikam.plugin.bqm.RedEyeCorrection"

using namespace Digikam;

namespace DigikamBqmRestorationPlugin
{

class RedEyeCorrectionPlugin : public DPluginBqm
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DPLUGIN_IID)
    Q_INTERFACES(Digikam::DPluginBqm)

public:

    explicit RedEyeCorrectionPlugin(QObject* const parent = nullptr);
    ~RedEyeCorrectionPlugin()            override;

    QString name()                 const override;
    QString iid()                  const override;
    QIcon   icon()                 const override;
    QString details()              const override;
    QString description()          const override;
    QList<DPluginAuthor> authors() const override;

    void setup(QObject* const)           override;
};

} // namespace DigikamBqmRestorationPlugin

#endif // DIGIKAM_RED_EYE_CORRECTION_PLUGIN_H
