/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010-2011 by Jens Mueller <tschenser at gmx dot de>
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

/**
 * References about DNG:
 * DNG SDK tutorial:    www.adobeforums.com/webx/.3bc2944e
 *                      www.adobeforums.com/webx/.3c054bde
 * DNG review:          www.barrypearson.co.uk/articles/dng/index.htm
 * DNG intro:           www.adobe.com/digitalimag/pdfs/dng_primer.pdf
 *                      www.adobe.com/products/dng/pdfs/DNG_primer_manufacturers.pdf
 * DNG Specification:   www.adobe.com/products/dng/pdfs/dng_spec_1_2_0_0.pdf
 * TIFF/EP Spec.:       www.map.tu.chiba-u.ac.jp/IEC/100/TA2/recdoc/N4378.pdf
 * DNG SDK reference:   www.thomasdideriksen.dk/misc/File%20Formats/dng_sdk_refman.pdf
 * DNG SDK tarball:     ftp.adobe.com/pub/adobe/dng/dng_sdk_1_2.zip
 * DNG users forum:     www.adobeforums.com/webx/.3bb5f0ec
 *
 * Applications using DNG SDK:
 * DNG4PS2:             dng4ps2.chat.ru/index_en.html
 * CORNERFIX:           sourceforge.net/projects/cornerfix
 * ADOBE DNG CONVERTER: ftp.adobe.com/pub/adobe/photoshop/cameraraw/win/4.x
 * DNGCONVERT:          github.com/jmue/dngconvert
 * MOVIE2DNG:           elphel.svn.sourceforge.net/svnroot/elphel/tools/Movie2DNG
 */

#include "dngwriter_p.h"

namespace Digikam
{

DNGWriter::DNGWriter()
    : d(new Private)
{
}

DNGWriter::~DNGWriter()
{
    delete d;
}

void DNGWriter::cancel()
{
    d->cancel = true;
}

void DNGWriter::reset()
{
    d->reset();
}

void DNGWriter::setCompressLossLess(bool b)
{
    d->jpegLossLessCompression = b;
}

bool DNGWriter::compressLossLess() const
{
    return d->jpegLossLessCompression;
}

void DNGWriter::setUpdateFileDate(bool b)
{
    d->updateFileDate = b;
}

bool DNGWriter::updateFileDate() const
{
    return d->updateFileDate;
}

void DNGWriter::setBackupOriginalRawFile(bool b)
{
    d->backupOriginalRawFile = b;
}

bool DNGWriter::backupOriginalRawFile() const
{
    return d->backupOriginalRawFile;
}

void DNGWriter::setPreviewMode(int mode)
{
    d->previewMode = mode;
}

int DNGWriter::previewMode() const
{
    return d->previewMode;
}

void DNGWriter::setInputFile(const QString& filePath)
{
    d->inputFile = filePath;
}

void DNGWriter::setOutputFile(const QString& filePath)
{
    d->outputFile = filePath;
}

QString DNGWriter::inputFile() const
{
    return d->inputFile;
}

QString DNGWriter::outputFile() const
{
    return d->outputFile;
}

} // namespace Digikam
