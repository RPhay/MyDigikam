/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Performs face detection and recognition
 *
 * SPDX-FileCopyrightText: 2024      by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QIcon>
#include <QExplicitlySharedDataPointer>

// local includes

#include "digikam_export.h"
#include "dimg.h"

namespace Digikam
{

class DIGIKAM_EXPORT MLPipelinePackageNotify : public QSharedData
// class DIGIKAM_EXPORT MLPipelinePackageNotify
{
public:

    explicit MLPipelinePackageNotify(const QString _name, const QString _path, int _processed, const DImg& _thumbnail);
    explicit MLPipelinePackageNotify(const QString _name, const QString _path, int _processed, const QIcon& _thumbnail);
    virtual ~MLPipelinePackageNotify();

    const QString   name;
    const QString   path;
    int             processed       = 0;     
    QIcon           thumbnail;

    typedef QExplicitlySharedDataPointer<MLPipelinePackageNotify> Ptr;

private:

    MLPipelinePackageNotify()                           = delete;
    MLPipelinePackageNotify(MLPipelinePackageNotify&)   = delete;

};

}