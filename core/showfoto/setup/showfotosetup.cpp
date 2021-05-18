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

#include "showfotosetup.h"

// Qt includes

#include <QPointer>
#include <QPushButton>
#include <QApplication>

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <klocalizedstring.h>

// Local includes

#include "setupeditoriface.h"
#include "setupicc.h"
#include "setupiofiles.h"
#include "showfotosettings.h"
#include "showfotosetupraw.h"
#include "showfotosetupmisc.h"
#include "showfotosetupmetadata.h"
#include "showfotosetuptooltip.h"
#include "showfotosetupplugins.h"
#include "dxmlguiwindow.h"
#include "onlineversiondlg.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoSetup::Private
{
public:

    explicit Private()
      : page_editorIface(nullptr),
        page_metadata   (nullptr),
        page_tooltip    (nullptr),
        page_raw        (nullptr),
        page_iofiles    (nullptr),
        page_icc        (nullptr),
        page_plugins    (nullptr),
        page_misc       (nullptr),
        metadataPage    (nullptr),
        toolTipPage     (nullptr),
        miscPage        (nullptr),
        rawPage         (nullptr),
        pluginsPage     (nullptr),
        editorIfacePage (nullptr),
        iofilesPage     (nullptr),
        iccPage         (nullptr)
    {
    }

    DConfigDlgWdgItem*            page_editorIface;
    DConfigDlgWdgItem*            page_metadata;
    DConfigDlgWdgItem*            page_tooltip;
    DConfigDlgWdgItem*            page_raw;
    DConfigDlgWdgItem*            page_iofiles;
    DConfigDlgWdgItem*            page_icc;
    DConfigDlgWdgItem*            page_plugins;
    DConfigDlgWdgItem*            page_misc;

    ShowfotoSetupMetadata*        metadataPage;
    ShowfotoSetupToolTip*                 toolTipPage;
    ShowfotoSetupMisc*            miscPage;
    ShowfotoSetupRaw*                     rawPage;
    ShowfotoSetupPlugins*                 pluginsPage;

    Digikam::SetupEditorIface*    editorIfacePage;
    Digikam::SetupIOFiles*        iofilesPage;
    Digikam::SetupICC*            iccPage;

public:

    DConfigDlgWdgItem* pageItem(ShowfotoSetup::Page page) const;
};

