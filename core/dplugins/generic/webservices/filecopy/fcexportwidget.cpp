/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-03-27
 * Description : a tool to export items to a local storage
 *
 * Copyright (C) 2006-2009 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019-2020 by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "fcexportwidget.h"

// Qt includes

#include <QApplication>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QGroupBox>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "dfileselector.h"
#include "wstoolutils.h"
#include "ditemslist.h"
#include "dlayoutbox.h"
#include "fctask.h"

namespace DigikamGenericFileCopyPlugin
{

class Q_DECL_HIDDEN FCExportWidget::Private
{
public:

    explicit Private()
      : iface               (nullptr),
        selector            (nullptr),
        imageList           (nullptr),
        overwrite           (nullptr),
        albumPath           (nullptr),
        targetButtonGroup   (nullptr),
        fileCopyButton      (nullptr),
        symLinkButton       (nullptr),
        relativeButton      (nullptr),
        imageChangeGroupBox (nullptr),
        changeImagesProp    (nullptr),
        removeMetadataProp  (nullptr),
        imageCompression    (nullptr),
        imageResize         (nullptr),
        imageFormat         (nullptr)
    {
    }

    DInfoInterface* iface;
    DFileSelector*  selector;
    DItemsList*     imageList;
    QCheckBox*      overwrite;
    QCheckBox*      albumPath;

    QButtonGroup*   targetButtonGroup;
    QRadioButton*   fileCopyButton;
    QRadioButton*   symLinkButton;
    QRadioButton*   relativeButton;

    QUrl            targetUrl;

    QGroupBox*      imageChangeGroupBox;
    QCheckBox*      changeImagesProp;
    QCheckBox*      removeMetadataProp;

