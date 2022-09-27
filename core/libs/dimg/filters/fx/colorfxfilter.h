/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-25
 * Description : Color FX threaded image filter.
 *
 * SPDX-FileCopyrightText: 2005-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
 * SPDX-FileCopyrightText: 2015      by Andrej Krutak <dev at andree dot sk>
 *
 * Original Blur algorithms copyrighted 2004 by
 * Pieter Z. Voloshyn <pieter dot voloshyn at gmail dot com>.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_COLOR_FX_FILTER_H
#define DIGIKAM_COLOR_FX_FILTER_H

// Local includes

#include "digikam_export.h"
#include "dimgthreadedfilter.h"
#include "digikam_globals.h"

namespace Digikam
{

class DIGIKAM_EXPORT ColorFXContainer
{

public:

    explicit ColorFXContainer()
      : colorFXType (0),            ///< ColorFXFilter::Solarize
        level       (0),
        iterations  (2),
        intensity   (100),
        path        (QString())
    {
    };

    ~ColorFXContainer()
    {
    };

public:

    int     colorFXType;
    int     level;
    int     iterations;
    int     intensity;
    QString path;
};

// ----------------------------------------------------------------------------------------------

class DIGIKAM_EXPORT ColorFXFilter : public DImgThreadedFilter
{
    Q_OBJECT

public:

    enum ColorFXFilterTypes
    {
        Solarize = 0,
        Vivid,
        Neon,
        FindEdges,
        Lut3D
    };

public:

    explicit ColorFXFilter(QObject* const parent = nullptr);
    explicit ColorFXFilter(DImg* const orgImage,
                           QObject* const parent,
                           const ColorFXContainer& settings = ColorFXContainer());
    ~ColorFXFilter()                                          override;

    static QString FilterIdentifier()
    {
        return QLatin1String("digikam:ColorFXFilter");
    }

    static QString DisplayableName();

    static QList<int> SupportedVersions()
    {
        return QList<int>() << 1;
    }

    static int CurrentVersion()
    {
        return 1;
    }

    QString filterIdentifier()                          const override
    {
        return FilterIdentifier();
    }

    FilterAction filterAction()                               override;

    void readParameters(const FilterAction& action)           override;

private:

    void filterImage()                                        override;

    void solarize(DImg* const orgImage, DImg* const destImage, int factor);
    void vivid(DImg* const orgImage, DImg* const destImage, int factor);
    void neon(DImg* const orgImage, DImg* const destImage, int Intensity, int BW);
    void findEdges(DImg* const orgImage, DImg* const destImage, int Intensity, int BW);
    void neonFindEdges(DImg* const orgImage, DImg* const destImage, bool neon, int Intensity, int BW);
    void loadLut3D(const QString& path);
    void applyLut3D();

private:

    ColorFXContainer m_settings;
    quint16*         m_lutTable;     ///< RGBA, A is unused
    int              m_lutTableSize; ///< all axis are of this size
};

} // namespace Digikam

#endif // DIGIKAM_COLOR_FX_FILTER_H
