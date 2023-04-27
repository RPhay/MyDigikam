/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-02-03
 * Description : digiKam setup dialog.
 *
 * SPDX-FileCopyrightText: 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2003-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "setup.h"

// Qt includes

#include <QPointer>
#include <QApplication>
#include <QMessageBox>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>
#include <ksharedconfig.h>

// Local includes

#include "digikam_debug.h"
#include "loadingcacheinterface.h"
#include "applicationsettings.h"
#include "setupalbumview.h"
#include "setupcamera.h"
#include "setupcollections.h"
#include "setupeditor.h"
#include "setupicc.h"
#include "setuplighttable.h"
#include "setupmetadata.h"
#include "setupmisc.h"
#include "setupimagequalitysorter.h"
#include "setuptooltip.h"
#include "setupdatabase.h"
#include "setupplugins.h"
#include "importsettings.h"
#include "dxmlguiwindow.h"
#include "onlineversiondlg.h"

#ifdef HAVE_MEDIAPLAYER
#   include "setupvideo.h"
#endif

namespace Digikam
{

class Q_DECL_HIDDEN Setup::Private
{
public:

    explicit Private()
      : page_database           (nullptr),
        page_collections        (nullptr),
        page_albumView          (nullptr),
        page_tooltip            (nullptr),

#ifdef HAVE_MEDIAPLAYER

        page_video              (nullptr),

#endif

        page_metadata           (nullptr),
        page_template           (nullptr),
        page_lighttable         (nullptr),
        page_editor             (nullptr),
        page_imagequalitysorter (nullptr),
        page_icc                (nullptr),
        page_camera             (nullptr),
        page_plugins            (nullptr),
        page_misc               (nullptr),
        databasePage            (nullptr),
        collectionsPage         (nullptr),
        albumViewPage           (nullptr),
        tooltipPage             (nullptr),

#ifdef HAVE_MEDIAPLAYER

        videoPage               (nullptr),

#endif

        metadataPage            (nullptr),
        templatePage            (nullptr),
        lighttablePage          (nullptr),
        editorPage              (nullptr),
        imageQualitySorterPage  (nullptr),
        iccPage                 (nullptr),
        cameraPage              (nullptr),
        pluginsPage             (nullptr),
        miscPage                (nullptr)
    {
    }

    DConfigDlgWdgItem*       page_database;
    DConfigDlgWdgItem*       page_collections;
    DConfigDlgWdgItem*       page_albumView;
    DConfigDlgWdgItem*       page_tooltip;

#ifdef HAVE_MEDIAPLAYER

    DConfigDlgWdgItem*       page_video;

#endif

    DConfigDlgWdgItem*       page_metadata;
    DConfigDlgWdgItem*       page_template;
    DConfigDlgWdgItem*       page_lighttable;
    DConfigDlgWdgItem*       page_editor;
    DConfigDlgWdgItem*       page_imagequalitysorter;
    DConfigDlgWdgItem*       page_icc;
    DConfigDlgWdgItem*       page_camera;
    DConfigDlgWdgItem*       page_plugins;
    DConfigDlgWdgItem*       page_misc;

    SetupDatabase*           databasePage;
    SetupCollections*        collectionsPage;
    SetupAlbumView*          albumViewPage;
    SetupToolTip*            tooltipPage;

#ifdef HAVE_MEDIAPLAYER

    SetupVideo*              videoPage;

#endif

    SetupMetadata*           metadataPage;
    SetupTemplate*           templatePage;
    SetupLightTable*         lighttablePage;
    SetupEditor*             editorPage;
    SetupImageQualitySorter* imageQualitySorterPage;
    SetupICC*                iccPage;
    SetupCamera*             cameraPage;
    SetupPlugins*            pluginsPage;
    SetupMisc*               miscPage;

public:

