/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-03-27
 * Description : a tool to export items to a local storage
 *
 * Copyright (C) 2006-2009 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2011-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019      by Maik Qualmann <metzpinguin at gmail dot com>
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
#include "fcsettings.h"

// Qt includes

#include <QApplication>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QSpinBox>
#include <QComboBox>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "dfileselector.h"
#include "ditemslist.h"
#include "wstoolutils.h"
#include "dlayoutbox.h"
#include "fctask.h"

namespace DigikamGenericFileCopyPlugin
{

class Q_DECL_HIDDEN FCExportWidget::Private
{
public:

    explicit Private()
      : selector(nullptr),
        imageList(nullptr),
        overwrite(nullptr),
        targetButtonGroup(nullptr),
        fileCopyButton(nullptr),
        symLinkButton(nullptr),
        relativeButton(nullptr),
        imageChangeGroupBox(nullptr),
        changeImagesProp(nullptr),
        removeMetadataProp(nullptr),
        imageCompression(nullptr),
        imagesResize(nullptr),
        imagesFormat(nullptr)
    {
    }

    DFileSelector* selector;
    DItemsList*    imageList;
    QCheckBox*     overwrite;

    QButtonGroup*  targetButtonGroup;
    QRadioButton*  fileCopyButton;
    QRadioButton*  symLinkButton;
    QRadioButton*  relativeButton;

    QUrl           targetUrl;

    QGroupBox*     imageChangeGroupBox;
    QCheckBox*     changeImagesProp;
    QCheckBox*     removeMetadataProp;

