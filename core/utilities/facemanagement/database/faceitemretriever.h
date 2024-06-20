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

class Q_DECL_HIDDEN FaceItemRetriever
{
public:

    explicit FaceItemRetriever(FacePipeline::Private* const d);
    ~FaceItemRetriever();

    void cancel();

    QList<QImage*> getDetails(const DImg& src, const QList<QRectF>& rects)                   const;
    QList<QImage*> getDetails(const DImg& src, const QList<FaceTagsIface>& faces)            const;
    QList<QImage*> getThumbnails(const QString& filePath, const QList<FaceTagsIface>& faces) const;

protected:

    ThumbnailImageCatcher* catcher = nullptr;

private:

    // Disable
    FaceItemRetriever(const FaceItemRetriever&)            = delete;
    FaceItemRetriever& operator=(const FaceItemRetriever&) = delete;
};

} // namespace Digikam