    DConfigDlgWdgItem* pageItem(Setup::Page page) const;
};

Setup::Setup(QWidget* const parent)
    : DConfigDlg(parent),
      d         (new Private)
{
    setWindowFlags((windowFlags() & ~Qt::Dialog) |
                   Qt::Window                    |
                   Qt::WindowCloseButtonHint     |
                   Qt::WindowMinMaxButtonsHint);

    setWindowTitle(i18nc("@title:window", "Configure"));
    setStandardButtons(QDialogButtonBox::Help | QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    button(QDialogButtonBox::Ok)->setDefault(true);
    setFaceType(List);
    setModal(true);

    d->databasePage     = new SetupDatabase();
    d->page_database    = addPage(d->databasePage, i18nc("@title: settings section", "Database"));
    d->page_database->setHeader(i18nc("@title", "Database Settings\nCustomize database settings"));
    d->page_database->setIcon(QIcon::fromTheme(QLatin1String("network-server-database")));

    d->collectionsPage  = new SetupCollections();
    d->page_collections = addPage(d->collectionsPage, i18nc("@title: settings section", "Collections"));
    d->page_collections->setHeader(i18nc("@title", "Collections Settings\nSet root albums locations"));
    d->page_collections->setIcon(QIcon::fromTheme(QLatin1String("folder-pictures")));

    d->albumViewPage  = new SetupAlbumView();
    d->page_albumView = addPage(d->albumViewPage, i18nc("@title: settings section", "Views"));
    d->page_albumView->setHeader(i18nc("@title", "Application Views Settings\nCustomize the look of the views"));
    d->page_albumView->setIcon(QIcon::fromTheme(QLatin1String("view-list-icons")));

    d->tooltipPage  = new SetupToolTip();
    d->page_tooltip = addPage(d->tooltipPage, i18nc("@title: settings section", "Tool-Tip"));
    d->page_tooltip->setHeader(i18nc("@title", "Items Tool-Tip Settings\nCustomize information in item tool-tips"));
    d->page_tooltip->setIcon(QIcon::fromTheme(QLatin1String("dialog-information")));

#ifdef HAVE_MEDIAPLAYER

    d->videoPage  = new SetupVideo();
    d->page_video = addPage(d->videoPage, i18nc("@title: settings section", "Video"));
    d->page_video->setHeader(i18nc("@title", "Video Preview Settings\nCustomize settings to play video media"));
    d->page_video->setIcon(QIcon::fromTheme(QLatin1String("video-x-generic")));

#endif

    d->metadataPage  = new SetupMetadata();
    d->page_metadata = addPage(d->metadataPage, i18nc("@title: settings section", "Metadata"));
    d->page_metadata->setHeader(i18nc("@title", "Embedded Image Information Management\nSetup relations between images and metadata"));
    d->page_metadata->setIcon(QIcon::fromTheme(QLatin1String("format-text-code")));

    d->templatePage  = new SetupTemplate();
    d->page_template = addPage(d->templatePage, i18nc("@title: settings section", "Templates"));
    d->page_template->setHeader(i18nc("@title", "Metadata templates\nManage your collection of metadata templates"));
    d->page_template->setIcon(QIcon::fromTheme(QLatin1String("im-user")));

    d->editorPage  = new SetupEditor();
    d->page_editor = addPage(d->editorPage, i18nc("@title: settings section", "Image Editor"));
    d->page_editor->setHeader(i18nc("@title", "Image Editor Settings\nCustomize the image editor settings"));
    d->page_editor->setIcon(QIcon::fromTheme(QLatin1String("document-edit")));

    d->iccPage  = new SetupICC(buttonBox());
    d->page_icc = addPage(d->iccPage, i18nc("@title: settings section", "Color Management"));
    d->page_icc->setHeader(i18nc("@title", "Settings for Color Management\nCustomize the color management settings"));
    d->page_icc->setIcon(QIcon::fromTheme(QLatin1String("preferences-desktop-display-color")));

    d->lighttablePage  = new SetupLightTable();
    d->page_lighttable = addPage(d->lighttablePage, i18nc("@title: settings section", "Light Table"));
    d->page_lighttable->setHeader(i18nc("@title", "Light Table Settings\nCustomize tool used to compare images"));
    d->page_lighttable->setIcon(QIcon::fromTheme(QLatin1String("lighttable")));

    d->imageQualitySorterPage = new SetupImageQualitySorter();
    d->page_imagequalitysorter = addPage(d->imageQualitySorterPage, i18nc("@title: settings section", "Image Quality Sorter"));
    d->page_imagequalitysorter->setHeader(i18nc("@title", "Image Quality Sorter Settings\nCustomize default settings to perform image triaging by quality"));
    d->page_imagequalitysorter->setIcon(QIcon::fromTheme(QLatin1String("flag-green")));

    d->cameraPage  = new SetupCamera();
    d->page_camera = addPage(d->cameraPage, i18nc("@title: settings section", "Cameras"));
    d->page_camera->setHeader(i18nc("@title", "Camera Settings\nManage your camera devices"));
    d->page_camera->setIcon(QIcon::fromTheme(QLatin1String("camera-photo")));

    connect(d->cameraPage, SIGNAL(signalUseFileMetadataChanged(bool)),
            d->tooltipPage, SLOT(slotUseFileMetadataChanged(bool)));

    d->pluginsPage  = new SetupPlugins();
    d->page_plugins = addPage(d->pluginsPage, i18nc("@title: settings section", "Plugins"));
    d->page_plugins->setHeader(i18nc("@title", "Plug-in Settings\nSet which plugins will be accessible from application"));
    d->page_plugins->setIcon(QIcon::fromTheme(QLatin1String("preferences-plugin")));

    d->miscPage  = new SetupMisc(this);
    d->page_misc = addPage(d->miscPage, i18nc("@title: settings section", "Miscellaneous"));
    d->page_misc->setHeader(i18nc("@title", "Miscellaneous Settings\nCustomize behavior of the other parts of digiKam"));
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

    connect(buttonBox(), SIGNAL(helpRequested()),
            this, SLOT(slotHelp()));

    connect(buttonBox()->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &Setup::slotOkClicked);

    connect(buttonBox()->button(QDialogButtonBox::Cancel), &QPushButton::clicked,
            this, &Setup::slotCancelClicked);

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("Setup Dialog"));

    winId();
    DXmlGuiWindow::setGoodDefaultWindowSize(windowHandle());
    DXmlGuiWindow::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size());
}

