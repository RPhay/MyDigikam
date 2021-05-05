/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : The recognition wrapper
 *
 * Copyright (C)      2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C)      2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef FACIAL_RECOGNITION_WRAPPER_H
#define FACIAL_RECOGNITION_WRAPPER_H

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

class DIGIKAM_GUI_EXPORT FacialRecognitionWrapper
{
public:

    explicit FacialRecognitionWrapper();
    FacialRecognitionWrapper(const FacialRecognitionWrapper&);

    ~FacialRecognitionWrapper();

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
    QVariantMap parameters()                                                const;

    // --- Identity management (facesengine_interface_identity.cpp) -----------------------------------------

    /// NOTE: For the documentation of standard attributes, see identity.h

    /**
     * Returns all identities known to the database
     */
    QList<Identity> allIdentities()                                         const;
    Identity        identity(int id)                                        const;

    /**
     * Finds the first identity with matching attribute - value.
     * Returns a null identity if no match is found or attribute is empty.
     */
    Identity findIdentity(const QString& attribute, const QString& value)   const;

    /**
     * Finds the identity matching the given attributes.
     * Attributes are first checked with knowledge of their meaning.
     * Secondly, all unknown attributes are used.
     * Returns a null Identity if no match is possible or the map is empty.
     */
    Identity findIdentity(const QMap<QString, QString>& attributes)         const;

    /**
     * Adds a new identity with the specified attributes.
     * Please note that a UUID is automatically generated.
     */
    Identity addIdentity(const QMap<QString, QString>& attributes);

    /**
     * This is the debug version of addIdentity, so the identity is only added
     * to identityCache, but not into the recognition database.
     */
    Identity addIdentityDebug(const QMap<QString, QString>& attributes);

    /**
     * Adds or sets, resp., the attributes of an identity.
     */
    void addIdentityAttributes(int id, const QMap<QString, QString>& attributes);
    void addIdentityAttribute(int id, const QString& attribute, const QString& value);
    void setIdentityAttributes(int id, const QMap<QString, QString>& attributes);

    /**
     * Deletes an identity from the database.
     */
    void deleteIdentity(const Identity& identityToBeDeleted);

    /**
     * Deletes a list of identities from the database.
     */
    void deleteIdentities(QList<Identity> identitiesToBeDeleted);

    // --- Faces Training management (facesengine_interface_training.cpp) ----------------------------------------------------
    /**
     * Performs training.
     * The identities which have new images to be trained are given.
     * An empty list means that all identities are checked.
     *
     * All needed data will be queried from the provider.
     *
     * An identifier for the current training context is given,
     * which can identify the application or group of collections.
     * (It is assumed that training from different contexts is based on
     * non-overlapping collections of images. Keep it always constant for your app.)
     */
    void train(const QList<Identity>& identitiesToBeTrained,
               TrainingDataProvider* const data,
               const QString& trainingContext);
    void train(const Identity& identityToBeTrained,
               TrainingDataProvider* const data,
               const QString& trainingContext);

    /**
     * Performs training by using image data directly.
     *
     * These are convenience functions for simple setups.
     * If you want good performance and/or a more versatile implementation, be sure to
     * implement your own TrainingDataProvider and use one of the above functions.
     */
    void train(const Identity& identityToBeTrained,
               QImage* image,
               const QString& trainingContext);
    void train(const Identity& identityToBeTrained,
               const QList<QImage*>& images,
               const QString& trainingContext);

    /**
     * Deletes the training data for all identities,
     * leaving the identities as such in the database.
     */
    void clearAllTraining(const QString& trainingContext = QString());

    /**
     * Deletes the training data for the given identity,
     * leaving the identity as such in the database.
     */
    void clearTraining(const QList<Identity>& identitiesToClean,
                       const QString& trainingContext = QString());

    // --- Recognition management (facesengine_interface_recognize.cpp) -------------------

    /**
     * Returns the recommended size if you want to scale face images for recognition.
     * Larger images can be passed, but may be downscaled.
     */
    // TODO : review to see if this function is necessary
    //int recommendedImageSize(const QSize& availableSize = QSize()) const;

    /**
     * Performs recognition.
     * The face details to be recognized are passed by the provider.
     * For each entry in the provider, in 1-to-1 mapping,
     * a recognized identity or the null identity is returned.
     */
    QList<Identity> recognizeFaces(ImageListProvider* const images);
    QList<Identity> recognizeFaces(const QList<QImage*>& images);
    Identity        recognizeFace(QImage* const image);

private:

    // Disable
    FacialRecognitionWrapper& operator=(const FacialRecognitionWrapper&) = delete;

private:

    class Private;
    static Private* d;
};

} // namespace Digikam

#endif // FACIAL_RECOGNITION_WRAPPER_H
