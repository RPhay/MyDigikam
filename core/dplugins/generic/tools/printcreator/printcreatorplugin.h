/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a plugin to create print compositions.
 *
 * SPDX-FileCopyrightText: 2018-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_PRINT_CREATOR_PLUGIN_H
#define DIGIKAM_PRINT_CREATOR_PLUGIN_H

// Local includes

#include "dplugingeneric.h"

#define DPLUGIN_IID "org.kde.digikam.plugin.generic.PrintCreator"

using namespace Digikam;

namespace DigikamGenericPrintCreatorPlugin
{

class PrintCreatorPlugin : public DPluginGeneric
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DPLUGIN_IID)
    Q_INTERFACES(Digikam::DPluginGeneric)

public:

    explicit PrintCreatorPlugin(QObject* const parent = nullptr);
    ~PrintCreatorPlugin()                override = default;

    QString name()                 const override;
    QString iid()                  const override;
    QIcon   icon()                 const override;
    QString details()              const override;
    QString description()          const override;
    QList<DPluginAuthor> authors() const override;
    QString handbookSection()      const override;
    QString handbookChapter()      const override;

    void setup(QObject* const)           override;

private Q_SLOTS:

    void slotPrintCreator();
};

} // namespace DigikamGenericPrintCreatorPlugin

#endif // DIGIKAM_PRINT_CREATOR_PLUGIN_H
