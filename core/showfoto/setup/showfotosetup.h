/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-04-02
 * Description : showFoto setup dialog.
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SHOW_FOTO_SETUP_H
#define SHOW_FOTO_SETUP_H

// Local includes

#include "dconfigdlg.h"

using namespace Digikam;

namespace ShowFoto
{

class ShowfotoSetup : public DConfigDlg
{
    Q_OBJECT

public:

    enum Page
    {
        LastPageUsed = -1,

        EditorPage   = 0,
        MetadataPage,
        ToolTipPage,
        RawPage,
        IOFilesPage,
        ICCPage,
        PluginsPage,
        MiscellaneousPage,

        SetupPageEnumLast
    };

public:

    explicit ShowfotoSetup(QWidget* const parent = nullptr, Page page = LastPageUsed);
    ~ShowfotoSetup() override;

    static bool execMetadataFilters(QWidget* const parent, int tab);

    /**
     * Show a setup dialog. Only the specified page will be available.
     */
    static bool execSinglePage(Page page);
    static bool execSinglePage(QWidget* const parent, Page page);
    static bool execExifTool(QWidget* const parent);

    static void onlineVersionCheck();

private Q_SLOTS:

    void slotOkClicked();
    void slotHelp();

private:

    ShowfotoSetup::Page activePageIndex();
    void showPage(ShowfotoSetup::Page page);

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOW_FOTO_SETUP_H
