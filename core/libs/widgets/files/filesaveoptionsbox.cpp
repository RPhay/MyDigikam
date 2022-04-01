/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-08-02
 * Description : a stack of widgets to set image file save
 *               options into image editor.
 *
 * Copyright (C) 2007-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "filesaveoptionsbox.h"

// Qt includes

#include <QCheckBox>
#include <QGridLayout>
#include <QImageReader>
#include <QLabel>
#include <QWidget>
#include <QApplication>
#include <QStyle>

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"
#include "dpluginloader.h"

namespace Digikam
{

class Q_DECL_HIDDEN FileSaveOptionsBox::Private
{

public:

    explicit Private()
      : noneOptions     (nullptr),
        noneGrid        (nullptr),
        labelNone       (nullptr),
        JPEGOptions     (nullptr),
        PNGOptions      (nullptr),
        TIFFOptions     (nullptr),

#ifdef HAVE_JASPER

        JPEG2000Options (nullptr),

#endif // HAVE_JASPER

#ifdef HAVE_X265

        HEIFOptions     (nullptr),

#endif // HAVE_X265

        PGFOptions      (nullptr)
    {
    }

    QWidget*            noneOptions;

    QGridLayout*        noneGrid;

    QLabel*             labelNone;

    DImgLoaderSettings* JPEGOptions;
    DImgLoaderSettings* PNGOptions;
    DImgLoaderSettings* TIFFOptions;

#ifdef HAVE_JASPER

    DImgLoaderSettings* JPEG2000Options;

#endif // HAVE_JASPER

#ifdef HAVE_X265

    DImgLoaderSettings* HEIFOptions;

#endif // HAVE_X265

    DImgLoaderSettings* PGFOptions;
};

FileSaveOptionsBox::FileSaveOptionsBox(QWidget* const parent)
    : QStackedWidget(parent),
      d             (new Private)
{
    setAttribute(Qt::WA_DeleteOnClose);

    //-- NONE Settings ------------------------------------------------------

    d->noneOptions               = new QWidget(this);
    d->noneGrid                  = new QGridLayout(d->noneOptions);
    d->noneGrid->setSpacing(qMin(QApplication::style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing),
                             QApplication::style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing)));
    d->noneOptions->setLayout(d->noneGrid);
    d->labelNone                 = new QLabel(i18n("No options available"), d->noneOptions);
    d->noneGrid->addWidget(d->labelNone, 0, 0, 0, 1);
    DPluginLoader* const ploader = DPluginLoader::instance();

    //-- JPEG Settings ------------------------------------------------------

    d->JPEGOptions     = ploader->exportWidget(QLatin1String("JPEG"));
    d->JPEGOptions->setParent(this);

    //-- PNG Settings -------------------------------------------------------

    d->PNGOptions      = ploader->exportWidget(QLatin1String("PNG"));
    d->PNGOptions->setParent(this);

    //-- TIFF Settings ------------------------------------------------------

    d->TIFFOptions     = ploader->exportWidget(QLatin1String("TIFF"));
    d->TIFFOptions->setParent(this);

    //-- JPEG 2000 Settings -------------------------------------------------

#ifdef HAVE_JASPER

    d->JPEG2000Options = ploader->exportWidget(QLatin1String("JP2"));
    d->JPEG2000Options->setParent(this);

#endif // HAVE_JASPER

    //-- PGF Settings -------------------------------------------------

    d->PGFOptions      = ploader->exportWidget(QLatin1String("PGF"));
    d->PGFOptions->setParent(this);

    //-- HEIF Settings -------------------------------------------------

#ifdef HAVE_X265

    d->HEIFOptions     = ploader->exportWidget(QLatin1String("HEIF"));
    d->HEIFOptions->setParent(this);

#endif // HAVE_X265

    //-----------------------------------------------------------------------

    insertWidget(DImg::NONE, d->noneOptions);
    insertWidget(DImg::JPEG, d->JPEGOptions);
    insertWidget(DImg::PNG,  d->PNGOptions);
    insertWidget(DImg::TIFF, d->TIFFOptions);

#ifdef HAVE_JASPER

    insertWidget(DImg::JP2K, d->JPEG2000Options);

#endif // HAVE_JASPER

    insertWidget(DImg::PGF,  d->PGFOptions);

#ifdef HAVE_X265

    insertWidget(DImg::HEIF, d->HEIFOptions);

#endif // HAVE_X265

    //-----------------------------------------------------------------------

    readSettings();
}

FileSaveOptionsBox::~FileSaveOptionsBox()
{
    delete d;
}

void FileSaveOptionsBox::setImageFileFormat(const QString& ext)
{
    qCDebug(DIGIKAM_WIDGETS_LOG) << "Format selected: " << ext;
    setCurrentIndex(discoverFormat(ext, DImg::NONE));
}

