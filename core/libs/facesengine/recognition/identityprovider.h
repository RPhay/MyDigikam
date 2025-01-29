/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Face classifier training data provider
 *
 * SPDX-FileCopyrightText: 2024-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "digikam_export.h"
#include "digikam_opencv.h"
#include "identity.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT IdentityProvider
{
public:

    static IdentityProvider* instance();

    bool checkRetrainingRequired() const;

    /**
     * Checks the integrity and returns true if everything is fine.
     */
    bool integrityCheck();

    /**
     * Shrinks the database.
     */
    void vacuum();

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
    Identity findIdentity(const QMultiMap<QString, QString>& attributes)    const;

    /**
     * Adds a new identity with the specified attributes.
     * Please note that a UUID is automatically generated.
     */
    Identity addIdentity(const QMultiMap<QString, QString>& attributes);

    /**
     * This is the debug version of addIdentity, so the identity is only added
     * to identityCache, but not into the recognition database.
     */
    Identity addIdentityDebug(const QMultiMap<QString, QString>& attributes);

    // /**
    //  * Adds or sets, resp., the attributes of an identity.
    //  */
    // void addIdentityAttributes(int id, const QMultiMap<QString, QString>& attributes);
    // void addIdentityAttribute(int id, const QString& attribute, const QString& value);
    // void setIdentityAttributes(int id, const QMultiMap<QString, QString>& attributes);

    /**
     * Deletes an identity from the database.
     */
    void deleteIdentity(const Identity& identityToBeDeleted);

    /**
     * Deletes a list of identities from the database.
     */
    void deleteIdentities(QList<Identity> identitiesToBeDeleted);

    /**
     * Deletes the training image for the given hash,
     * leaving the identity as such in the database.
     */
    bool clearTraining(const QString& hash);

    /**
     * clears all identites and face training
     * from the recognition DB
     */
    void clearAllTraining();

    /**
     * add the face features and hash to the recognition DB
     * returns the ID of the new row
     */
    int addTraining(const Identity& identity, const QString& hash, const cv::Mat& feature);

    /**
     * checks if the id exists in the recognition DB
     */
    bool isValidId(int label)                                               const;

    static QString FaceTrainingVersion;
    static QString ExtractorModel;

protected:

    bool initialize();

    /**
     * Deletes a list of identities from the database.
     */
    cv::Ptr<cv::ml::TrainData> getTrainingData()                            const;

    bool addIdentityFace(const Identity& identity, QString& hash, cv::Mat embedding);
    bool deleteIdentityFace(const Identity& identity, QString& hash);


private:

    IdentityProvider();
    ~IdentityProvider();

    void addSeedTraining();

    bool identityContains(const Identity& identity,
                          const QString& attribute,
                          const QString& value)                             const;

    Identity findByAttribute(const QString& attribute,
                             const QString& value)                          const;

    Identity findByAttributes(const QString& attribute,
                              const QMultiMap<QString, QString>& valueMap)  const;

    bool trainingRemoveConcurrent();

    class Private;
    static Private* d;

private:

    // Disable
    IdentityProvider(const IdentityProvider&)                     = delete;

private:

    friend class IdentityProviderCreator;
    friend class Identity;
    friend class FaceClassifier;
};

} // namespace Digikam
