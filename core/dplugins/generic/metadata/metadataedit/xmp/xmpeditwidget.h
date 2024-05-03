/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-03-14
 * Description : a Widget to edit XMP metadata
 *
 * SPDX-FileCopyrightText: 2007-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2011 by Victor Dodon <dodon dot victor at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_XMP_EDIT_WIDGET_H
#define DIGIKAM_XMP_EDIT_WIDGET_H

// Local includes

#include "dconfigdlgwidgets.h"

using namespace Digikam;

namespace DigikamGenericMetadataEditPlugin
{

class MetadataEditDialog;

class XMPEditWidget : public DConfigDlgWdg
{
    Q_OBJECT

public:

    explicit XMPEditWidget(MetadataEditDialog* const parent);
    ~XMPEditWidget()                      override;

    bool isModified()               const;
    void saveSettings();
    void apply();

Q_SIGNALS:

    void signalModified();
    void signalSetReadOnly(bool);

public Q_SLOTS:

    void slotModified();
    void slotItemChanged();

private:

    void readSettings();

    int  activePageIndex()          const;
    void showPage(int page);

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace DigikamGenericMetadataEditPlugin

#endif // DIGIKAM_XMP_EDIT_WIDGET_H