    QSpinBox*      imageCompression;
    QSpinBox*      imagesResize;
    QComboBox*     imagesFormat;
};

FCExportWidget::FCExportWidget(DInfoInterface* const iface, QWidget* const parent)
    : QWidget(parent),
      d(new Private)
{
    // setup local target selection

    DHBox* const hbox           = new DHBox(this);
    QLabel* const locationLabel = new QLabel(hbox);
    locationLabel->setText(i18n("Target location: "));
    d->selector                 = new DFileSelector(hbox);
    d->selector->setFileDlgMode(QFileDialog::Directory);
    d->selector->setFileDlgTitle(i18n("Target Folder"));
    d->selector->setWhatsThis(i18n("Sets the target address to copy the items to."));

    QLabel* const targetLabel   = new QLabel(i18n("Target File behavior:"), this);
    d->targetButtonGroup        = new QButtonGroup(this);
    d->fileCopyButton           = new QRadioButton(i18n("Copy files"), this);
    d->symLinkButton            = new QRadioButton(i18n("Create symlinks"), this);
    d->relativeButton           = new QRadioButton(i18n("Create relative symlinks"), this);

    d->overwrite                = new QCheckBox(i18n("Overwrite existing items in the target"), this);

    d->targetButtonGroup->addButton(d->fileCopyButton, FCTask::CopyFile);
    d->targetButtonGroup->addButton(d->symLinkButton,  FCTask::FullSymLink);
    d->targetButtonGroup->addButton(d->relativeButton, FCTask::RelativeSymLink);
    d->targetButtonGroup->setExclusive(true);
    d->fileCopyButton->setChecked(true);

    //---------------------------------------------

    d->changeImagesProp  = new QCheckBox(i18n("Adjust image properties"), this);
    d->changeImagesProp->setChecked(false);
    d->changeImagesProp->setWhatsThis(i18n("If you enable this option, "
                                           "all images to be sent can be "
                                           "resized and recompressed."));

    //---------------------------------------------

    d->imageChangeGroupBox = new QGroupBox(i18n("Image Properties"), this);

    d->imagesResize = new QSpinBox(d->imageChangeGroupBox);
    d->imagesResize->setRange(300, 4000);
    d->imagesResize->setSingleStep(1);
    d->imagesResize->setValue(1024);
    d->imagesResize->setSuffix(i18n(" px"));
    d->imagesResize->setWhatsThis(i18n("Select the length of the images that are to be sent. "
                                       "The aspect ratio is preserved."));
    d->imageChangeGroupBox->setEnabled(false);

    QLabel* const  labelImagesResize = new QLabel(i18n("Image Length:"), d->imageChangeGroupBox);
    labelImagesResize->setBuddy(d->imagesResize);

    //---------------------------------------------

    QLabel* const labelImagesFormat = new QLabel(d->imageChangeGroupBox);
    labelImagesFormat->setWordWrap(false);
    labelImagesFormat->setText(i18n("Image Format:"));

    d->imagesFormat  = new QComboBox(d->imageChangeGroupBox);
    d->imagesFormat->setEditable(false);
    d->imagesFormat->setWhatsThis(i18n("Select your preferred format to convert image."));

    QMap<ImageFormat, QString> frms;
    frms[ImageFormat::JPEG] = i18nc("Image format: JPEG", "Jpeg");
    frms[ImageFormat::PNG]  = i18nc("Image format: PNG",  "Png");
    QMap<ImageFormat, QString>::const_iterator it2 = frms.constBegin();

    while (it2 != frms.constEnd())
    {
        d->imagesFormat->addItem(it2.value(), (int)it2.key());
        ++it2;
    }

    labelImagesFormat->setBuddy(d->imagesFormat);

    // --------------------

    d->imageCompression = new QSpinBox(d->imageChangeGroupBox);
    d->imageCompression->setRange(1, 100);
    d->imageCompression->setSingleStep(1);
    d->imageCompression->setValue(75);
    QString whatsThis = i18n("<p>The new compression value of JPEG images to be sent:</p>");
    whatsThis         = whatsThis + i18n("<p><b>1</b>: very high compression<br/>"
                                         "<b>25</b>: high compression<br/>"
                                         "<b>50</b>: medium compression<br/>"
                                         "<b>75</b>: low compression (default value)<br/>"
                                         "<b>100</b>: no compression</p>");

    d->imageCompression->setWhatsThis(whatsThis);

    QLabel* const labelImageCompression = new QLabel(i18n("Image quality:"), this);
    labelImageCompression->setBuddy(d->imageCompression);

    // --------------------

    d->removeMetadataProp = new QCheckBox(i18n("Remove all metadata"), this);
    d->removeMetadataProp->setWhatsThis(i18n("If you enable this option, all metadata "
                                             "as Exif, Iptc, and Xmp will be removed."));

    // --------------------

    QGridLayout* const grid2  = new QGridLayout(d->imageChangeGroupBox);
    grid2->addWidget(labelImagesResize,     0, 0, 1, 1);
    grid2->addWidget(d->imagesResize,       0, 1, 1, 2);
    grid2->addWidget(labelImagesFormat,     1, 0, 1, 1);
    grid2->addWidget(d->imagesFormat,       1, 1, 1, 2);
    grid2->addWidget(labelImageCompression, 2, 0, 1, 1);
    grid2->addWidget(d->imageCompression,   2, 1, 1, 2);
    grid2->addWidget(d->removeMetadataProp, 3, 0, 1, 2);
    grid2->setRowStretch(4, 10);
    grid2->setColumnStretch(2, 10);
    grid2->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    grid2->setAlignment(Qt::AlignTop);

    //---------------------------------------------

    // setup image list
    d->imageList                = new DItemsList(this);
    d->imageList->setObjectName(QLatin1String("FCExport ImagesList"));
    d->imageList->setIface(iface);
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
    layout->addWidget(d->imageList);

    layout->addWidget(d->changeImagesProp);
    layout->addWidget(d->imageChangeGroupBox);

    layout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
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

QUrl FCExportWidget::targetUrl() const
{
    return d->targetUrl;
}

void FCExportWidget::setTargetUrl(const QUrl& url)
{
    d->targetUrl = url;
    d->selector->setFileDlgPath(d->targetUrl.toLocalFile());
}

ChangeImagePropertiesPtr FCExportWidget::getImageChangeProperties() const
{
    ChangeImagePropertiesPtr props; // nullptr

    if (d->fileCopyButton->isChecked() && Qt::Checked == d->changeImagesProp->checkState())
    {
        props = std::make_shared<ChangeImageProperties>();
        props->imageSize = d->imagesResize->value();
        props->imageFormat = (ImageFormat)d->imagesFormat->currentIndex();
        props->imageCompression = d->imageCompression->value();
        props->removeMetadata = Qt::Checked == d->removeMetadataProp->checkState();
    }

    return props;
}

DItemsList* FCExportWidget::imagesList() const
{
    return d->imageList;
}

QCheckBox* FCExportWidget::overwriteBox() const
{
    return d->overwrite;
}

QButtonGroup* FCExportWidget::targetButtonGroup() const
{
    return d->targetButtonGroup;
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
        d->changeImagesProp->setCheckState(Qt::Unchecked);
    }

    d->changeImagesProp->setEnabled(enabled);

    d->imageChangeGroupBox->setEnabled(false); // The changeImagesProp is by default and on each change unchecked
}

} // namespace DigikamGenericFileCopyPlugin