Setup::~Setup()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("Setup Dialog"));
    group.writeEntry(QLatin1String("Setup Page"),      (int)activePageIndex());
    group.writeEntry(QLatin1String("Albumview Tab"),   (int)d->albumViewPage->activeTab());
    group.writeEntry(QLatin1String("ToolTip Tab"),     (int)d->tooltipPage->activeTab());

#ifdef HAVE_MEDIAPLAYER

    group.writeEntry(QLatin1String("Video Tab"),       (int)d->videoPage->activeTab());

#endif

    group.writeEntry(QLatin1String("Metadata Tab"),    (int)d->metadataPage->activeTab());
    group.writeEntry(QLatin1String("Metadata SubTab"), (int)d->metadataPage->activeSubTab());
    group.writeEntry(QLatin1String("Editor Tab"),      (int)d->editorPage->activeTab());
    group.writeEntry(QLatin1String("ICC Tab"),         (int)d->iccPage->activeTab());
    group.writeEntry(QLatin1String("Camera Tab"),      (int)d->cameraPage->activeTab());
    group.writeEntry(QLatin1String("Plugin Tab"),      (int)d->pluginsPage->activeTab());
    group.writeEntry(QLatin1String("Misc Tab"),        (int)d->miscPage->activeTab());
    DXmlGuiWindow::saveWindowSize(windowHandle(), group);
    config->sync();

    delete d;
}

void Setup::slotHelp()
{
    openOnlineDocumentation(QLatin1String("setup_application"));
}

void Setup::setTemplate(const Template& t)
{
    if (d->templatePage)
    {
        d->templatePage->setTemplate(t);
    }
}