ShowfotoSetup::ShowfotoSetup(QWidget* const parent, ShowfotoSetup::Page page)
    : DConfigDlg(parent),
      d         (new Private)
{
    setWindowTitle(i18n("Configure"));
    setStandardButtons(QDialogButtonBox::Help | QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    button(QDialogButtonBox::Ok)->setDefault(true);
    setFaceType(DConfigDlg::List);
    setModal(true);

    d->editorIfacePage     = new Digikam::SetupEditorIface();
    d->page_editorIface    = addPage(d->editorIfacePage, i18n("Editor Window"));
    d->page_editorIface->setHeader(i18n("<qt>Editor Window Settings<br/>"
                                   "<i>Customize editor window behavior</i></qt>"));
    d->page_editorIface->setIcon(QIcon::fromTheme(QLatin1String("document-edit")));

    d->metadataPage   = new ShowfotoSetupMetadata();
    d->page_metadata  = addPage(d->metadataPage, i18n("Metadata"));
    d->page_metadata->setHeader(i18n("<qt>Embedded Image Information Management<br/>"
                                     "<i>Setup relations between images and metadata</i></qt>"));
    d->page_metadata->setIcon(QIcon::fromTheme(QLatin1String("format-text-code"))); // krazy:exclude=iconnames

    d->toolTipPage    = new ShowfotoSetupToolTip();
    d->page_tooltip   = addPage(d->toolTipPage, i18n("Tool Tip"));
    d->page_tooltip->setHeader(i18n("<qt>Thumbbar Items Tool-Tip Settings<br/>"
                                    "<i>Customize information in tool-tips</i></qt>"));
    d->page_tooltip->setIcon(QIcon::fromTheme(QLatin1String("dialog-information")));

    d->rawPage        = new ShowfotoSetupRaw();
    d->page_raw       = addPage(d->rawPage, i18n("RAW Decoding"));
    d->page_raw->setHeader(i18n("<qt>RAW Files Decoding Settings<br/>"
                                  "<i>Customize default RAW decoding settings</i></qt>"));
    d->page_raw->setIcon(QIcon::fromTheme(QLatin1String("image-x-adobe-dng")));

    d->iccPage        = new Digikam::SetupICC(buttonBox());
    d->page_icc       = addPage(d->iccPage, i18n("Color Management"));
    d->page_icc->setHeader(i18n("<qt>Settings for Color Management<br/>"
                                "<i>Customize color management settings</i></qt>"));
    d->page_icc->setIcon(QIcon::fromTheme(QLatin1String("preferences-desktop-display-color")));

    d->iofilesPage    = new Digikam::SetupIOFiles();
    d->page_iofiles   = addPage(d->iofilesPage, i18n("Save Images"));
    d->page_iofiles->setHeader(i18n("<qt>Settings for Saving Image Files<br/>"
                                    "<i>Set default configuration used to save images</i></qt>"));
    d->page_iofiles->setIcon(QIcon::fromTheme(QLatin1String("document-save-all")));

    d->pluginsPage  = new ShowfotoSetupPlugins();
    d->page_plugins = addPage(d->pluginsPage, i18n("Plugins"));
    d->page_plugins->setHeader(i18n("<qt>Main Interface Plug-in Settings<br/>"
                                    "<i>Set which plugins will be accessible from application</i></qt>"));
    d->page_plugins->setIcon(QIcon::fromTheme(QLatin1String("preferences-plugin")));

    d->miscPage       = new ShowfotoSetupMisc(this);
    d->page_misc      = addPage(d->miscPage, i18nc("@title: misc setup options page", "Miscellaneous"));
    d->page_misc->setHeader(i18n("<qt>Miscellaneous Settings<br/>"
                                 "<i>Customize behavior of the other parts of Showfoto</i></qt>"));
    d->page_misc->setIcon(QIcon::fromTheme(QLatin1String("preferences-other")));

    for (int i = 0 ; i != SetupPageEnumLast ; ++i)
    {
        DConfigDlgWdgItem* const item = d->pageItem((Page)i);

        if (!item)
        {
            continue;
        }

        QWidget* const wgt            = item->widget();
        QScrollArea* const scrollArea = qobject_cast<QScrollArea*>(wgt);

        if (scrollArea)
        {
            scrollArea->setFrameShape(QFrame::NoFrame);
        }
    }

    connect(buttonBox()->button(QDialogButtonBox::Ok),
            &QPushButton::clicked, this, &ShowfotoSetup::slotOkClicked);

    connect(buttonBox(), SIGNAL(helpRequested()),
            this, SLOT(slotHelp()));

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("Setup Dialog"));

    if (page != LastPageUsed)
    {
        showPage(page);
    }
    else
    {
        showPage((Page)group.readEntry(QLatin1String("Setup Page"), (int)EditorPage));
    }

    winId();
    Digikam::DXmlGuiWindow::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size());
}

ShowfotoSetup::~ShowfotoSetup()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("Setup Dialog"));
    group.writeEntry(QLatin1String("Setup Page"), (int)activePageIndex());
    Digikam::DXmlGuiWindow::saveWindowSize(windowHandle(), group);
    config->sync();
    delete d;
}

void ShowfotoSetup::slotHelp()
{
    Digikam::DXmlGuiWindow::openHandbook();
}

void ShowfotoSetup::slotOkClicked()
{
    if (!d->miscPage->checkSettings())
    {
        showPage(MiscellaneousPage);
        return;
    }

    qApp->setOverrideCursor(Qt::WaitCursor);

    d->editorIfacePage->applySettings();
    d->metadataPage->applySettings();
    d->toolTipPage->applySettings();
    d->rawPage->applySettings();
    d->iofilesPage->applySettings();
    d->iccPage->applySettings();
    d->pluginsPage->applySettings();
    d->miscPage->applySettings();

    qApp->restoreOverrideCursor();

    accept();
}

void ShowfotoSetup::showPage(ShowfotoSetup::Page page)
{
    switch (page)
    {
        case ToolTipPage:
            setCurrentPage(d->page_tooltip);
            break;

        case RawPage:
            setCurrentPage(d->page_raw);
            break;

        case IOFilesPage:
            setCurrentPage(d->page_iofiles);
            break;

        case ICCPage:
            setCurrentPage(d->page_icc);
            break;

        case MetadataPage:
            setCurrentPage(d->page_metadata);
            break;

        case PluginsPage:
            setCurrentPage(d->page_plugins);
            break;

        case MiscellaneousPage:
            setCurrentPage(d->page_misc);
            break;

        default:
            setCurrentPage(d->page_editorIface);
            break;
    }
}

