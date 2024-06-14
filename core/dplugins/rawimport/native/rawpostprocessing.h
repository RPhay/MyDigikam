/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-13-08
 * Description : Raw post processing corrections.
 *
 * SPDX-FileCopyrightText: 2008-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "dimgthreadedfilter.h"

using namespace Digikam;

namespace DigikamRawImportNativePlugin
{

class RawPostProcessing : public DImgThreadedFilter
{

public:

    explicit RawPostProcessing(DImg* const orgImage,
                               QObject* const parent = nullptr,
                               const DRawDecoding& settings = DRawDecoding());

    /**
     * Constructor for slave mode: execute immediately in current thread with specified master filter
     */
    RawPostProcessing(DImgThreadedFilter* const parentFilter,
                      const DImg& orgImage,
                      const DImg& destImage,
                      int progressBegin = 0,
                      int progressEnd = 100,
                      const DRawDecoding& settings = DRawDecoding());

    ~RawPostProcessing();

private:

    void rawPostProcessing();

    virtual void filterImage()                          override;

    /**
     * This filter is only for preview calculation.
     */
    virtual FilterAction filterAction()                 override
    {
        return FilterAction();
    }

    virtual void readParameters(const FilterAction&)    override
    {
    }

    virtual QString filterIdentifier() const            override
    {
        return QString();
    }

private:

    DRawDecoding m_customRawSettings;
};

} // namespace DigikamRawImportNativePlugin