QSize Setup::sizeHint() const
{
    // The minimum size is very small. But the default initial size is such
    // that some important tabs get a scroll bar, although the dialog could be larger
    // on a normal display (QScrollArea size hint does not take widget into account)
    // Adjust size hint here so that certain selected tabs are display full per default.

    QSize hint          = DConfigDlg::sizeHint();
    int maxHintHeight   = 0;
    int maxWidgetHeight = 0;

    for (int page = 0 ; page != SetupPageEnumLast ; ++page)
    {
        // only take tabs into account here that should better be displayed without scrolling

        if (
            (page == CollectionsPage) ||
            (page == AlbumViewPage)   ||
            (page == TemplatePage)    ||
            (page == LightTablePage)  ||
            (page == EditorPage)      ||
            (page == PluginsPage)     ||
            (page == MiscellaneousPage)
           )
        {
            DConfigDlgWdgItem* const item = d->pageItem((Page)page);

            if (!item)
            {
                continue;
            }

            QWidget* const wdg            = item->widget();
            maxHintHeight                 = qMax(maxHintHeight, wdg->sizeHint().height());
            QScrollArea* const scrollArea = qobject_cast<QScrollArea*>(wdg);

            if (scrollArea)
            {
                maxWidgetHeight = qMax(maxWidgetHeight, scrollArea->widget()->sizeHint().height());
            }
        }
    }

    // The additional 20 is a hack to make it work.
    // Don't know why, the largest page would have scroll bars without this

    if (maxWidgetHeight > maxHintHeight)
    {
        hint.setHeight(hint.height() + (maxWidgetHeight - maxHintHeight) + 20);
    }

    return hint;
}

bool Setup::execDialog(Page page)
{
    return execDialog(nullptr, page);
}

bool Setup::execDialog(QWidget* const parent, Page page)
{
    QPointer<Setup> setup = new Setup(parent);
    setup->showPage(page);
    bool success          = (setup->DConfigDlg::exec() == QDialog::Accepted);
    delete setup;

    return success;
}

bool Setup::execSinglePage(Page page)
{
    return execSinglePage(nullptr, page);
}

bool Setup::execSinglePage(QWidget* const parent, Page page)
{
    QPointer<Setup> setup = new Setup(parent);
    setup->showPage(page);
    setup->setFaceType(Plain);
    bool success          = (setup->DConfigDlg::exec() == QDialog::Accepted);
    delete setup;

    return success;
}

bool Setup::execTemplateEditor(QWidget* const parent, const Template& t)
{
    QPointer<Setup> setup = new Setup(parent);
    setup->showPage(TemplatePage);
    setup->setFaceType(Plain);
    setup->setTemplate(t);
    bool success          = (setup->DConfigDlg::exec() == QDialog::Accepted);
    delete setup;

    return success;
}

bool Setup::execMetadataFilters(QWidget* const parent, int tab)
{
    QPointer<Setup> setup        = new Setup(parent);
    setup->showPage(MetadataPage);
    setup->setFaceType(Plain);

    DConfigDlgWdgItem* const cur = setup->currentPage();

    if (!cur)
    {
        return false;
    }

    SetupMetadata* const widget  = dynamic_cast<SetupMetadata*>(cur->widget());

    if (!widget)
    {
        return false;
    }

    widget->setActiveTab(SetupMetadata::Display);
    widget->setActiveSubTab((SetupMetadata::MetadataSubTab)tab);

    bool success                 = (setup->DConfigDlg::exec() == QDialog::Accepted);
    delete setup;

    return success;
}

bool Setup::execExifTool(QWidget* const parent)
{
    QPointer<Setup> setup        = new Setup(parent);
    setup->showPage(MetadataPage);
    setup->setFaceType(Plain);

    DConfigDlgWdgItem* const cur = setup->currentPage();

    if (!cur)
    {
        return false;
    }

    SetupMetadata* const widget  = dynamic_cast<SetupMetadata*>(cur->widget());

    if (!widget)
    {
        return false;
    }

    widget->setActiveTab(SetupMetadata::ExifTool);

    bool success                 = (setup->DConfigDlg::exec() == QDialog::Accepted);
    delete setup;

    return success;
}

