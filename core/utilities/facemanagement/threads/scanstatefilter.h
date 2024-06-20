/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Integrated, multithread face detection / recognition
 *
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "facepipeline_p.h"

namespace Digikam
{

class Q_DECL_HIDDEN ScanStateFilter : public DynamicThread
{
    Q_OBJECT

public:

    ScanStateFilter(FacePipeline::FilterMode fmode, FacePipeline::Private* const dd);
    ~ScanStateFilter() override = default;

    void process(const QList<ItemInfo>& infos);
    void process(const ItemInfo& info);

    FacePipelineExtendedPackage::Ptr filter(const ItemInfo& info);

public:

    FacePipeline::Private* const     d     = nullptr;
    FacePipeline::FilterMode         mode  = FacePipeline::SkipAlreadyScanned;
    FacePipelineFaceTagsIface::Roles tasks;

protected Q_SLOTS:

    void dispatch();

Q_SIGNALS:

    void signalInfosToDispatch();

protected:

    void run() override;

protected:

    QList<ItemInfo>                         toFilter;
    QList<FacePipelineExtendedPackage::Ptr> toSend;
    QList<ItemInfo>                         toBeSkipped;

private:

    // Disable
    ScanStateFilter(const ScanStateFilter&)            = delete;
    ScanStateFilter& operator=(const ScanStateFilter&) = delete;
};

} // namespace Digikam
