/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-02-13
 * Description : slide show settings container.
 *
 * Copyright (C) 2007-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019-2020 by Minh Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#include "slideshowsettings.h"

// Qt includes

#include <QFontDatabase>

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>

// Local includes

#include "metaenginesettings.h"

using namespace Digikam;

namespace DigikamGenericSlideShowPlugin
{

const QString SlideShowSettings::configGroupName(QLatin1String("ImageViewer Settings"));
const QString SlideShowSettings::configSlideShowStartCurrentEntry(QLatin1String("SlideShowStartCurrent"));
const QString SlideShowSettings::configSlideShowDelayEntry(QLatin1String("SlideShowDelay"));
const QString SlideShowSettings::configSlideShowLoopEntry(QLatin1String("SlideShowLoop"));
const QString SlideShowSettings::configSlideShowSuffleEntry(QLatin1String("SlideShowSuffle"));
const QString SlideShowSettings::configSlideShowPrintApertureFocalEntry(QLatin1String("SlideShowPrintApertureFocal"));
const QString SlideShowSettings::configSlideShowPrintCommentEntry(QLatin1String("SlideShowPrintComment"));
const QString SlideShowSettings::configSlideShowPrintTitleEntry(QLatin1String("SlideShowPrintTitle"));
const QString SlideShowSettings::configSlideShowPrintCapIfNoTitleEntry(QLatin1String("SlideShowPrintCapIfNoTitle"));
const QString SlideShowSettings::configSlideShowPrintDateEntry(QLatin1String("SlideShowPrintDate"));
const QString SlideShowSettings::configSlideShowPrintExpoSensitivityEntry(QLatin1String("SlideShowPrintExpoSensitivity"));
const QString SlideShowSettings::configSlideShowPrintMakeModelEntry(QLatin1String("SlideShowPrintMakeModel"));
const QString SlideShowSettings::configSlideShowPrintLensModelEntry(QLatin1String("SlideShowPrintLensModel"));
const QString SlideShowSettings::configSlideShowPrintNameEntry(QLatin1String("SlideShowPrintName"));
const QString SlideShowSettings::configSlideShowPrintTagsEntry(QLatin1String("SlideShowPrintTags"));
const QString SlideShowSettings::configSlideShowPrintLabelsEntry(QLatin1String("SlideShowPrintLabels"));
const QString SlideShowSettings::configSlideShowPrintRatingEntry(QLatin1String("SlideShowPrintRating"));
const QString SlideShowSettings::configSlideShowProgressIndicatorEntry(QLatin1String("SlideShowProgressIndicator"));
const QString SlideShowSettings::configSlideShowCaptionFontEntry(QLatin1String("SlideShowCaptionFont"));
const QString SlideShowSettings::configSlideScreenEntry(QLatin1String("SlideScreen"));

SlideShowSettings::SlideShowSettings()
    : startWithCurrent      (false),
      exifRotate            (true),
      printName             (true),
      printDate             (false),
      printApertureFocal    (false),
      printMakeModel        (false),
      printLensModel        (false),
      printExpoSensitivity  (false),
      printComment          (false),
      printTitle            (false),
      printCapIfNoTitle     (false),
      printTags             (false),
      printLabels           (false),
      printRating           (false),
      loop                  (false),
      suffle                (false),
      delay                 (5),
      autoPlayEnabled       (true),
      slideScreen           (-2),
      showProgressIndicator (true),
      captionFont           (QFontDatabase::systemFont(QFontDatabase::GeneralFont)),
      iface                 (nullptr),
      plugin                (nullptr)
{
}

SlideShowSettings::~SlideShowSettings()
{
}

void SlideShowSettings::readFromConfig()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(configGroupName);