bool Setup::execLocalize(QWidget* const parent)
{
    QPointer<Setup> setup        = new Setup(parent);
    setup->showPage(MiscellaneousPage);
    setup->setFaceType(Plain);

    DConfigDlgWdgItem* const cur = setup->currentPage();

    if (!cur)
    {
        return false;
    }

    SetupMisc* const widget  = dynamic_cast<SetupMisc*>(cur->widget());

    if (!widget)
    {
        return false;
    }

    widget->setActiveTab(SetupMisc::Localize);

    bool success                 = (setup->DConfigDlg::exec() == QDialog::Accepted);
    delete setup;

    return success;
}

bool Setup::execImageQualitySorter(QWidget* const parent)
{
    QPointer<Setup> setup        = new Setup(parent);
    setup->showPage(ImageQualityPage);
    setup->setFaceType(Plain);

    DConfigDlgWdgItem* const cur = setup->currentPage();

    if (!cur)
    {
        return false;
    }

    SetupImageQualitySorter* const widget  = dynamic_cast<SetupImageQualitySorter*>(cur->widget());

    if (!widget)
    {
        return false;
    }

    bool success                 = (setup->DConfigDlg::exec() == QDialog::Accepted);
    delete setup;

    return success;
}

void Setup::slotOkClicked()
{
    if (!d->cameraPage->checkSettings())
    {
        showPage(CameraPage);
        return;
    }

    if (!d->miscPage->checkSettings())
    {
        showPage(MiscellaneousPage);
        return;
    }

    qApp->setOverrideCursor(Qt::WaitCursor);

    d->cameraPage->applySettings();
    d->databasePage->applySettings();
    d->collectionsPage->applySettings();
    d->albumViewPage->applySettings();
    d->tooltipPage->applySettings();

#ifdef HAVE_MEDIAPLAYER

    d->videoPage->applySettings();

#endif

    d->metadataPage->applySettings();
    d->templatePage->applySettings();
    d->lighttablePage->applySettings();
    d->editorPage->applySettings();
    d->imageQualitySorterPage->applySettings();
    d->iccPage->applySettings();
    d->pluginsPage->applySettings();
    d->miscPage->applySettings();

    ApplicationSettings::instance()->emitSetupChanged();
    ImportSettings::instance()->emitSetupChanged();

    qApp->restoreOverrideCursor();

    if (d->metadataPage->exifAutoRotateHasChanged())
    {
        LoadingCacheInterface::cleanThumbnailCache();
    }

    accept();
}

void Setup::slotCancelClicked()
{

#ifdef HAVE_MEDIAPLAYER

    d->videoPage->cancel();

#endif

}

void Setup::showPage(Setup::Page page)
{
    DConfigDlgWdgItem* item = nullptr;

    if (page == LastPageUsed)
    {
        KSharedConfig::Ptr config = KSharedConfig::openConfig();
        KConfigGroup group        = config->group(QLatin1String("Setup Dialog"));

        item = d->pageItem((Page)group.readEntry(QLatin1String("Setup Page"), (int)CollectionsPage));
        d->albumViewPage->setActiveTab((SetupAlbumView::AlbumTab)group.readEntry(QLatin1String("AlbumView Tab"), (int)SetupAlbumView::IconView));
        d->tooltipPage->setActiveTab((SetupToolTip::ToolTipTab)group.readEntry(QLatin1String("ToolTip Tab"), (int)SetupToolTip::IconItems));

#ifdef HAVE_MEDIAPLAYER

        d->videoPage->setActiveTab((SetupVideo::VideoTab)group.readEntry(QLatin1String("Video Tab"), (int)SetupVideo::Decoder));

#endif

        d->metadataPage->setActiveTab((SetupMetadata::MetadataTab)group.readEntry(QLatin1String("Metadata Tab"), (int)SetupMetadata::Behavior));
        d->metadataPage->setActiveSubTab((SetupMetadata::MetadataSubTab)group.readEntry(QLatin1String("Metadata SubTab"), (int)SetupMetadata::ExifViewer));
        d->editorPage->setActiveTab((SetupEditor::EditorTab)group.readEntry(QLatin1String("Editor Tab"), (int)SetupEditor::EditorWindow));
        d->iccPage->setActiveTab((SetupICC::ICCTab)group.readEntry(QLatin1String("ICC Tab"), (int)SetupICC::Behavior));
        d->cameraPage->setActiveTab((SetupCamera::CameraTab)group.readEntry(QLatin1String("Camera Tab"), (int)SetupCamera::Devices));
        d->pluginsPage->setActiveTab((SetupPlugins::PluginTab)group.readEntry(QLatin1String("Plugin Tab"), (int)SetupPlugins::Generic));
        d->miscPage->setActiveTab((SetupMisc::MiscTab)group.readEntry(QLatin1String("Misc Tab"), (int)SetupMisc::Behaviour));
    }
    else
    {
        item = d->pageItem(page);
    }

    if (!item)
    {
        item = d->pageItem(CollectionsPage);
    }

    setCurrentPage(item);
}

