/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a BQM plugin to remove metadata
 *
 * SPDX-FileCopyrightText: 2018-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_REMOVE_METADATA_PLUGIN_H
#define DIGIKAM_REMOVE_METADATA_PLUGIN_H

// Local includes

#include "dpluginbqm.h"

#define DPLUGIN_IID "org.kde.digikam.plugin.bqm.RemoveMetadata"

using namespace Digikam;

namespace DigikamBqmRemoveMetadataPlugin
{

class RemoveMetadataPlugin : public DPluginBqm
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DPLUGIN_IID)
    Q_INTERFACES(Digikam::DPluginBqm)

public:

    explicit RemoveMetadataPlugin(QObject* const parent = nullptr);
    ~RemoveMetadataPlugin()              override = default;

    QString name()                 const override;
    QString iid()                  const override;
    QIcon   icon()                 const override;
    QString details()              const override;
    QString description()          const override;
    QList<DPluginAuthor> authors() const override;
    QString handbookSection()      const override;
    QString handbookChapter()      const override;
    QString handbookReference()    const override;

    void setup(QObject* const)           override;
};

} // namespace DigikamBqmRemoveMetadataPlugin

#endif // DIGIKAM_REMOVE_METADATA_PLUGIN_H
