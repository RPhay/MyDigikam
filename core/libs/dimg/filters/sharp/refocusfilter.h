/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-25
 * Description : Refocus threaded image filter.
 *
 * SPDX-FileCopyrightText: 2005-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009      by Matthias Welwarsky <matze at welwarsky dot de>
 * SPDX-FileCopyrightText: 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_REFOCUS_FILTER_H
#define DIGIKAM_REFOCUS_FILTER_H

// Local includes

#include "digikam_export.h"
#include "dimgthreadedfilter.h"
#include "digikam_globals.h"

namespace Digikam
{

class DIGIKAM_EXPORT RefocusFilter : public DImgThreadedFilter
{
    Q_OBJECT

private:

    struct Q_DECL_HIDDEN Args
    {
        explicit Args()
          : orgData(nullptr),
            destData(nullptr),
            width(0),
            height(0),
            sixteenBit(false),
            matrix(nullptr),
            mat_size(0)
        {
        }

        uchar*  orgData;
        uchar*  destData;
        int     width;
        int     height;
        bool    sixteenBit;
        double* matrix;
        uint    mat_size;
    };

public:

    explicit RefocusFilter(QObject* const parent = nullptr);
    explicit RefocusFilter(DImg* const orgImage, QObject* const parent = nullptr, int matrixSize=5, double radius=0.9,
                           double gauss=0.0, double correlation=0.5, double noise=0.01);

    ~RefocusFilter() override;

    static int maxMatrixSize();

    static QString          FilterIdentifier()
    {
        return QLatin1String("digikam:RefocusFilter");
    }

    static QString          DisplayableName();

    static QList<int>       SupportedVersions()
    {
        return QList<int>() << 1;
    }

    static int              CurrentVersion()
    {
        return 1;
    }

    QString         filterIdentifier()                          const override
    {
        return FilterIdentifier();
    }

    FilterAction    filterAction()                                    override;
    void                    readParameters(const FilterAction& action)        override;


private:

    void filterImage()                                                        override;

    void refocusImage(uchar* const data, int width, int height, bool sixteenBit,
                      int matrixSize, double radius, double gauss,
                      double correlation, double noise);

    void convolveImage(const Args& prm);

    void convolveImageMultithreaded(uint start, uint stop, uint y1, const Args& prm);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_REFOCUS_FILTER_H