Setup::Page Setup::activePageIndex() const
{
    DConfigDlgWdgItem* const cur = currentPage();

    if (cur == d->page_collections)
    {
        return CollectionsPage;
    }

    if (cur == d->page_albumView)
    {
        return AlbumViewPage;
    }

    if (cur == d->page_tooltip)
    {
        return ToolTipPage;
    }

#ifdef HAVE_MEDIAPLAYER

    if (cur == d->page_video)
    {
        return VideoPage;
    }

#endif

    if (cur == d->page_metadata)
    {
        return MetadataPage;
    }

    if (cur == d->page_template)
    {
        return TemplatePage;
    }

    if (cur == d->page_lighttable)
    {
        return LightTablePage;
    }

    if (cur == d->page_editor)
    {
        return EditorPage;
    }

    if (cur == d->page_imagequalitysorter)
    {
        return ImageQualityPage;
    }

    if (cur == d->page_icc)
    {
        return ICCPage;
    }

    if (cur == d->page_camera)
    {
        return CameraPage;
    }

    if (cur == d->page_plugins)
    {
        return PluginsPage;
    }

    if (cur == d->page_misc)
    {
        return MiscellaneousPage;
    }

    return DatabasePage;
}

DConfigDlgWdgItem* Setup::Private::pageItem(Setup::Page page) const
{
    switch (page)
    {
        case Setup::DatabasePage:
            return page_database;

        case Setup::CollectionsPage:
            return page_collections;

        case Setup::AlbumViewPage:
            return page_albumView;

        case Setup::ToolTipPage:
            return page_tooltip;

#ifdef HAVE_MEDIAPLAYER

        case Setup::VideoPage:
            return page_video;

#endif

        case Setup::MetadataPage:
            return page_metadata;

        case Setup::TemplatePage:
            return page_template;

        case Setup::LightTablePage:
            return page_lighttable;

        case Setup::EditorPage:
            return page_editor;

        case Setup::ImageQualityPage:
            return page_imagequalitysorter;

        case Setup::ICCPage:
            return page_icc;

        case Setup::CameraPage:
            return page_camera;

        case Setup::PluginsPage:
            return page_plugins;

        case Setup::MiscellaneousPage:
            return page_misc;

        default:
            return nullptr;
    }
}

void Setup::onlineVersionCheck()
{
    OnlineVersionDlg* const dlg = new OnlineVersionDlg(qApp->activeWindow(),
                                                       QLatin1String(digikam_version_short),
                                                       digiKamBuildDate(),
                                                       ApplicationSettings::instance()->getUpdateType(),
                                                       ApplicationSettings::instance()->getUpdateWithDebug());

    connect(dlg, &OnlineVersionDlg::signalSetupUpdate,
            [=]()
        {
            Setup::execSinglePage(nullptr, Setup::MiscellaneousPage);
        }
    );

    dlg->exec();
}

} // namespace Digikam