    startWithCurrent          = group.readEntry(configSlideShowStartCurrentEntry,         false);
    delay                     = group.readEntry(configSlideShowDelayEntry,                5);
    loop                      = group.readEntry(configSlideShowLoopEntry,                 false);
    suffle                    = group.readEntry(configSlideShowSuffleEntry,               false);
    printName                 = group.readEntry(configSlideShowPrintNameEntry,            true);
    printDate                 = group.readEntry(configSlideShowPrintDateEntry,            false);
    printApertureFocal        = group.readEntry(configSlideShowPrintApertureFocalEntry,   false);
    printExpoSensitivity      = group.readEntry(configSlideShowPrintExpoSensitivityEntry, false);
    printMakeModel            = group.readEntry(configSlideShowPrintMakeModelEntry,       false);
    printLensModel            = group.readEntry(configSlideShowPrintLensModelEntry,       false);
    printComment              = group.readEntry(configSlideShowPrintCommentEntry,         false);
    printTitle                = group.readEntry(configSlideShowPrintTitleEntry,           false);
    printCapIfNoTitle         = group.readEntry(configSlideShowPrintCapIfNoTitleEntry,    false);
    printTags                 = group.readEntry(configSlideShowPrintTagsEntry,            false);
    printLabels               = group.readEntry(configSlideShowPrintLabelsEntry,          false);
    printRating               = group.readEntry(configSlideShowPrintRatingEntry,          false);
    showProgressIndicator     = group.readEntry(configSlideShowProgressIndicatorEntry,    true);
    captionFont               = group.readEntry(configSlideShowCaptionFontEntry,
                                                QFontDatabase::systemFont(QFontDatabase::GeneralFont));
    slideScreen               = group.readEntry(configSlideScreenEntry,                   -2);
    exifRotate                = MetaEngineSettings::instance()->settings().exifRotate;
}

void SlideShowSettings::writeToConfig()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(configGroupName);

    group.writeEntry(configSlideShowStartCurrentEntry,         startWithCurrent);
    group.writeEntry(configSlideShowDelayEntry,                delay);
    group.writeEntry(configSlideShowLoopEntry,                 loop);
    group.writeEntry(configSlideShowSuffleEntry,               suffle);
    group.writeEntry(configSlideShowPrintNameEntry,            printName);
    group.writeEntry(configSlideShowPrintDateEntry,            printDate);
    group.writeEntry(configSlideShowPrintApertureFocalEntry,   printApertureFocal);
    group.writeEntry(configSlideShowPrintExpoSensitivityEntry, printExpoSensitivity);
    group.writeEntry(configSlideShowPrintMakeModelEntry,       printMakeModel);
    group.writeEntry(configSlideShowPrintLensModelEntry,       printLensModel);
    group.writeEntry(configSlideShowPrintCommentEntry,         printComment);
    group.writeEntry(configSlideShowPrintTitleEntry,           printTitle);
    group.writeEntry(configSlideShowPrintCapIfNoTitleEntry,    printCapIfNoTitle);
    group.writeEntry(configSlideShowPrintTagsEntry,            printTags);
    group.writeEntry(configSlideShowPrintLabelsEntry,          printLabels);
    group.writeEntry(configSlideShowPrintRatingEntry,          printRating);
    group.writeEntry(configSlideShowProgressIndicatorEntry,    showProgressIndicator);
    group.writeEntry(configSlideShowCaptionFontEntry,          captionFont);
    group.writeEntry(configSlideScreenEntry,                   slideScreen);
    group.sync();
}

int SlideShowSettings::indexOf(const QUrl& url) const
{
    return fileList.indexOf(url);
}

int SlideShowSettings::count() const
{
    return fileList.count();
}

void SlideShowSettings::suffleImages()
{
    if (suffle && autoPlayEnabled)
    {
        if (originalFileList.isEmpty())
        {
            // keep a backup of original file list at the first suffle

            originalFileList         = fileList;

            // suffle

            qsrand(QTime::currentTime().msec());

            QList<QUrl>::iterator it = fileList.begin();
            QList<QUrl>::iterator it1;

            for (uint i = 0 ; i < (uint)count() ; ++i)
            {
                int inc = (int) (float(count()) * qrand() / (RAND_MAX + 1.0));

                it1     = fileList.begin();
                it1    += inc;

                std::swap(*(it++), *(it1));
             }
        }
    }
    else
    {
        if (!originalFileList.isEmpty())
        {
            // return to original list

            fileList = originalFileList;
            originalFileList.clear();
        }
    }
}

} // namespace DigikamGenericSlideShowPlugin