DImg::FORMAT FileSaveOptionsBox::discoverFormat(const QString& filename, DImg::FORMAT fallback)
{
    qCDebug(DIGIKAM_WIDGETS_LOG) << "Trying to discover format based on filename '" << filename
                                 << "', fallback = " << fallback;

    QStringList splitParts = filename.split(QLatin1Char('.'));
    QString ext;

    if (splitParts.size() < 2)
    {
        qCDebug(DIGIKAM_WIDGETS_LOG) << "filename '" << filename
                                     << "' does not contain an extension separated by a point.";
        ext = filename;
    }
    else
    {
        ext = splitParts.at(splitParts.size() - 1);
    }

    ext = ext.toUpper();

    DImg::FORMAT format = fallback;

    if      (ext.contains(QLatin1String("JPEG")) || ext.contains(QLatin1String("JPG")) || ext.contains(QLatin1String("JPE")))
    {
        format = DImg::JPEG;
    }
    else if (ext.contains(QLatin1String("PNG")))
    {
        format = DImg::PNG;
    }
    else if (ext.contains(QLatin1String("TIFF")) || ext.contains(QLatin1String("TIF")))
    {
        format = DImg::TIFF;
    }

#ifdef HAVE_JASPER

    else if (ext.contains(QLatin1String("JP2")) || ext.contains(QLatin1String("JPX")) || ext.contains(QLatin1String("JPC")) ||
             ext.contains(QLatin1String("PGX")) || ext.contains(QLatin1String("J2K")))
    {
        format = DImg::JP2K;
    }

#endif // HAVE_JASPER

#ifdef HAVE_X265

    else if (ext.contains(QLatin1String("HEIC")) || ext.contains(QLatin1String("HEIF")))
    {
        format = DImg::HEIF;
    }

#endif // HAVE_X265

    else if (ext.contains(QLatin1String("PGF")))
    {
        format = DImg::PGF;
    }
    else
    {
        qCWarning(DIGIKAM_WIDGETS_LOG) << "Using fallback format " << fallback;
    }

    qCDebug(DIGIKAM_WIDGETS_LOG) << "Discovered format: " << format;

    return format;
}

void FileSaveOptionsBox::applySettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group("ImageViewer Settings");
    group.writeEntry(QLatin1String("JPEGCompression"),     d->JPEGOptions->settings()[QLatin1String("quality")].toInt());
    group.writeEntry(QLatin1String("JPEGSubSampling"),     d->JPEGOptions->settings()[QLatin1String("subsampling")].toInt());

    // ---

    group.writeEntry(QLatin1String("PNGCompression"),      d->PNGOptions->settings()[QLatin1String("quality")].toInt());

    // ---

    group.writeEntry(QLatin1String("TIFFCompression"),     d->TIFFOptions->settings()[QLatin1String("compress")].toBool());

#ifdef HAVE_JASPER

    group.writeEntry(QLatin1String("JPEG2000Compression"), d->JPEG2000Options->settings()[QLatin1String("quality")].toInt());
    group.writeEntry(QLatin1String("JPEG2000LossLess"),    d->JPEG2000Options->settings()[QLatin1String("lossless")].toBool());

#endif // HAVE_JASPER

    group.writeEntry(QLatin1String("PGFCompression"),      d->PGFOptions->settings()[QLatin1String("quality")].toInt());
    group.writeEntry(QLatin1String("PGFLossLess"),         d->PGFOptions->settings()[QLatin1String("lossless")].toBool());

#ifdef HAVE_X265

    group.writeEntry(QLatin1String("HEIFCompression"),     d->HEIFOptions->settings()[QLatin1String("quality")].toInt());
    group.writeEntry(QLatin1String("HEIFLossLess"),        d->HEIFOptions->settings()[QLatin1String("lossless")].toBool());

#endif // HAVE_X265

    config->sync();
}

void FileSaveOptionsBox::readSettings()
{
    DImgLoaderPrms set;
    
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group("ImageViewer Settings");

    set.clear();
    set.insert(QLatin1String("quality"),     group.readEntry(QLatin1String("JPEGCompression"),     75));
    set.insert(QLatin1String("subsampling"), group.readEntry(QLatin1String("JPEGSubSampling"),     1));  ///< Medium subsampling
    d->JPEGOptions->setSettings(set);

    // ---
    
    set.clear();
    set.insert(QLatin1String("quality"),  group.readEntry(QLatin1String("PNGCompression"),         9));
    d->PNGOptions->setSettings(set);

    // ---
    
    set.clear();
    set.insert(QLatin1String("compress"), group.readEntry(QLatin1String("TIFFCompression"),        false));
    d->TIFFOptions->setSettings(set);

#ifdef HAVE_JASPER

    set.clear();
    set.insert(QLatin1String("quality"),  group.readEntry(QLatin1String("JPEG2000Compression"),    75));
    set.insert(QLatin1String("lossless"), group.readEntry(QLatin1String("JPEG2000LossLess"),       true));
    d->JPEG2000Options->setSettings(set);

#endif // HAVE_JASPER

    set.clear();
    set.insert(QLatin1String("quality"),  group.readEntry(QLatin1String("PGFCompression"),         3));
    set.insert(QLatin1String("lossless"), group.readEntry(QLatin1String("PGFLossLess"),            true));
    d->PGFOptions->setSettings(set);  

#ifdef HAVE_X265

    set.clear();
    set.insert(QLatin1String("quality"),  group.readEntry(QLatin1String("HEIFCompression"),        75));
    set.insert(QLatin1String("lossless"), group.readEntry(QLatin1String("HEIFLossLess"),           true));
    d->HEIFOptions->setSettings(set);

#endif // HAVE_X265

}

} // namespace Digikam
