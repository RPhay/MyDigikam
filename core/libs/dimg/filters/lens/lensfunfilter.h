/* ============================================================
 *
 * Date        : 2008-02-10
 * Description : a tool to fix automatically camera lens aberrations
 *
 * SPDX-FileCopyrightText: 2008      by Adrian Schroeter <adrian at suse dot de>
 * SPDX-FileCopyrightText: 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_LENS_FUN_FILTER_H
#define DIGIKAM_LENS_FUN_FILTER_H

// Local includes

#include "digikam_config.h"
#include "dimgthreadedfilter.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT LensFunContainer
{

public:

    explicit LensFunContainer()
      : filterCCA       (true),
        filterVIG       (true),
        filterDST       (true),
        filterGEO       (true),
        cropFactor      (-1.0),
        focalLength     (-1.0),
        aperture        (-1.0),
        subjectDistance (-1.0),
        cameraMake      (QString()),
        cameraModel     (QString()),
        lensModel       (QString())
    {
    };

    ~LensFunContainer()
    {
    };

public:

    bool      filterCCA;       ///< Chromatic Aberration Corrections
    bool      filterVIG;       ///< Vignetting Corrections
    bool      filterDST;       ///< Distortion Corrections
    bool      filterGEO;       ///< Geometry Corrections

    double    cropFactor;
    double    focalLength;
    double    aperture;
    double    subjectDistance;

    QString   cameraMake;
    QString   cameraModel;
    QString   lensModel;
};

// -----------------------------------------------------------------

class DIGIKAM_EXPORT LensFunFilter : public DImgThreadedFilter
{
    Q_OBJECT

public:

    explicit LensFunFilter(QObject* const parent = nullptr);
    explicit LensFunFilter(DImg* const origImage,
                           QObject* const parent,
                           const LensFunContainer& settings);
    ~LensFunFilter()                                       override;

    bool registerSettingsToXmp(MetaEngineData& data) const;
    void readParameters(const FilterAction& action)        override;

    static QString FilterIdentifier()
    {
        return QLatin1String("digikam:LensFunFilter");
    }

    static QString DisplayableName();

    static QList<int> SupportedVersions()
    {
        return QList<int>() << 1 << 2;
    }

    static int CurrentVersion()
    {
        return 2;
    }

    QString filterIdentifier()                       const override
    {
        return FilterIdentifier();
    }

    FilterAction filterAction()                            override;

private:

    void filterImage()                                     override;
    void filterCCAMultithreaded(uint start, uint stop);
    void filterVIGMultithreaded(uint start, uint stop);
    void filterDSTMultithreaded(uint start, uint stop);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_LENS_FUN_FILTER_H
