/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-08-09
 * Description : Enhance image with local contrasts (as human eye does).
 *               LDR ToneMapper zynaddsubfx.sourceforge.net/other/tonemapping
 *
 * Copyright (C) 2009      by Julien Pontabry <julien dot pontabry at gmail dot com>
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
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

#ifndef DIGIKAM_LOCAL_CONTRAST_FILTER_H
#define DIGIKAM_LOCAL_CONTRAST_FILTER_H

// Qt includes

#include <QImage>

// Local includes

#include "digikam_export.h"
#include "dimgthreadedfilter.h"
#include "digikam_globals.h"
#include "localcontrastcontainer.h"

namespace Digikam
{

class DIGIKAM_EXPORT LocalContrastFilter : public DImgThreadedFilter
{
    Q_OBJECT

private:

    struct Q_DECL_HIDDEN Args
    {
        explicit Args()
            : start          (0),
              stop           (0),
              a              (0.0),
              data           (nullptr),
              sizex          (0),
              sizey          (0),
              blur           (0.0),
              denormal_remove(0.0)
        {
        }

        uint   start;
        uint   stop;
        float  a;
        float* data;
        int    sizex;
        int    sizey;
        float  blur;
        float  denormal_remove;
    };

public:

    explicit LocalContrastFilter(QObject* const parent = nullptr);
    explicit LocalContrastFilter(DImg* const image, QObject* const parent = nullptr,
                                 const LocalContrastContainer& par = LocalContrastContainer());
    ~LocalContrastFilter()                                                    override;

    static QString          FilterIdentifier()
    {
        return QLatin1String("digikam:LocalContrastFilter");
    }

    static QString          DisplayableName();

    static QList<int>       SupportedVersions()
    {
        return QList<int>() << 2;
    }

    static int              CurrentVersion()
    {
        return 2;
    }

    QString         filterIdentifier()                                  const override
    {
        return FilterIdentifier();
    }

    FilterAction    filterAction()                                            override;

    void                    readParameters(const FilterAction& action)        override;

private:

    void filterImage()                                                        override;

    void  processRgbImage(float* const img, int sizex, int sizey);
    void  process8bitRgbImage(unsigned char* const img, int sizex, int sizey);
    void  process16bitRgbImage(unsigned short* const img, int sizex, int sizey);

    float func(float x1, float x2);

    void  inplaceBlur(float* const data, int sizex, int sizey, float blur);
    void  stretchContrast(float* const data, int datasize);

    inline void rgb2hsv(const float& r, const float& g, const float& b, float& h, float& s, float& v);
    inline void hsv2rgb(const float& h, const float& s, const float& v, float& r, float& g, float& b);

    void blurMultithreaded(uint start, uint stop, float* const img, float* const blurimage);
    void saturationMultithreaded(uint start, uint stop, float* const img, float* const srcimg);

    void inplaceBlurYMultithreaded(const Args& prm);
    void inplaceBlurXMultithreaded(const Args& prm);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_LOCAL_CONTRAST_FILTER_H
