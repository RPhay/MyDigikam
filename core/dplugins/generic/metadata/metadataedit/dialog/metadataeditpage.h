/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-10-15
 * Description : IPTC subjects settings page.
 *
 * SPDX-FileCopyrightText: 2006-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009      by Andi Clemens <andi dot clemens at googlemail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_META_DATA_EDIT_PAGE_H
#define DIGIKAM_META_DATA_EDIT_PAGE_H

// Qt includes

#include <QScrollArea>

namespace DigikamGenericMetadataEditPlugin
{

class MetadataEditPage : public QScrollArea
{
    Q_OBJECT

public:

    explicit MetadataEditPage(QWidget* const parent);
    ~MetadataEditPage() override = default;
};

} // namespace DigikamGenericMetadataEditPlugin

#endif // DIGIKAM_META_DATA_EDIT_PAGE_H
