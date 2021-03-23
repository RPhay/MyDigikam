/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-11-01
 * Description : A digital camera RAW files loader for DImg
 *               framework using an external dcraw instance.
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2005-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DIGIKAM_DIMG_RAW_LOADER_H
#define DIGIKAM_DIMG_RAW_LOADER_H

// Local includes

#include "dimg.h"
#include "drawdecoder.h"
#include "dimgloader.h"
#include "drawdecoding.h"
#include "rawprocessingfilter.h"

using namespace Digikam;

namespace DigikamRAWDImgPlugin
{

class DImgRAWLoader : public DRawDecoder,
                      public DImgLoader
{
    Q_OBJECT

public:

    explicit DImgRAWLoader(DImg* const image, const DRawDecoding& rawDecodingSettings = DRawDecoding());
    ~DImgRAWLoader()                                                                 override;

    bool load(const QString& filePath, DImgLoaderObserver* const observer = nullptr) override;
    bool save(const QString&, DImgLoaderObserver* const)                             override;

    bool hasAlpha()   const                                                          override;
    bool isReadOnly() const                                                          override;
    bool sixteenBit() const                                                          override;

    void postProcess(DImgLoaderObserver* const observer);

    FilterAction filterAction() const;

private:

    bool loadedFromRawData(const QByteArray& data,
                           int width,
                           int height,
                           int rgbmax,
                           DImgLoaderObserver* const observer);

    bool checkToCancelWaitingData()                                                 override;
    void setWaitingDataProgress(double value)                                       override;

private:

    // Disable
    explicit DImgRAWLoader(QObject*)               = delete;
    DImgRAWLoader(const DImgRAWLoader&)            = delete;
    DImgRAWLoader& operator=(const DImgRAWLoader&) = delete;

private:

    DImgLoaderObserver*  m_observer;
    RawProcessingFilter* m_filter;
};

} // namespace DigikamRAWDImgPlugin

#endif // DIGIKAM_DIMG_RAW_LOADER_H
