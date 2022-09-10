/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Integrated, multithread face detection / recognition
 *
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_TRAINER_WORKERS_H
#define DIGIKAM_TRAINER_WORKERS_H

// Local includes

#include "facepipeline_p.h"
#include "faceitemretriever.h"

namespace Digikam
{

class Q_DECL_HIDDEN TrainerWorker : public WorkerObject
{
    Q_OBJECT

public:

    explicit TrainerWorker(FacePipeline::Private* const dd);
    ~TrainerWorker() override;

protected:

    void aboutToDeactivate() override;

public Q_SLOTS:

    void process(FacePipelineExtendedPackage::Ptr package);

Q_SIGNALS:

    void processed(FacePipelineExtendedPackage::Ptr package);

protected:

    FacialRecognitionWrapper     recognizer;
    FaceItemRetriever            imageRetriever;
    FacePipeline::Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_TRAINER_WORKERS_H
