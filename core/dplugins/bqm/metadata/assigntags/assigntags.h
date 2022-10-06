/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-10-03
 * Description : assign tags metadata batch tool.
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_BQM_ASSIGN_TAGS_H
#define DIGIKAM_BQM_ASSIGN_TAGS_H

// Local includes

#include "batchtool.h"

using namespace Digikam;

namespace DigikamBqmAssignTagsPlugin
{

class AssignTags : public BatchTool
{
    Q_OBJECT

public:

    explicit AssignTags(QObject* const parent = nullptr);
    ~AssignTags()                                          override;

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

} // namespace DigikamBqmAssignTagsPlugin

#endif // DIGIKAM_BQM_ASSIGN_TAGS_H
