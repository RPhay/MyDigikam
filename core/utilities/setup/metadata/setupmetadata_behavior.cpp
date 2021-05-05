/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-08-03
 * Description : setup Metadata tab.
 *
 * Copyright (C) 2003-2004 by Ralf Holzer <ralf at well dot com>
 * Copyright (C) 2003-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2017      by Simon Frei <freisim93 at gmail dot com>
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

#include "setupmetadata_p.h"

namespace Digikam
{

void SetupMetadata::appendBehaviorTab()
{
    QWidget* const panel            = new QWidget;
    QVBoxLayout* const mainLayout   = new QVBoxLayout;

    d->fieldsGroup                  = new QGroupBox;
    QGridLayout* const fieldsLayout = new QGridLayout;

    d->fieldsGroup->setWhatsThis(xi18nc("@info:whatsthis",
                                        "<para>In addition to the pixel content, image files usually "
                                        "contain a variety of metadata. A lot of the parameters you can use "
                                        "in digiKam to manage files, such as rating or comment, can be written "
                                        "to the files' metadata.</para> "
                                        "<para>Storing in metadata allows one to preserve this information "
                                        "when moving or sending the files to different systems.</para>"));

    QLabel* const fieldsIconLabel = new QLabel;
    fieldsIconLabel->setPixmap(QIcon::fromTheme(QLatin1String("format-list-unordered")).pixmap(32));

    QLabel* const fieldsLabel     = new QLabel(i18nc("@label", "Write This Information to the Metadata"));

    d->saveTagsBox       = new QCheckBox;
    d->saveTagsBox->setText(i18nc("@option:check", "Image tags"));
    d->saveTagsBox->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to store the item tags "
                                       "in the XMP and IPTC tags."));

    d->saveCommentsBox   = new QCheckBox;
    d->saveCommentsBox->setText(i18nc("@option:check", "Captions and title"));
    d->saveCommentsBox->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to store item caption and title "
                                           "in the JFIF Comment section, the Exif tag, the XMP tag, "
                                           "and the IPTC tag."));

    d->saveRatingBox     = new QCheckBox;
    d->saveRatingBox->setText(i18nc("@option:check", "Rating"));
    d->saveRatingBox->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to store the item rating "
                                         "in the Exif tag and the XMP tags."));

    d->savePickLabelBox  = new QCheckBox;
    d->savePickLabelBox->setText(i18nc("@option:check", "Pick label"));
    d->savePickLabelBox->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to store the item pick label "
                                            "in the XMP tags."));

    d->saveColorLabelBox = new QCheckBox;
    d->saveColorLabelBox->setText(i18nc("@option:check", "Color label"));
    d->saveColorLabelBox->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to store the item color label "
                                             "in the XMP tags."));

    d->saveDateTimeBox   = new QCheckBox;
    d->saveDateTimeBox->setText(i18nc("@option:check", "Timestamps"));
    d->saveDateTimeBox->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to store the item date and time "
                                           "in the EXIF, XMP, and IPTC tags."));

    d->saveTemplateBox   = new QCheckBox;
    d->saveTemplateBox->setText(i18nc("@option:check", "Metadata templates (Copyright etc.)"));
    d->saveTemplateBox->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to store the metadata "
                                           "template in the XMP and the IPTC tags. "
                                           "You can set template values to Template setup page."));
    d->saveFaceTags      = new QCheckBox;
    d->saveFaceTags->setText(i18nc("@option:check", "Face Tags (including face areas)"));
    d->saveFaceTags->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to store face tags "
                                        "with face rectangles in the XMP tags."));

    d->savePosition      = new QCheckBox;
    d->savePosition->setText(i18nc("@option:check", "Geolocation information (GPS)"));
    d->savePosition->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to store Geolocation information "
                                        "in the Exif tag and the XMP tags."));

    fieldsLayout->addWidget(fieldsIconLabel,       0, 0, 2, 3);
    fieldsLayout->addWidget(fieldsLabel,           0, 1, 2, 3);
    fieldsLayout->addWidget(d->saveTagsBox,        2, 0, 1, 3);
    fieldsLayout->addWidget(d->saveCommentsBox,    3, 0, 1, 3);
    fieldsLayout->addWidget(d->saveRatingBox,      4, 0, 1, 3);
    fieldsLayout->addWidget(d->savePickLabelBox,   5, 0, 1, 3);
    fieldsLayout->addWidget(d->saveColorLabelBox,  6, 0, 1, 3);
    fieldsLayout->addWidget(d->saveDateTimeBox,    7, 0, 1, 3);
    fieldsLayout->addWidget(d->saveTemplateBox,    8, 0, 1, 3);
    fieldsLayout->addWidget(d->saveFaceTags,       9, 0, 1, 3);
    fieldsLayout->addWidget(d->savePosition,      10, 0, 1, 3);
    fieldsLayout->setColumnStretch(3, 10);
    d->fieldsGroup->setLayout(fieldsLayout);

    // --------------------------------------------------------

    d->readWriteGroup                  = new QGroupBox;
    QGridLayout* const readWriteLayout = new QGridLayout;

    QLabel* const readWriteIconLabel   = new QLabel;
    readWriteIconLabel->setPixmap(QIcon::fromTheme(QLatin1String("document-open")).pixmap(32));

    QLabel* const readWriteLabel       = new QLabel(i18nc("@label", "Reading and Writing Metadata"));

    d->useLazySync        = new QCheckBox;
    d->useLazySync->setText(i18nc("@option:check", "Use lazy synchronization"));
    d->useLazySync->setWhatsThis(i18nc("@info:whatsthis",
                                       "Instead of synchronizing metadata, just schedule it for synchronization."
                                       "Synchronization can be done later by triggering the apply pending, or at digikam exit"));

    d->writeDngFilesBox   = new QCheckBox;
    d->writeDngFilesBox->setText(i18nc("@option:check", "Write metadata to DNG files"));
    d->writeDngFilesBox->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to write metadata into DNG files."));
    d->writeDngFilesBox->setEnabled(MetaEngine::supportMetadataWriting(QLatin1String("image/x-raw")));

    d->writeRawFilesBox   = new QCheckBox;
    d->writeRawFilesBox->setText(i18nc("@option:check", "If possible write metadata to RAW files (Dangerous)"));
    d->writeRawFilesBox->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to write metadata into RAW TIFF/EP files. "
                                            "This feature requires the Exiv2 shared library, version >= 0.18.0. It is still "
                                            "experimental, and is disabled by default. Activate this option only if you know what you are doing."));
    d->writeRawFilesBox->setEnabled(MetaEngine::supportMetadataWriting(QLatin1String("image/x-raw")));

    d->updateFileTimeStampBox = new QCheckBox;
    d->updateFileTimeStampBox->setText(i18nc("@option:check", "&Update file modification timestamp when files are modified"));
    d->updateFileTimeStampBox->setWhatsThis(i18nc("@info:whatsthis",
                                                  "Turn off this option to not update file timestamps when files are changed as "
                                                  "when you update metadata or image data. Note: disabling this option can "
                                                  "introduce some dysfunctions with applications which use file timestamps "
                                                  "properties to detect file modifications automatically."));

    d->rescanImageIfModifiedBox = new QCheckBox;
    d->rescanImageIfModifiedBox->setText(i18nc("@option:check", "&Rescan file when files are modified"));
    d->rescanImageIfModifiedBox->setWhatsThis(i18nc("@info:whatsthis",
                                                    "Turning this option on, will force digiKam to rescan files that has been "
                                                    "modified outside digiKam. If a file has changed it is file size or if "
                                                    "the last modified timestamp has changed, a rescan of that "
                                                    "file will be performed when digiKam starts."));

    d->clearMetadataIfRescanBox    = new QCheckBox;
    d->clearMetadataIfRescanBox->setText(i18nc("@option:check", "&Clean up the metadata from the database when rescan files"));
    d->clearMetadataIfRescanBox->setWhatsThis(i18nc("@info:whatsthis",
                                                    "Turning this option on, will force digiKam to delete the file metadata "
                                                    "contained in the database before the file is rescanned. WARNING: "
                                                    "if your metadata has been written to the database only and not "
                                                    "to the file or sidecar, you will be able to lose inserted "
                                                    "metadata such as tags, keywords, or geographic coordinates."));

    readWriteLayout->addWidget(readWriteIconLabel,          0, 0, 2, 3);
    readWriteLayout->addWidget(readWriteLabel,              0, 1, 2, 3);
    readWriteLayout->addWidget(d->useLazySync,              2, 0, 1, 3);
    readWriteLayout->addWidget(d->writeDngFilesBox,         3, 0, 1, 3);
    readWriteLayout->addWidget(d->writeRawFilesBox,         4, 0, 1, 3);
    readWriteLayout->addWidget(d->updateFileTimeStampBox,   5, 0, 1, 3);
    readWriteLayout->addWidget(d->rescanImageIfModifiedBox, 6, 0, 1, 3);
    readWriteLayout->addWidget(d->clearMetadataIfRescanBox, 7, 0, 1, 3);
    readWriteLayout->setColumnStretch(3, 10);
    d->readWriteGroup->setLayout(readWriteLayout);

    // --------------------------------------------------------

    QFrame* const infoBox           = new QFrame;
    QGridLayout* const infoBoxGrid  = new QGridLayout;
    infoBox->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    DActiveLabel* const exiv2LogoLabel = new DActiveLabel(QUrl(QLatin1String("https://www.exiv2.org")),
                                                          QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("digikam/data/logo-exiv2.png")),
                                                          infoBox);
    exiv2LogoLabel->setWhatsThis(i18nc("@info:whatsthis", "Visit Exiv2 project website"));

    QLabel* const explanation = new QLabel(infoBox);
    explanation->setOpenExternalLinks(true);
    explanation->setWordWrap(true);
    QString txt;

    txt.append(QString::fromUtf8("<p><a href='https://en.wikipedia.org/wiki/Exif'>Exif</a> - %1</p>")
               .arg(i18nc("@info", "a standard used by most digital cameras today to store technical "
                          "information (like aperture and shutter speed) about an image.")));

    txt.append(QString::fromUtf8("<p><a href='https://en.wikipedia.org/wiki/IPTC_Information_Interchange_Model'>IPTC</a> - %1</p>")
               .arg(i18nc("@info", "an older standard used in digital photography to store "
                          "photographer information in images.")));

    if (MetaEngine::supportXmp())
    {
        txt.append(QString::fromUtf8("<p><a href='https://en.wikipedia.org/wiki/Extensible_Metadata_Platform'>XMP</a> - %1</p>")
                   .arg(i18nc("@info", "a new standard used in digital photography, designed to replace IPTC.")));
    }

    explanation->setText(txt);

    infoBoxGrid->addWidget(exiv2LogoLabel, 0, 0, 1, 1);
    infoBoxGrid->addWidget(explanation,    0, 1, 1, 2);
    infoBoxGrid->setColumnStretch(1, 10);
    infoBoxGrid->setRowStretch(1, 10);
    infoBoxGrid->setSpacing(0);
    infoBox->setLayout(infoBoxGrid);

    // --------------------------------------------------------

    mainLayout->addWidget(d->fieldsGroup);
    mainLayout->addWidget(d->readWriteGroup);
    mainLayout->addWidget(infoBox);
    panel->setLayout(mainLayout);

    d->tab->insertTab(Behavior, panel, i18nc("@title:tab", "Behavior"));
}

} // namespace Digikam