    QSpinBox*       imageCompression;
    QSpinBox*       imageResize;
    QComboBox*      imageFormat;
};

FCExportWidget::FCExportWidget(DInfoInterface* const iface, QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    d->iface = iface;

    // setup local target selection

    const int spacing           = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    DHBox* const hbox           = new DHBox(this);
    QLabel* const locationLabel = new QLabel(hbox);
    locationLabel->setText(i18n("Target location: "));
    d->selector                 = new DFileSelector(hbox);
    d->selector->setFileDlgMode(QFileDialog::Directory);
    d->selector->setFileDlgOptions(QFileDialog::ShowDirsOnly);
    d->selector->setFileDlgTitle(i18n("Target Folder"));
    d->selector->setWhatsThis(i18n("Sets the target address to copy the items to."));

    QLabel* const targetLabel   = new QLabel(i18n("Target File behavior:"), this);
    d->targetButtonGroup        = new QButtonGroup(this);
    d->fileCopyButton           = new QRadioButton(i18n("Copy files"), this);
    d->symLinkButton            = new QRadioButton(i18n("Create symlinks"), this);
    d->relativeButton           = new QRadioButton(i18n("Create relative symlinks"), this);

    d->overwrite                = new QCheckBox(i18n("Overwrite existing items in the target"), this);
    d->albumPath                = new QCheckBox(i18n("Use the album path of the items in the target"), this);

    if (!d->iface->supportAlbums())
    {
        d->albumPath->hide();
    }

    d->targetButtonGroup->addButton(d->fileCopyButton, FCContainer::CopyFile);
    d->targetButtonGroup->addButton(d->symLinkButton,  FCContainer::FullSymLink);
    d->targetButtonGroup->addButton(d->relativeButton, FCContainer::RelativeSymLink);
    d->targetButtonGroup->setExclusive(true);
    d->fileCopyButton->setChecked(true);

    //---------------------------------------------

    d->changeImagesProp = new QCheckBox(i18n("Adjust image properties"), this);
    d->changeImagesProp->setChecked(false);
    d->changeImagesProp->setWhatsThis(i18n("If you enable this option, "
                                           "all images to be sent can be "
                                           "resized and recompressed."));

    //---------------------------------------------

    d->imageChangeGroupBox          = new QGroupBox(i18n("Image Properties"), this);

    d->imageResize                  = new QSpinBox(d->imageChangeGroupBox);
    d->imageResize->setRange(300, 6000);
    d->imageResize->setSingleStep(1);
    d->imageResize->setValue(1024);
    d->imageResize->setSuffix(i18n(" px"));
    d->imageResize->setWhatsThis(i18n("Select the length of the images that are to be sent. "
                                       "The aspect ratio is preserved."));
    d->imageChangeGroupBox->setEnabled(false);

    QLabel* const  labelImageResize = new QLabel(i18n("Image Length:"), d->imageChangeGroupBox);
    labelImageResize->setBuddy(d->imageResize);

    //---------------------------------------------

    QLabel* const labelImageFormat = new QLabel(d->imageChangeGroupBox);
    labelImageFormat->setWordWrap(false);
    labelImageFormat->setText(i18n("Image Format:"));

    d->imageFormat                 = new QComboBox(d->imageChangeGroupBox);
    d->imageFormat->setEditable(false);
    d->imageFormat->setWhatsThis(i18n("Select your preferred format to convert image."));
    d->imageFormat->addItem(i18nc("Image format: JPEG", "Jpeg"), FCContainer::JPEG);
    d->imageFormat->addItem(i18nc("Image format: PNG",  "Png"),  FCContainer::PNG);
    labelImageFormat->setBuddy(d->imageFormat);

    //---------------------------------------------

    d->imageCompression                 = new QSpinBox(d->imageChangeGroupBox);
    d->imageCompression->setRange(1, 100);
    d->imageCompression->setSingleStep(1);
    d->imageCompression->setValue(75);
    d->imageCompression->setWhatsThis(i18n("<p>The new compression value of JPEG images to be sent:</p>"
                                           "<p><b>1</b>: very high compression<br/>"
                                           "<b>25</b>: high compression<br/>"
                                           "<b>50</b>: medium compression<br/>"
                                           "<b>75</b>: low compression (default value)<br/>"
                                           "<b>100</b>: no compression</p>"));

    QLabel* const labelImageCompression = new QLabel(i18n("Image quality:"), d->imageChangeGroupBox);
    labelImageCompression->setBuddy(d->imageCompression);

    //---------------------------------------------

    d->removeMetadataProp = new QCheckBox(i18n("Remove all metadata"), d->imageChangeGroupBox);
    d->removeMetadataProp->setWhatsThis(i18n("If you enable this option, all metadata "
                                             "as Exif, Iptc, and Xmp will be removed."));

    //---------------------------------------------

    QGridLayout* const grid2 = new QGridLayout(d->imageChangeGroupBox);
    grid2->addWidget(labelImageResize,      0, 0, 1, 1);
    grid2->addWidget(d->imageResize,        0, 1, 1, 2);
    grid2->addWidget(labelImageFormat,      1, 0, 1, 1);
    grid2->addWidget(d->imageFormat,        1, 1, 1, 2);
    grid2->addWidget(labelImageCompression, 2, 0, 1, 1);
    grid2->addWidget(d->imageCompression,   2, 1, 1, 2);
    grid2->addWidget(d->removeMetadataProp, 3, 0, 1, 2);
    grid2->setColumnStretch(2, 10);
    grid2->setSpacing(spacing);
    grid2->setAlignment(Qt::AlignTop);

    //---------------------------------------------

    // setup image list
    d->imageList = new DItemsList(this);
    d->imageList->setObjectName(QLatin1String("FCExport ImagesList"));
    d->imageList->setIface(d->iface);
    d->imageList->loadImagesFromCurrentSelection();
    d->imageList->setAllowRAW(true);
    d->imageList->listView()->setWhatsThis(i18n("This is the list of items to copy "
                                                "to the specified target."));

    // layout dialog
    QVBoxLayout* const layout = new QVBoxLayout(this);

    layout->addWidget(hbox);
    layout->addWidget(targetLabel);
    layout->addWidget(d->fileCopyButton);
    layout->addWidget(d->symLinkButton);
    layout->addWidget(d->relativeButton);
    layout->addWidget(d->overwrite);
    layout->addWidget(d->albumPath);
    layout->addWidget(d->imageList);
    layout->addWidget(d->changeImagesProp);
    layout->addWidget(d->imageChangeGroupBox);
    layout->setSpacing(spacing);
    layout->setContentsMargins(QMargins());

    // ------------------------------------------------------------------------

    connect(d->selector->lineEdit(), SIGNAL(textEdited(QString)),
            this, SLOT(slotLabelUrlChanged()));

    connect(d->selector, SIGNAL(signalUrlSelected(QUrl)),
            this, SLOT(slotLabelUrlChanged()));

    connect(d->fileCopyButton, SIGNAL(toggled(bool)),
            this, SLOT(slotFileCopyButtonChanged(bool)));

    connect(d->changeImagesProp, SIGNAL(toggled(bool)),
            d->imageChangeGroupBox, SLOT(setEnabled(bool)));

}

FCExportWidget::~FCExportWidget()
{
    delete d;
}

DItemsList* FCExportWidget::imagesList() const
{
    return d->imageList;
}

QUrl FCExportWidget::targetUrl() const
{
    return d->targetUrl;
}

FCContainer FCExportWidget::getSettings() const
{
    FCContainer settings;

    settings.iface                 = d->iface;
    settings.destUrl               = d->targetUrl;
    settings.behavior              = d->targetButtonGroup->checkedId();
    settings.imageFormat           = d->imageFormat->currentIndex();
    settings.imageResize           = d->imageResize->value();
    settings.imageCompression      = d->imageCompression->value();
    settings.overwrite             = d->overwrite->isChecked();
    settings.albumPath             = d->albumPath->isChecked();
    settings.removeMetadata        = d->removeMetadataProp->isChecked();
    settings.changeImageProperties = d->changeImagesProp->isChecked();

    return settings;
}

void FCExportWidget::setSettings(const FCContainer& settings)
{
    d->targetUrl                  = settings.destUrl;
    d->selector->setFileDlgPath(d->targetUrl.toLocalFile());
    QAbstractButton* const button = d->targetButtonGroup->button(settings.behavior);

    if (button)
    {
        button->setChecked(true);
    }

    d->imageFormat->setCurrentIndex(settings.imageFormat);
    d->imageResize->setValue(settings.imageResize);
    d->imageCompression->setValue(settings.imageCompression);
    d->overwrite->setChecked(settings.overwrite);
    d->albumPath->setChecked(settings.albumPath);
    d->removeMetadataProp->setChecked(settings.removeMetadata);
    d->changeImagesProp->setChecked(settings.changeImageProperties);
}

void FCExportWidget::slotLabelUrlChanged()
{
    d->targetUrl = QUrl::fromLocalFile(d->selector->fileDlgPath());

    emit signalTargetUrlChanged(d->targetUrl);
}

void FCExportWidget::slotFileCopyButtonChanged(bool enabled)
{
    if (!enabled)
    {
        d->changeImagesProp->setChecked(false);
    }

    d->changeImagesProp->setEnabled(enabled);

     // The changeImagesProp is by default and on each change unchecked

    d->imageChangeGroupBox->setEnabled(false);
}

} // namespace DigikamGenericFileCopyPlugin
