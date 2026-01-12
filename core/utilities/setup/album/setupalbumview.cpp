/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-02-01
 * Description : album view configuration setup tab
 *
 * SPDX-FileCopyrightText: 2003-2004 by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2005-2026 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "setupalbumview.h"

// Qt includes

#include <QButtonGroup>
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QStyle>
#include <QComboBox>
#include <QApplication>
#include <QMessageBox>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "digikam_globals.h"
#include "digikam_debug.h"
#include "thumbnailsize.h"
#include "applicationsettings.h"
#include "dfontselect.h"
#include "dexpanderbox.h"
#include "fullscreensettings.h"
#include "dxmlguiwindow.h"
#include "previewsettings.h"
#include "previewosdsettings.h"
#include "previewosdwidget.h"
#include "setupcategory.h"
#include "setupmime.h"

namespace Digikam
{

class Q_DECL_HIDDEN SetupAlbumView::Private
{
public:

    Private() = default;

public:

    bool                useLargeThumbsOriginal          = false;
    bool                useLargeThumbsShowedInfo        = false;

    QCheckBox*          iconShowNameBox                 = nullptr;
    QCheckBox*          iconShowSizeBox                 = nullptr;
    QCheckBox*          iconShowDateBox                 = nullptr;
    QCheckBox*          iconShowModDateBox              = nullptr;
    QCheckBox*          iconShowResolutionBox           = nullptr;
    QCheckBox*          iconShowAspectRatioBox          = nullptr;
    QCheckBox*          iconShowTitleBox                = nullptr;
    QCheckBox*          iconShowCommentsBox             = nullptr;
    QCheckBox*          iconShowTagsBox                 = nullptr;
    QCheckBox*          iconShowOverlaysBox             = nullptr;
    QCheckBox*          iconShowFullscreenBox           = nullptr;
    QCheckBox*          iconShowRatingBox               = nullptr;
    QCheckBox*          iconShowPickLabelBox            = nullptr;
    QCheckBox*          iconShowColorLabelBox           = nullptr;
    QCheckBox*          iconShowFormatBox               = nullptr;
    QCheckBox*          iconShowCoordinatesBox          = nullptr;
    QRadioButton*       previewFastPreview              = nullptr;
    QRadioButton*       previewFullView                 = nullptr;
    QComboBox*          previewRawMode                  = nullptr;
    QComboBox*          magnifierZoomLevel              = nullptr;
    QCheckBox*          previewConvertToEightBit        = nullptr;
    QCheckBox*          previewScaleFitToWindow         = nullptr;
    QCheckBox*          previewSmoothScaled             = nullptr;
    QCheckBox*          previewShowIcons                = nullptr;
    QCheckBox*          previewAutoPlay                 = nullptr;
    QCheckBox*          previewOverlay                  = nullptr;
    QCheckBox*          showFolderTreeViewItemsCount    = nullptr;
    QCheckBox*          largeThumbsBox                  = nullptr;

    QComboBox*          iconTreeThumbSize               = nullptr;
    QComboBox*          iconTreeFaceSize                = nullptr;
    QComboBox*          leftClickActionComboBox         = nullptr;

    QTabWidget*         tab                             = nullptr;

    DFontSelect*        iconViewFontSelect              = nullptr;
    DFontSelect*        treeViewFontSelect              = nullptr;

    FullScreenSettings* fullScreenSettings              = nullptr;

    PreviewOsdSettings  osdSettings;
    PreviewOsdWidget*   osdWidget                       = nullptr;

