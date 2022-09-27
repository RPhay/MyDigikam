/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-14
 * Description : remove metadata batch tool.
 *
 * SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_BQM_REMOVE_METADATA_H
#define DIGIKAM_BQM_REMOVE_METADATA_H

// Local includes

#include "batchtool.h"

using namespace Digikam;

namespace DigikamBqmRemoveMetadataPlugin
{

class RemoveMetadata : public BatchTool
{
    Q_OBJECT

public:

    explicit RemoveMetadata(QObject* const parent = nullptr);
    ~RemoveMetadata()                                       override;

    BatchToolSettings defaultSettings()                     override;

    BatchTool* clone(QObject* const parent = nullptr) const override;

    void registerSettingsWidget()                           override;

private Q_SLOTS:

    void slotAssignSettings2Widget()                        override;
    void slotSettingsChanged()                              override;

private:

    bool toolOperations()                                   override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamBqmRemoveMetadataPlugin

#endif // DIGIKAM_BQM_REMOVE_METADATA_H
