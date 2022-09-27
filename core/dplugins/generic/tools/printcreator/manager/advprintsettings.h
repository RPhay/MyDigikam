/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-11-07
 * Description : a tool to print images
 *
 * SPDX-FileCopyrightText: 2007-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ADV_PRINT_SETTINGS_H
#define DIGIKAM_ADV_PRINT_SETTINGS_H

// Qt includes

#include <QtGlobal>
#include <QList>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QMap>
#include <QColor>
#include <QPrinter>
#include <QFont>

// Local includes

#include "filesaveconflictbox.h"

using namespace Digikam;

class KConfigGroup;

namespace DigikamGenericPrintCreatorPlugin
{

class AdvPrintPhoto;
class AdvPrintPhotoSize;

class AdvPrintSettings
{

public:

    /// Images selection mode
    enum Selection
    {
        IMAGES = 0,
        ALBUMS
    };

    /// Print output destination, outside real printers configured
    enum Output
    {
        PDF = 0,
        FILES,
        GIMP
    };

    /// Image format for Output::FILES
    enum ImageFormat
    {
        JPEG = 0,
        PNG,
        TIFF
    };

    /// Caption type to print over the images
    enum CaptionType
    {
        NONE = 0,
        FILENAME,
        DATETIME,
        COMMENT,
        CUSTOM
    };

public:

    explicit AdvPrintSettings();
    ~AdvPrintSettings();

    /// Read and write settings in config file between sessions.
    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

    QString format()                                 const;
    QString outputName(Output out)                   const;
    QRect*  getLayout(int photoIndex, int sizeIndex) const;

    /// Helper methods to fill combobox from GUI.
    static QMap<Output,      QString> outputNames();
    static QMap<ImageFormat, QString> imageFormatNames();
    static QMap<CaptionType, QString> captionTypeNames();

public:

    Selection                         selMode;       ///< Items selection mode

    QList<QUrl>                       inputImages;

    QString                           printerName;

    QSizeF                            pageSize;      ///< Page Size in mm

    QList<AdvPrintPhoto*>             photos;
    QList<AdvPrintPhotoSize*>         photosizes;

    /// Caption management.
    CaptionType                       captionType;
    QColor                            captionColor;
    QFont                             captionFont;
    int                               captionSize;
    QString                           captionTxt;    ///< String use to customize caption with CUSTOM mode.

    int                               currentPreviewPage;

    /// Crop management
    int                               currentCropPhoto;
    bool                              disableCrop;

    /// For Print to Gimp only
    QString                           tempPath;
    QStringList                       gimpFiles;
    QString                           gimpPath;
    QString                           savedPhotoSize;

    /// For print to image files only.
    ImageFormat                       imageFormat;
    FileSaveConflictBox::ConflictRule conflictRule;  ///< Rule if output image files already exists.
    QUrl                              outputDir;     ///< Directory where to store output images.
    bool                              openInFileBrowser;

    /// Generic data used by printing thread.
    AdvPrintPhotoSize*                outputLayouts;
    QPrinter*                         outputPrinter;
    QString                           outputPath;
};

} // namespace DigikamGenericPrintCreatorPlugin

#endif // DIGIKAM_ADV_PRINT_SETTINGS_H
