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

#ifndef DIGIKAM_DATABASE_WRITER_H
#define DIGIKAM_DATABASE_WRITER_H

// Local includes

#include "facepipeline_p.h"
#include "faceitemretriever.h"

namespace Digikam
{

class Q_DECL_HIDDEN DatabaseWriter : public WorkerObject
{
    Q_OBJECT

public:

    DatabaseWriter(FacePipeline::WriteMode mode, FacePipeline::Private* const dd);
    ~DatabaseWriter() override;

public Q_SLOTS:

    void process(FacePipelineExtendedPackage::Ptr package);

Q_SIGNALS:

    void processed(FacePipelineExtendedPackage::Ptr package);

protected:

    FacePipeline::WriteMode      mode;
    ThumbnailLoadThread*         thumbnailLoadThread;
    FacePipeline::Private* const d;

private:

    // Disable
    DatabaseWriter(const DatabaseWriter&)            = delete;
    DatabaseWriter& operator=(const DatabaseWriter&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_DATABASE_WRITER_H