ShowfotoSetup::Page ShowfotoSetup::activePageIndex()
{
    DConfigDlgWdgItem* const cur = currentPage();

    if (cur == d->page_tooltip)
    {
        return ToolTipPage;
    }

    if (cur == d->page_raw)
    {
        return RawPage;
    }

    if (cur == d->page_iofiles)
    {
        return IOFilesPage;
    }

    if (cur == d->page_icc)
    {
        return ICCPage;
    }

    if (cur == d->page_metadata)
    {
        return MetadataPage;
    }

    if (cur == d->page_plugins)
    {
        return PluginsPage;
    }

    if (cur == d->page_misc)
    {
        return MiscellaneousPage;
    }

    return EditorPage;
}

DConfigDlgWdgItem* ShowfotoSetup::Private::pageItem(ShowfotoSetup::Page page) const
{
    switch (page)
    {
        case ShowfotoSetup::EditorPage:
            return page_editorIface;

        case ShowfotoSetup::MetadataPage:
            return page_metadata;

        case ShowfotoSetup::ToolTipPage:
            return page_tooltip;

        case ShowfotoSetup::RawPage:
            return page_raw;

        case ShowfotoSetup::IOFilesPage:
            return page_iofiles;

        case ShowfotoSetup::ICCPage:
            return page_icc;

        case ShowfotoSetup::PluginsPage:
            return page_plugins;

        case ShowfotoSetup::MiscellaneousPage:
            return page_misc;

        default:
            return nullptr;
    }
}

bool ShowfotoSetup::execSinglePage(Page page)
{
    return (execSinglePage(nullptr, page));
}

bool ShowfotoSetup::execSinglePage(QWidget* const parent, Page page)
{
    QPointer<ShowfotoSetup> setup = new ShowfotoSetup(parent);
    setup->showPage(page);
    setup->setFaceType(Plain);

    bool success          = (setup->DConfigDlg::exec() == QDialog::Accepted);
    delete setup;

    return success;
}

bool ShowfotoSetup::execMetadataFilters(QWidget* const parent, int tab)
{
    QPointer<ShowfotoSetup> setup = new ShowfotoSetup(parent);
    setup->showPage(MetadataPage);
    setup->setFaceType(Plain);

    DConfigDlgWdgItem* const cur  = setup->currentPage();

    if (!cur)
    {
        return false;
    }

    ShowfotoSetupMetadata* const widget = dynamic_cast<ShowfotoSetupMetadata*>(cur->widget());

    if (!widget)
    {
        return false;
    }

    widget->setActiveTab((ShowfotoSetupMetadata::MetadataTab)tab);

    bool success                = (setup->DConfigDlg::exec() == QDialog::Accepted);
    delete setup;

    return success;
}

bool ShowfotoSetup::execExifTool(QWidget* const parent)
{
    QPointer<ShowfotoSetup> setup        = new ShowfotoSetup(parent);
    setup->showPage(MetadataPage);
    setup->setFaceType(Plain);

    DConfigDlgWdgItem* const cur = setup->currentPage();

    if (!cur)
    {
        return false;
    }

    ShowfotoSetupMetadata* const widget  = dynamic_cast<ShowfotoSetupMetadata*>(cur->widget());

    if (!widget)
    {
        return false;
    }

    widget->setActiveTab(ShowfotoSetupMetadata::ExifTool);

    bool success                 = (setup->DConfigDlg::exec() == QDialog::Accepted);
    delete setup;

    return success;
}

void ShowfotoSetup::onlineVersionCheck()
{
    Digikam::OnlineVersionDlg* const dlg = new Digikam::OnlineVersionDlg(qApp->activeWindow(),
                                                                         QLatin1String(digikam_version_short),
                                                                         Digikam::digiKamBuildDate(),
                                                                         ShowfotoSettings::instance()->getUpdateType(),
                                                                         ShowfotoSettings::instance()->getUpdateWithDebug());

    connect(dlg, &OnlineVersionDlg::signalSetupUpdate,
            [=]()
        {
            ShowfotoSetup::execSinglePage(nullptr, ShowfotoSetup::MiscellaneousPage);
        }
    );

    dlg->exec();
}

} // namespace ShowFoto
