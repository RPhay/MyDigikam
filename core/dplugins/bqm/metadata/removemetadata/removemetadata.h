/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-14
 * Description : remove metadata batch tool.
 *
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
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
