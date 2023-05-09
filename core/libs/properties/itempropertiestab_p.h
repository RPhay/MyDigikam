/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-04-19
 * Description : A tab to display general item information
 *
 * SPDX-FileCopyrightText: 2006-2023 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ITEM_PROPERTIES_TAB_P_H
#define DIGIKAM_ITEM_PROPERTIES_TAB_P_H

#include "itempropertiestab.h"

// Qt includes

#include <QGridLayout>
#include <QStyle>
#include <QDir>
#include <QFile>
#include <QPixmap>
#include <QPainter>
#include <QPair>
#include <QVariant>
#include <QApplication>
#include <QCollator>
#include <QIcon>
#include <QLocale>
#include <QTime>
#include <QtMath>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "itempropertiestxtlabel.h"
#include "picklabelwidget.h"
#include "colorlabelwidget.h"
#include "ditemtooltip.h"

namespace Digikam
{

class Q_DECL_HIDDEN ItemPropertiesTab::Private
{
public:

    explicit Private();
    ~Private();

    DTextLabelName*    caption;
    DTextLabelName*    title;
    DTextLabelName*    pickLabel;
    DTextLabelName*    colorLabel;
    DTextLabelName*    rating;
    DTextLabelName*    tags;
    DTextLabelName*    peoples;
    DTextLabelName*    versionned;
    DTextLabelName*    grouped;

    DTextLabelValue*   labelFile;
    DTextLabelValue*   labelFolder;
    DTextLabelValue*   labelSymlink;
    DTextLabelValue*   labelFileModifiedDate;
    DTextLabelValue*   labelFileSize;
    DTextLabelValue*   labelFileOwner;
    DTextLabelValue*   labelFilePermissions;

    DTextLabelValue*   labelImageMime;
    DTextLabelValue*   labelImageDimensions;
    DTextLabelValue*   labelImageRatio;
    DTextLabelValue*   labelImageBitDepth;
    DTextLabelValue*   labelImageColorMode;
    DTextLabelValue*   labelHasSidecar;
    DTextLabelValue*   labelHasGPSInfo;
    DTextLabelValue*   labelVersionnedInfo;
    DTextLabelValue*   labelGroupedInfo;

    DTextLabelValue*   labelPhotoMake;
    DTextLabelValue*   labelPhotoModel;
    DTextLabelValue*   labelPhotoDateTime;
    DTextLabelValue*   labelPhotoLens;
    DTextLabelValue*   labelPhotoAperture;
    DTextLabelValue*   labelPhotoFocalLength;
    DTextLabelValue*   labelPhotoExposureTime;
    DTextLabelValue*   labelPhotoSensitivity;
    DTextLabelValue*   labelPhotoExposureMode;
    DTextLabelValue*   labelPhotoFlash;
    DTextLabelValue*   labelPhotoWhiteBalance;

    // NOTE: special case for the caption and title. See bug #460134
    QLabel*            labelCaption;
    QLabel*            labelTitle;

    QLabel*            labelTags;
    QLabel*            labelPeoples;

    QLabel*            labelLocation;
    QLabel*            labelRights;

    DTextLabelValue*   labelPickLabel;
    DTextLabelValue*   labelColorLabel;
    DTextLabelValue*   labelRating;

    DTextLabelValue*   labelVideoAspectRatio;
    DTextLabelValue*   labelVideoDuration;
    DTextLabelValue*   labelVideoFrameRate;
    DTextLabelValue*   labelVideoVideoCodec;
    DTextLabelValue*   labelVideoAudioBitRate;
    DTextLabelValue*   labelVideoAudioChannelType;
    DTextLabelValue*   labelVideoAudioCodec;

    DToolTipStyleSheet cnt;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_PROPERTIES_TAB_P_H
