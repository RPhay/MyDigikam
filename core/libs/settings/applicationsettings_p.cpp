/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-16-10
 * Description : application settings interface
 *
 * SPDX-FileCopyrightText: 2003-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2015      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * SPDX-FileCopyrightText: 2017      by Simon Frei <freisim93 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "applicationsettings_p.h"

namespace Digikam
{

ApplicationSettings::OperationStrings ApplicationSettings::Private::createConfigGroupingOperateOnAll()
{
    ApplicationSettings::OperationStrings out;
    out.insert(ApplicationSettings::Metadata,     QLatin1String("Do metadata operations on all"));
    out.insert(ApplicationSettings::ImportExport, QLatin1String("Do Import Export operations on all"));
    out.insert(ApplicationSettings::BQM,          QLatin1String("Do BQM operations on all"));
    out.insert(ApplicationSettings::LightTable,   QLatin1String("Do light table operations on all"));
    out.insert(ApplicationSettings::Slideshow,    QLatin1String("Do slideshow operations on all"));
    out.insert(ApplicationSettings::Rename,       QLatin1String("Rename all"));
    out.insert(ApplicationSettings::Tools,        QLatin1String("Operate tools on all"));

    return out;
}

ApplicationSettings::Private::Private(ApplicationSettings* const qq)
    : q(qq)
{
    for (int i = 0 ; i != ApplicationSettings::Unspecified ; ++i)
    {
        groupingOperateOnAll.insert((ApplicationSettings::OperationType)i,
                ApplicationSettings::Ask);
    }
}

void ApplicationSettings::Private::init()
{
    albumCategoryNames.clear();
    albumCategoryNames.append(i18nc("@item: album categories", "Category"));
    albumCategoryNames.append(i18nc("@item: album categories", "Travel"));
    albumCategoryNames.append(i18nc("@item: album categories", "Holidays"));
    albumCategoryNames.append(i18nc("@item: album categories", "Friends"));
    albumCategoryNames.append(i18nc("@item: album categories", "Nature"));
    albumCategoryNames.append(i18nc("@item: album categories", "Party"));
    albumCategoryNames.append(i18nc("@item: album categories", "Todo"));
    albumCategoryNames.append(i18nc("@item: album categories", "Miscellaneous"));
    albumCategoryNames.sort();

    toolTipsFont                                      = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
    showToolTips                                      = false;
    tooltipShowFileName                               = true;
    tooltipShowFileDate                               = false;
    tooltipShowFileSize                               = false;
    tooltipShowImageType                              = false;
    tooltipShowImageDim                               = true;
    tooltipShowImageAR                                = true;
    tooltipShowPhotoMake                              = true;
    tooltipShowPhotoLens                              = true;
    tooltipShowPhotoDate                              = true;
    tooltipShowPhotoFocal                             = true;
    tooltipShowPhotoExpo                              = true;
    tooltipShowPhotoMode                              = true;
    tooltipShowPhotoFlash                             = false;
    tooltipShowPhotoWb                                = false;
    tooltipShowAlbumName                              = false;
    tooltipShowTitles                                 = false;
    tooltipShowComments                               = true;
    tooltipShowTags                                   = true;
    tooltipShowLabelRating                            = true;

    tooltipShowVideoAspectRatio                       = true;
    tooltipShowVideoAudioBitRate                      = true;
    tooltipShowVideoAudioChannelType                  = true;
    tooltipShowVideoAudioCodec                        = true;
    tooltipShowVideoDuration                          = true;
    tooltipShowVideoFrameRate                         = true;
    tooltipShowVideoVideoCodec                        = true;

    showAlbumToolTips                                 = false;
    tooltipShowAlbumTitle                             = true;
    tooltipShowAlbumDate                              = true;
    tooltipShowAlbumCollection                        = true;
    tooltipShowAlbumCategory                          = true;
    tooltipShowAlbumCaption                           = true;
    tooltipShowAlbumPreview                           = false;

    scaleFitToWindow                                  = false;
    previewShowIcons                                  = true;
    showThumbbar                                      = true;

    recursiveAlbums                                   = false;
    recursiveTags                                     = true;
    allGroupsOpen                                     = false;

    showFolderTreeViewItemsCount                      = false;

    syncToDigikam                                     = false;
    syncToBaloo                                       = false;

    faceDetectionAccuracy                             = 0.7;
    faceDetectionYoloV3                               = false;

    minimumSimilarityBound                            = 40;
    duplicatesSearchLastMinSimilarity                 = 90;
    duplicatesSearchLastMaxSimilarity                 = 100;
    duplicatesSearchLastAlbumTagRelation              = 0;
    duplicatesSearchLastRestrictions                  = 0;
    duplicatesSearchLastReferenceImageSelectionMethod = HaarIface::RefImageSelMethod::OlderOrLarger;

    scanAtStart                                       = true;
    cleanAtStart                                      = true;
    databaseDirSetAtCmd                               = false;
    albumMonitoring                                   = false;
    stringComparisonType                              = ApplicationSettings::Natural;

    applicationStyle                                  = qApp->style()->objectName();
    applicationIcon                                   = QString();
    applicationFont                                   = QFontDatabase::systemFont(QFontDatabase::GeneralFont);

    for (int i = 0 ; i != ApplicationSettings::Unspecified ; ++i)
    {
        groupingOperateOnAll.insert((ApplicationSettings::OperationType)i,
                                    ApplicationSettings::Ask);
    }

    q->connect(q, SIGNAL(balooSettingsChanged()),
               q, SLOT(applyBalooSettings()));
}

} // namespace Digikam