    SetupCategory*      category                        = nullptr;
    SetupMime*          mimetype                        = nullptr;
};

SetupAlbumView::SetupAlbumView(QWidget* const parent)
    : QScrollArea(parent),
      d          (new Private)
{
    d->tab                     = new QTabWidget(viewport());
    setWidget(d->tab);
    setWidgetResizable(true);

    QWidget* const iwpanel     = new QWidget(d->tab);
    const int spacing          = layoutSpacing();

    // --------------------------------------------------------

    QGridLayout* const grid    = new QGridLayout(iwpanel);

    d->iconShowNameBox         = new QCheckBox(i18n("Show file&name"), iwpanel);
    d->iconShowNameBox->setWhatsThis(i18n("Set this option to show the filename below the thumbnail."));

    d->iconShowSizeBox         = new QCheckBox(i18n("Show file si&ze"), iwpanel);
    d->iconShowSizeBox->setWhatsThis(i18n("Set this option to show the file size below the thumbnail."));

    d->iconShowDateBox         = new QCheckBox(i18n("Show camera creation &date"), iwpanel);
    d->iconShowDateBox->setWhatsThis(i18n("Set this option to show the camera creation date "
                                          "below the thumbnail."));

    d->iconShowModDateBox      = new QCheckBox(i18n("Show file &modification date"), iwpanel);
    d->iconShowModDateBox->setWhatsThis(i18n("Set this option to show the file modification date "
                                             "below the thumbnail if it is different than camera creation date. "
                                             "This option is useful to identify quickly which items have been modified."));

    d->iconShowResolutionBox   = new QCheckBox(i18n("Show item dimensions"), iwpanel);
    d->iconShowResolutionBox->setWhatsThis(i18n("Set this option to show the item size in pixels "
                                                "below the thumbnail."));

    d->iconShowAspectRatioBox  = new QCheckBox(i18n("Show item aspect ratio"), iwpanel);
    d->iconShowAspectRatioBox->setWhatsThis(i18n("Set this option to show the item aspect ratio "
                                                "below the thumbnail."));

    d->iconShowFormatBox       = new QCheckBox(i18n("Show item format"), iwpanel);
    d->iconShowFormatBox->setWhatsThis(i18n("Set this option to show item mime type over thumbnail."));

    d->iconShowTitleBox        = new QCheckBox(i18n("Show digiKam tit&le"), iwpanel);
    d->iconShowTitleBox->setWhatsThis(i18n("Set this option to show the digiKam title "
                                           "below the thumbnail."));

    d->iconShowCommentsBox     = new QCheckBox(i18n("Show digiKam &captions"), iwpanel);
    d->iconShowCommentsBox->setWhatsThis(i18n("Set this option to show the digiKam captions "
                                              "below the thumbnail."));

    d->iconShowTagsBox         = new QCheckBox(i18n("Show digiKam &tags"), iwpanel);
    d->iconShowTagsBox->setWhatsThis(i18n("Set this option to show the digiKam tags list "
                                          "below the thumbnail."));

    d->iconShowRatingBox       = new QCheckBox(i18n("Show digiKam &rating"), iwpanel);
    d->iconShowRatingBox->setWhatsThis(i18n("Set this option to show the digiKam rating "
                                            "below the thumbnail."));

    d->iconShowPickLabelBox    = new QCheckBox(i18n("Show digiKam &pick label"), iwpanel);
    d->iconShowPickLabelBox->setWhatsThis(i18n("Set this option to show the digiKam pick label "
                                            "below the thumbnail."));

    d->iconShowColorLabelBox   = new QCheckBox(i18n("Show digiKam color &label"), iwpanel);
    d->iconShowColorLabelBox->setWhatsThis(i18n("Set this option to show the digiKam color label "
                                                "as a line below the thumbnail."));

    DLineWidget* const line    = new DLineWidget(Qt::Horizontal, iwpanel);

    d->iconShowOverlaysBox     = new QCheckBox(i18n("Show rotation overlay buttons"), iwpanel);
    d->iconShowOverlaysBox->setWhatsThis(i18n("Set this option to show the transformation overlay buttons over "
                                              "the thumbnail to process left or right rotation."));

    d->iconShowFullscreenBox   = new QCheckBox(i18n("Show fullscreen overlay button"), iwpanel);
    d->iconShowFullscreenBox->setWhatsThis(i18n("Set this option to show an overlay button over the "
                                                "thumbnail to open it in fullscreen mode."));

    d->iconShowCoordinatesBox  = new QCheckBox(i18n("Show geolocation overlay button"), iwpanel);
    d->iconShowCoordinatesBox->setWhatsThis(i18n("Set this option to show an overlay button over the "
                                                 "thumbnail to identify geolocated item and open the map tab."));

    DLineWidget* const line2   = new DLineWidget(Qt::Horizontal, iwpanel);

    QLabel* leftClickLabel     = new QLabel(i18n("Thumbnail click action:"), iwpanel);
    d->leftClickActionComboBox = new QComboBox(iwpanel);
    d->leftClickActionComboBox->addItem(i18n("Show Preview"),                  ApplicationSettings::ShowPreview);
    d->leftClickActionComboBox->addItem(i18n("Start Image Editor"),            ApplicationSettings::StartEditor);
    d->leftClickActionComboBox->addItem(i18n("Show on Light Table"),           ApplicationSettings::ShowOnTable);
    d->leftClickActionComboBox->addItem(i18n("Open with Default Application"), ApplicationSettings::OpenDefault);
    d->leftClickActionComboBox->setToolTip(i18n("Choose what should happen when you click on a thumbnail."));

    d->iconViewFontSelect      = new DFontSelect(i18n("Icon View font:"), iwpanel);
    d->iconViewFontSelect->setToolTip(i18n("Select here the font used to display text in icon views."));

    d->largeThumbsBox          = new QCheckBox(i18n("Use large thumbnail size for high screen resolution"), iwpanel);
    d->largeThumbsBox->setWhatsThis(i18n("Set this option to render icon-view with large thumbnail size, for example in case of 4K monitor is used.\n"
                                         "By default this option is turned off and the maximum thumbnail size is limited to 256x256 pixels. "
                                         "When this option is enabled, thumbnail size can be extended to 512x512 pixels.\n"
                                         "This option will store more data in thumbnail database and will use more system memory. "
                                         "digiKam needs to be restarted to take effect, and Rebuild Thumbnails option from Maintenance tool "
                                         "needs to be processed over whole collections."));

    grid->addWidget(d->iconShowNameBox,          0,  0, 1, 1);
    grid->addWidget(d->iconShowSizeBox,          1,  0, 1, 1);
    grid->addWidget(d->iconShowDateBox,          2,  0, 1, 1);
    grid->addWidget(d->iconShowModDateBox,       3,  0, 1, 1);
    grid->addWidget(d->iconShowResolutionBox,    4,  0, 1, 1);
    grid->addWidget(d->iconShowAspectRatioBox,   5,  0, 1, 1);
    grid->addWidget(d->iconShowFormatBox,        6,  0, 1, 1);

    grid->addWidget(d->iconShowTitleBox,         0,  1, 1, 1);
    grid->addWidget(d->iconShowCommentsBox,      1,  1, 1, 1);
    grid->addWidget(d->iconShowTagsBox,          2,  1, 1, 1);
    grid->addWidget(d->iconShowRatingBox,        3,  1, 1, 1);
    grid->addWidget(d->iconShowPickLabelBox,     4,  1, 1, 1);
    grid->addWidget(d->iconShowColorLabelBox,    5,  1, 1, 1);

    grid->addWidget(line,                        7,  0, 1, 2);
    grid->addWidget(d->iconShowCoordinatesBox,   8,  0, 1, 1);
    grid->addWidget(d->iconShowFullscreenBox,    8,  1, 1, 1);
    grid->addWidget(d->iconShowOverlaysBox,      9,  0, 1, 1);
    grid->addWidget(line2,                       10, 0, 1, 2);

    grid->addWidget(leftClickLabel,              11, 0, 1, 1);
    grid->addWidget(d->leftClickActionComboBox,  11, 1, 1, 1);
    grid->addWidget(d->iconViewFontSelect,       12, 0, 1, 2);
    grid->addWidget(d->largeThumbsBox,           13, 0, 1, 2);

    grid->setRowMinimumHeight(8, 10); // vertical space
    grid->setContentsMargins(spacing, spacing, spacing, spacing);
    grid->setSpacing(spacing);
    grid->setRowStretch(14, 10);

    d->tab->insertTab(IconView, iwpanel, i18nc("@title:tab", "Icons"));

    // --------------------------------------------------------

    QWidget* const fwpanel           = new QWidget(d->tab);
    QGridLayout* const grid2         = new QGridLayout(fwpanel);

    QLabel* const iconTreeThumbLabel = new QLabel(i18n("Tree View icon size:"), fwpanel);
    d->iconTreeThumbSize             = new QComboBox(fwpanel);
    d->iconTreeThumbSize->addItem(QLatin1String("16"), 16);
    d->iconTreeThumbSize->addItem(QLatin1String("22"), 22);
    d->iconTreeThumbSize->addItem(QLatin1String("32"), 32);
    d->iconTreeThumbSize->addItem(QLatin1String("48"), 48);
    d->iconTreeThumbSize->addItem(QLatin1String("64"), 64);
    d->iconTreeThumbSize->setToolTip(i18n("Set this option to configure the size in pixels of "
                                          "the Tree View icons in digiKam's sidebars."));

    QLabel* const iconTreeFaceLabel  = new QLabel(i18n("People Tree View icon size:"), fwpanel);
    d->iconTreeFaceSize              = new QComboBox(fwpanel);
    d->iconTreeFaceSize->addItem(QLatin1String("16"),   16);
    d->iconTreeFaceSize->addItem(QLatin1String("22"),   22);
    d->iconTreeFaceSize->addItem(QLatin1String("32"),   32);
    d->iconTreeFaceSize->addItem(QLatin1String("48"),   48);
    d->iconTreeFaceSize->addItem(QLatin1String("64"),   64);
    d->iconTreeFaceSize->addItem(QLatin1String("128"), 128);
    d->iconTreeFaceSize->setToolTip(i18n("Set this option to configure the size in pixels of "
                                         "the Tree View icons in digiKam's people sidebar."));

    d->treeViewFontSelect            = new DFontSelect(i18n("Tree View font:"), fwpanel);
    d->treeViewFontSelect->setToolTip(i18n("Select here the font used to display text in Tree Views."));

    d->showFolderTreeViewItemsCount  = new QCheckBox(i18n("Show a count of items in Tree Views"), fwpanel);
    d->showFolderTreeViewItemsCount->setToolTip(i18n("Set this option to display along the album name the number of icon-view items inside."));

    grid2->addWidget(iconTreeThumbLabel,              0, 0, 1, 1);
    grid2->addWidget(d->iconTreeThumbSize,            0, 1, 1, 1);
    grid2->addWidget(iconTreeFaceLabel,               1, 0, 1, 1);
    grid2->addWidget(d->iconTreeFaceSize,             1, 1, 1, 1);
    grid2->addWidget(d->treeViewFontSelect,           2, 0, 1, 2);
    grid2->addWidget(d->showFolderTreeViewItemsCount, 3, 0, 1, 2);
    grid2->setContentsMargins(spacing, spacing, spacing, spacing);
    grid2->setSpacing(spacing);
    grid2->setRowStretch(4, 10);

    d->tab->insertTab(FolderView, fwpanel, i18nc("@title:tab", "Tree-Views"));

    // --------------------------------------------------------

    QWidget* const pwpanel        = new QWidget(d->tab);
    QGridLayout* const grid3      = new QGridLayout(pwpanel);

    d->previewFastPreview         = new QRadioButton(i18nc("@option:radio",
                                                           "Preview shows embedded view if available (faster)"));
    d->previewFullView            = new QRadioButton(i18nc("@option:radio",
                                                           "Preview shows the full image"));
    QLabel* const rawPreviewLabel = new QLabel(i18nc("@label:listbox Mode of RAW preview decoding",
                                                     "Raw images:"));
    d->previewRawMode             = new QComboBox;
    d->previewRawMode->addItem(i18nc("@item Automatic choice of RAW image preview source",
                                     "Automatic"), PreviewSettings::RawPreviewAutomatic);
    d->previewRawMode->addItem(i18nc("@item Embedded preview as RAW image preview source",
                                     "Embedded preview"), PreviewSettings::RawPreviewFromEmbeddedPreview);
    d->previewRawMode->addItem(i18nc("@item Original, half-size data as RAW image preview source",
                                     "Raw data in half size"), PreviewSettings::RawPreviewFromRawHalfSize);
    d->previewRawMode->addItem(i18nc("@item Original, full-size data as RAW image preview source",
                                     "Raw data in full size (high quality)"), PreviewSettings::RawPreviewFromRawFullSize);

    d->previewConvertToEightBit   = new QCheckBox(i18n("Preview image is converted to 8 bits for a faster viewing"), pwpanel);
    d->previewConvertToEightBit->setWhatsThis(i18n("Check this if you want to convert a 16 bits preview image to 8 bits."));

    d->previewScaleFitToWindow    = new QCheckBox(i18n("Preview image is always scaled to fit to window"), pwpanel);
    d->previewScaleFitToWindow->setWhatsThis(i18n("Check this if you want to scale small images to fit to window."));

    d->previewSmoothScaled        = new QCheckBox(i18n("Preview image use Anti-aliasing to scale up"), pwpanel);
    d->previewSmoothScaled->setWhatsThis(i18n("Check this if you want to use Anti-aliasing to scale up."));

    QLabel* const magnifierLabel  = new QLabel(i18nc("@label:listbox Preview magnifier tool zoom factor",
                                                     "Magnifier Zoom Factor:"));
    d->magnifierZoomLevel         = new QComboBox;
    d->magnifierZoomLevel->addItem(QLatin1String("x1.5"), 1.5);
    d->magnifierZoomLevel->addItem(QLatin1String("x2.0"), 2.0);
    d->magnifierZoomLevel->addItem(QLatin1String("x2.5"), 2.5);
    d->magnifierZoomLevel->addItem(QLatin1String("x3.0"), 3.0);
    d->magnifierZoomLevel->addItem(QLatin1String("x3.5"), 3.5);
    d->magnifierZoomLevel->addItem(QLatin1String("x4.0"), 4.0);

    d->previewShowIcons           = new QCheckBox(i18n("Show icons and text over preview"), pwpanel);
    d->previewShowIcons->setWhatsThis(i18n("Check this if you want to see icons and text in the image preview."));

    d->previewAutoPlay            = new QCheckBox(i18n("Auto play of videos or animations"), pwpanel);
    d->previewAutoPlay->setWhatsThis(i18n("Check this if you want to auto play the videos or animations."));

    d->previewOverlay             = new QCheckBox(i18n("Show a text overlay over preview."), pwpanel);
    d->previewOverlay->setWhatsThis(i18n("Check this if you want to show a canvas overlay with technical shot settings and properties."));

    d->osdWidget                  = new PreviewOsdWidget(&d->osdSettings, pwpanel);

    grid3->setContentsMargins(spacing, spacing, spacing, spacing);
    grid3->setSpacing(spacing);
    grid3->addWidget(d->previewFastPreview,       0, 0, 1, 2);
    grid3->addWidget(d->previewFullView,          1, 0, 1, 2);
    grid3->addWidget(rawPreviewLabel,             2, 0, 1, 1);
    grid3->addWidget(d->previewRawMode,           2, 1, 1, 1);
    grid3->addWidget(d->previewConvertToEightBit, 3, 0, 1, 2);
    grid3->addWidget(d->previewScaleFitToWindow,  4, 0, 1, 2);
    grid3->addWidget(d->previewSmoothScaled,      5, 0, 1, 2);
    grid3->addWidget(magnifierLabel,              6, 0, 1, 1);
    grid3->addWidget(d->magnifierZoomLevel,       6, 1, 1, 1);
    grid3->addWidget(d->previewShowIcons,         7, 0, 1, 2);
    grid3->addWidget(d->previewAutoPlay,          8, 0, 1, 2);
    grid3->addWidget(d->previewOverlay,           9, 0, 1, 2);
    grid3->addWidget(d->osdWidget,               10, 0, 1, 2);
    grid3->setRowStretch(11, 10);

    d->previewFastPreview->setChecked(true);
    d->previewRawMode->setCurrentIndex(0);
    d->previewRawMode->setEnabled(false);

    d->tab->insertTab(Preview, pwpanel, i18nc("@title:tab", "Preview"));

    // --------------------------------------------------------

    QWidget* const fspanel    = new QWidget(d->tab);
    QVBoxLayout* const layout = new QVBoxLayout(fspanel);
    d->fullScreenSettings     = new FullScreenSettings(FS_ALBUMGUI, d->tab);
    d->fullScreenSettings->setTitle(QString());
    d->fullScreenSettings->setFlat(true);

    layout->setContentsMargins(QMargins());
    layout->setSpacing(spacing);
    layout->addWidget(d->fullScreenSettings);
    layout->addStretch();

    d->tab->insertTab(FullScreen, fspanel, i18nc("@title:tab", "Full-Screen"));

    // --------------------------------------------------------

    d->mimetype = new SetupMime();
    d->tab->insertTab(MimeType, d->mimetype, i18nc("@title:tab", "Mime Types"));

    d->category = new SetupCategory(d->tab);
    d->tab->insertTab(Category, d->category, i18nc("@title:tab", "Categories"));

    // --------------------------------------------------------

    readSettings();
    adjustSize();

    // --------------------------------------------------------

    connect(d->previewFullView, SIGNAL(toggled(bool)),
            d->previewRawMode, SLOT(setEnabled(bool)));

    connect(d->previewOverlay, SIGNAL(toggled(bool)),
            d->osdWidget, SLOT(setEnabled(bool)));

    connect(d->largeThumbsBox, SIGNAL(toggled(bool)),
            this, SLOT(slotUseLargeThumbsToggled(bool)));

    // --------------------------------------------------------
}

SetupAlbumView::~SetupAlbumView()
{
    delete d;
}

void SetupAlbumView::setActiveTab(AlbumTab tab)
{
    d->tab->setCurrentIndex(tab);
}

SetupAlbumView::AlbumTab SetupAlbumView::activeTab() const
{
    return (AlbumTab)d->tab->currentIndex();
}

void SetupAlbumView::applySettings()
{
    ApplicationSettings* const settings = ApplicationSettings::instance();

    if (!settings)
    {
        return;
    }

    settings->setTreeViewIconSize(d->iconTreeThumbSize->currentData().toInt());
    settings->setTreeViewFaceSize(d->iconTreeFaceSize->currentData().toInt());
    settings->setTreeViewFont(d->treeViewFontSelect->font());
    settings->setIconShowName(d->iconShowNameBox->isChecked());
    settings->setIconShowTags(d->iconShowTagsBox->isChecked());
    settings->setIconShowSize(d->iconShowSizeBox->isChecked());
    settings->setIconShowDate(d->iconShowDateBox->isChecked());
    settings->setIconShowModDate(d->iconShowModDateBox->isChecked());
    settings->setIconShowResolution(d->iconShowResolutionBox->isChecked());
    settings->setIconShowAspectRatio(d->iconShowAspectRatioBox->isChecked());
    settings->setIconShowTitle(d->iconShowTitleBox->isChecked());
    settings->setIconShowComments(d->iconShowCommentsBox->isChecked());
    settings->setIconShowOverlays(d->iconShowOverlaysBox->isChecked());
    settings->setIconShowFullscreen(d->iconShowFullscreenBox->isChecked());
    settings->setIconShowCoordinates(d->iconShowCoordinatesBox->isChecked());
    settings->setIconShowRating(d->iconShowRatingBox->isChecked());
    settings->setIconShowPickLabel(d->iconShowPickLabelBox->isChecked());
    settings->setIconShowColorLabel(d->iconShowColorLabelBox->isChecked());
    settings->setIconShowImageFormat(d->iconShowFormatBox->isChecked());
    settings->setIconViewFont(d->iconViewFontSelect->font());
    settings->setItemLeftClickAction(d->leftClickActionComboBox->currentIndex());

    PreviewSettings previewSettings;
    previewSettings.quality           = d->previewFastPreview->isChecked() ? PreviewSettings::FastPreview : PreviewSettings::HighQualityPreview;
    previewSettings.rawLoading        = (PreviewSettings::RawLoading)d->previewRawMode->itemData(d->previewRawMode->currentIndex()).toInt();
    previewSettings.convertToEightBit = d->previewConvertToEightBit->isChecked();
    settings->setPreviewSettings(previewSettings);

    settings->setPreviewAutoPlay(d->previewAutoPlay->isChecked());
    settings->setPreviewOverlay(d->previewOverlay->isChecked());
    settings->setPreviewShowIcons(d->previewShowIcons->isChecked());
    settings->setPreviewSmoothScaled(d->previewSmoothScaled->isChecked());
    settings->setScaleFitToWindow(d->previewScaleFitToWindow->isChecked());
    settings->setMagnifierZoomLevel(d->magnifierZoomLevel->currentData().toReal());
    settings->setShowFolderTreeViewItemsCount(d->showFolderTreeViewItemsCount->isChecked());
    settings->saveSettings();

    d->osdWidget->writeSettings();
    d->osdSettings.writeToConfig(QLatin1String("Preview OSD Settings"));

    KConfigGroup group = KSharedConfig::openConfig()->group(settings->generalConfigGroupName());
    d->fullScreenSettings->saveSettings(group);

    d->category->applySettings();
    d->mimetype->applySettings();

    // Method ThumbnailSize::setUseLargeThumbs() is not called here to prevent
    // dysfunction between Thumbs DB and icon if
    // thumb size is over 256 and when large thumbs size support is disabled.
    // digiKam need to be restarted to take effect.

    ThumbnailSize::saveSettings(group, d->largeThumbsBox->isChecked());
}

void SetupAlbumView::readSettings()
{
    ApplicationSettings* const settings = ApplicationSettings::instance();

    if (!settings)
    {
        return;
    }

    d->iconTreeThumbSize->setCurrentIndex(d->iconTreeThumbSize->findData(settings->getTreeViewIconSize()));
    d->iconTreeFaceSize->setCurrentIndex(d->iconTreeFaceSize->findData(settings->getTreeViewFaceSize()));

    d->treeViewFontSelect->setFont(settings->getTreeViewFont());
    d->iconShowNameBox->setChecked(settings->getIconShowName());
    d->iconShowTagsBox->setChecked(settings->getIconShowTags());
    d->iconShowSizeBox->setChecked(settings->getIconShowSize());
    d->iconShowDateBox->setChecked(settings->getIconShowDate());
    d->iconShowModDateBox->setChecked(settings->getIconShowModDate());
    d->iconShowResolutionBox->setChecked(settings->getIconShowResolution());
    d->iconShowAspectRatioBox->setChecked(settings->getIconShowAspectRatio());
    d->iconShowTitleBox->setChecked(settings->getIconShowTitle());
    d->iconShowCommentsBox->setChecked(settings->getIconShowComments());
    d->iconShowOverlaysBox->setChecked(settings->getIconShowOverlays());
    d->iconShowFullscreenBox->setChecked(settings->getIconShowFullscreen());
    d->iconShowCoordinatesBox->setChecked(settings->getIconShowCoordinates());
    d->iconShowRatingBox->setChecked(settings->getIconShowRating());
    d->iconShowPickLabelBox->setChecked(settings->getIconShowPickLabel());
    d->iconShowColorLabelBox->setChecked(settings->getIconShowColorLabel());
    d->iconShowFormatBox->setChecked(settings->getIconShowImageFormat());
    d->iconViewFontSelect->setFont(settings->getIconViewFont());

    d->leftClickActionComboBox->setCurrentIndex(settings->getItemLeftClickAction());

    PreviewSettings previewSettings = settings->getPreviewSettings();

    if (previewSettings.quality == PreviewSettings::FastPreview)
    {
        d->previewFastPreview->setChecked(true);
    }
    else
    {
        d->previewFullView->setChecked(true);
        d->previewRawMode->setEnabled(true);
    }

    d->previewRawMode->setCurrentIndex(d->previewRawMode->findData(previewSettings.rawLoading));

    d->previewAutoPlay->setChecked(settings->getPreviewAutoPlay());
    d->previewOverlay->setChecked(settings->getPreviewOverlay());
    d->previewShowIcons->setChecked(settings->getPreviewShowIcons());
    d->previewSmoothScaled->setChecked(settings->getPreviewSmoothScaled());
    d->previewScaleFitToWindow->setChecked(settings->getScaleFitToWindow());
    d->magnifierZoomLevel->setCurrentIndex(d->magnifierZoomLevel->findData(settings->getMagnifierZoomLevel()));
    d->previewConvertToEightBit->setChecked(previewSettings.convertToEightBit);
    d->showFolderTreeViewItemsCount->setChecked(settings->getShowFolderTreeViewItemsCount());

    d->osdSettings.readFromConfig(QLatin1String("Preview OSD Settings"));
    d->osdWidget->readSettings();

    KConfigGroup group = KSharedConfig::openConfig()->group(settings->generalConfigGroupName());
    d->fullScreenSettings->readSettings(group);

    ThumbnailSize::readSettings(group);
    d->useLargeThumbsOriginal = ThumbnailSize::getUseLargeThumbs();
    d->largeThumbsBox->setChecked(d->useLargeThumbsOriginal);

    d->category->readSettings();
    d->mimetype->readSettings();
}

bool SetupAlbumView::useLargeThumbsHasChanged() const
{
    return (d->largeThumbsBox->isChecked() != d->useLargeThumbsOriginal);
}

void SetupAlbumView::slotUseLargeThumbsToggled(bool b)
{
    // Show info if large thumbs were enabled, and only once.

    if (b && d->useLargeThumbsShowedInfo && useLargeThumbsHasChanged())
    {
        d->useLargeThumbsShowedInfo = true;
        QMessageBox::information(this, qApp->applicationName(),
                                 i18nc("@info",
                                       "This option changes the size in which thumbnails are generated. "
                                       "You need to restart digiKam for this option to take effect. "
                                       "Furthermore, you need to regenerate all already stored thumbnails via "
                                       "the <interface>Tools / Maintenance</interface> menu."));
    }
}

} // namespace Digikam

#include "moc_setupalbumview.cpp"
