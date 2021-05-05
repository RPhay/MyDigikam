/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Integrated, multithread face detection / recognition
 *
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
