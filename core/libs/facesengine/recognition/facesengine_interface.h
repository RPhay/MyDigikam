/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : The recognition wrapper
 *
 * Copyright (C)      2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C)      2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#ifndef FACESENGINE_INTERFACE_H
#define FACESENGINE_INTERFACE_H

// Qt includes

#include <QExplicitlySharedDataPointer>
#include <QImage>
#include <QList>
#include <QMap>
#include <QVariant>

// Local includes

#include "digikam_export.h"
#include "identity.h"
#include "dataproviders.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT FacesEngineInterface
{
public:

    explicit FacesEngineInterface();
    ~FacesEngineInterface();

public:

    /**
     * Checks the integrity and returns true if everything is fine.
     */
    bool integrityCheck();

    /**
     * Shrinks the database.
     */
    void vacuum();

public:

    // --- Backend parameters (facesengine_interface_setup.cpp) --------------------------
    /**
     * Tunes backend parameters.
     * Available parameters:
     * "accuracy", synonymous: "threshold", range: 0-1, type: float
     * Determines recognition threshold, 0->accept very insecure recognitions, 1-> be very sure about a recognition.
     *
     * "k-nearest" : limit the number of nearest neighbors for KNN
     */
    void        setParameter(const QString& parameter, const QVariant& value);
    void        setParameters(const QVariantMap& parameters);
    QVariantMap parameters() const;

private:

    class Private;
    Private* d;

    // Hidden assignment operator.
    FacesEngineInterface& operator=(const FacesEngineInterface&);
};

} // namespace Digikam

#endif // FACESENGINE_INTERFACE_H
