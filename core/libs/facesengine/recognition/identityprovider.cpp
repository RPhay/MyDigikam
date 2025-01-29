/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Face classifier
 *
 * SPDX-FileCopyrightText: 2024-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "identityprovider.h"

// Qt includes

#include <QException>
#include <QReadWriteLock>
#include <QUuid>
#include <QThread>
#include <QThreadPool>
#include <QFuture>
#include <QtConcurrent>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "applicationsettings.h"
#include "facedboperationgroup.h"
#include "facedbaccess.h"
#include "facedb.h"
#include "facescansettings.h"
#include "faceclassifier.h"
#include "recognitiontrainingupdatequeue.h"

namespace Digikam
{

IdentityProvider::Private* IdentityProvider::d = nullptr;
QString IdentityProvider::FaceTrainingVersion  = QLatin1String("8.6.0");
QString IdentityProvider::ExtractorModel       = QLatin1String("SFace");

class Q_DECL_HIDDEN IdentityProvider::Private
{
public:

    int                             ref                     = 1;
    bool                            dbAvailable             = false;
    int                             seedMax                 = 0;

    QHash<int, Identity>            identityCache;

    QReadWriteLock                  trainingLock;
/*
    QMutex                          trainingMutex;
*/
    RecognitionTrainingUpdateQueue  removeQueue;
    QThreadPool*                    removeThreadPool        = nullptr;
    QFuture<bool>                   removeThreadResult;
};

class Q_DECL_HIDDEN IdentityProviderCreator
{
public:

    IdentityProvider object;
};

Q_GLOBAL_STATIC(IdentityProviderCreator, IdentityProviderCreator)

IdentityProvider::IdentityProvider()
{
    if (!d)
    {
        d = new Private;

        if (!initialize())
        {
            QException().raise();
        }

        d->removeThreadPool = new QThreadPool();

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

        // Priority should be equal or greater than trainer or recognizer threads.

        d->removeThreadPool->setThreadPriority(QThread::NormalPriority);

#endif

        // We only need 1 thread for the training remover.

        d->removeThreadPool->setMaxThreadCount(1);

        // Run the remove queue listener thread.

        d->removeThreadResult = QtConcurrent::run(d->removeThreadPool,

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

                                                  &IdentityProvider::trainingRemoveConcurrent,
                                                  this

#else

                                                  this,
                                                  &IdentityProvider::trainingRemoveConcurrent

#endif
                                                 );

    }

    else
    {
        ++(d->ref);
    }
}

IdentityProvider::~IdentityProvider()
{
    --(d->ref);

    if (d->ref == 0)
    {
        d->removeQueue.push(d->removeQueue.endSignal());

        while (d->removeThreadResult.isRunning())
        {
            QThread::msleep(10);
        }

        delete d;
        d = nullptr;
    }
}

IdentityProvider* IdentityProvider::instance()
{
    return &IdentityProviderCreator->object;
}

bool IdentityProvider::initialize()
{
    DbEngineParameters params   = CoreDbAccess::parameters().faceParameters();
    params.setFaceDatabasePath(CoreDbAccess::parameters().faceParameters().getFaceDatabaseNameOrDir());
    FaceDbAccess::setParameters(params, FaceScanSettings::FaceRecognitionModel::SFace);

    d->dbAvailable              = FaceDbAccess::checkReadyForUse(nullptr);

    if (!d->dbAvailable || !integrityCheck())
    {
        return false;
    }

/*
    // check for seed identities

    Identity id = FaceDbAccess().db()->identity(1);

    if (id.attributesMap().isEmpty())
    {
        addSeedTraining();
    }
*/
    const auto ids = FaceDbAccess().db()->identities();

    for (const Identity& identity : ids)
    {
        d->identityCache[identity.id()] = identity;
    }

/*
    QMultiMap<QString, QString> attributes;
    attributes.insert(QLatin1String("name"), QLatin1String("digiKam seed identity MAX"));
    id = findIdentity(attributes);
    d->seedMax = id.id();
*/
    return true;
}

bool IdentityProvider::checkRetrainingRequired() const
{
    if (!d || !d->dbAvailable)
    {
        // don't use this if we're not initialized

        qCDebug(DIGIKAM_FACESENGINE_LOG) << "IdentityProvider not initialized or database not available";
        QException().raise();
    }

    // return false if we don't have any identities in the database

    if (d->identityCache.isEmpty())
    {
        // write the current version to the database

        FaceDbAccess().db()->setTrainingVersionInfo(IdentityProvider::FaceTrainingVersion, IdentityProvider::ExtractorModel);
        return false;
    }

    // Check if the face training version is up-to-date.
    
    QString version;
    QString model;

    FaceDbAccess().db()->getTrainingVersionInfo(version, model);

    if (IdentityProvider::FaceTrainingVersion == version &&
        IdentityProvider::ExtractorModel == model)
    {
        // retraining not needed

        return false;
    }

    // retraining needed

    return true;
}


bool IdentityProvider::integrityCheck()
{
    if (!d || !d->dbAvailable)
    {
        return false;
    }

    d->trainingLock.lockForRead();

    bool result = FaceDbAccess().db()->integrityCheck();

    d->trainingLock.unlock();

    return result;
}

void IdentityProvider::vacuum()
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    d->trainingLock.lockForWrite();

    FaceDbAccess().db()->vacuum();

    d->trainingLock.unlock();
}

// -----------------------------------------------------------------------

QList<Identity> IdentityProvider::allIdentities() const
{
    if (!d || !d->dbAvailable)
    {
        return QList<Identity>();
    }

    d->trainingLock.lockForRead();

    QList<Identity> result = (d->identityCache.values());

    d->trainingLock.unlock();

    return result;
}

Identity IdentityProvider::identity(int id) const
{
    if (!d || !d->dbAvailable)
    {
        return Identity();
    }

    d->trainingLock.lockForRead();

    Identity result = d->identityCache.value(id);

    d->trainingLock.unlock();

    return result;
}

Identity IdentityProvider::findIdentity(const QString& attribute, const QString& value) const
{
    if (!d || !d->dbAvailable || attribute.isEmpty())
    {
        return Identity();
    }

    d->trainingLock.lockForRead();

    Identity result = findByAttribute(attribute, value);

    d->trainingLock.unlock();

    return result;
}

Identity IdentityProvider::findIdentity(const QMultiMap<QString, QString>& attributes) const
{
    if (!d || !d->dbAvailable || attributes.isEmpty())
    {
        return Identity();
    }

    d->trainingLock.lockForRead();

    Identity match;

    // First and foremost, UUID.

    QString uuid = attributes.value(QLatin1String("uuid"));
    match        = findByAttribute(QLatin1String("uuid"), uuid);

    if (!match.isNull())
    {
        d->trainingLock.unlock();

        return match;
    }

    // A negative UUID match, with a given UUID, precludes any further search.

    if (!uuid.isNull())
    {
        d->trainingLock.unlock();

        return Identity();
    }

    // Full name.

    match = findByAttributes(QLatin1String("fullName"), attributes);

    if (!match.isNull())
    {
        d->trainingLock.unlock();

        return match;
    }

    // Name.

    match = findByAttributes(QLatin1String("name"), attributes);

    if (!match.isNull())
    {
        d->trainingLock.unlock();

        return match;
    }

    QMultiMap<QString, QString>::const_iterator it;

    for (it = attributes.begin() ; it != attributes.end() ; ++it)
    {
        if (
            (it.key() == QLatin1String("uuid"))     ||
            (it.key() == QLatin1String("fullName")) ||
            (it.key() == QLatin1String("name"))
        )
        {
            continue;
        }

        match = findByAttribute(it.key(), it.value());

        if (!match.isNull())
        {
            d->trainingLock.unlock();

            return match;
        }
    }

    d->trainingLock.unlock();

    return Identity();
}

Identity IdentityProvider::addIdentity(const QMultiMap<QString, QString>& attributes)
{
    if (!d || !d->dbAvailable)
    {
        return Identity();
    }

    if (attributes.contains(QLatin1String("uuid")))
    {
        d->trainingLock.lockForRead();

        Identity matchByUuid = findIdentity(QLatin1String("uuid"), attributes.value(QLatin1String("uuid")));

        d->trainingLock.unlock();

        if (!matchByUuid.isNull())
        {
            // This situation is not well defined.

            qCDebug(DIGIKAM_FACESENGINE_LOG) << "Called addIdentity with a given UUID, "
                                                "and there is such a UUID already in the database."
                                             << "The existing identity is returned without adjusting properties!";

            return matchByUuid;
        }
    }

    Identity identity;
    {
        FaceDbOperationGroup group;

        d->trainingLock.lockForWrite();

        int id = FaceDbAccess().db()->addIdentity();
        identity.setId(id);
        identity.setAttributesMap(attributes);
        identity.setAttribute(QLatin1String("uuid"), QUuid::createUuid().toString());

        FaceDbAccess().db()->updateIdentity(identity);

        d->trainingLock.unlock();
    }

    d->identityCache[identity.id()] = identity;

    return identity;
}

Identity IdentityProvider::addIdentityDebug(const QMultiMap<QString, QString>& attributes)
{
    Identity identity;
    {
        identity.setId(attributes.value(QLatin1String("name")).toInt());
        identity.setAttributesMap(attributes);
        identity.setAttribute(QLatin1String("uuid"), QUuid::createUuid().toString());
    }

    d->trainingLock.lockForWrite();

    d->identityCache[identity.id()] = identity;

    d->trainingLock.unlock();

    return identity;
}

cv::Ptr<cv::ml::TrainData> IdentityProvider::getTrainingData() const
{
    cv::Ptr<cv::ml::TrainData> trainData = nullptr;

    if (!d || !d->dbAvailable)
    {
        return trainData;
    }

    d->trainingLock.lockForRead();

    trainData = FaceDbAccess().db()->trainData();

    d->trainingLock.unlock();

    return trainData;
}

void IdentityProvider::deleteIdentity(const Identity& identityToBeDeleted)
{
    if (!d || !d->dbAvailable || identityToBeDeleted.isNull())
    {
        return;
    }

    d->trainingLock.lockForWrite();

    FaceDbAccess().db()->deleteIdentity(identityToBeDeleted.id());
    d->identityCache.remove(identityToBeDeleted.id());

    d->trainingLock.unlock();
}

void IdentityProvider::deleteIdentities(QList<Identity> identitiesToBeDeleted)
{
    QList<Identity>::iterator identity = identitiesToBeDeleted.begin();

    while (identity != identitiesToBeDeleted.end())
    {
        deleteIdentity(*identity);

        identity = identitiesToBeDeleted.erase(identity);
    }
}

bool IdentityProvider::clearTraining(const QString& hash)
{
    d->trainingLock.lockForWrite();

    bool result = FaceDbAccess().db()->removeFaceVector(hash);

    d->trainingLock.unlock();

    return result;
}

void IdentityProvider::clearAllTraining()
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    d->trainingLock.lockForWrite();

    d->identityCache.clear();
    FaceDbAccess().db()->clearIdentities();
    FaceDbAccess().db()->clearDNNTraining();

    d->trainingLock.unlock();
}

int IdentityProvider::addTraining(const Identity& identity, const QString& hash, const cv::Mat& feature)
{
    d->trainingLock.lockForWrite();

    int result = FaceDbAccess().db()->insertFaceVector(feature, identity.id(), hash);

    d->trainingLock.unlock();

    return result;
}

bool IdentityProvider::isValidId(int label) const
{
    /*
        return ((label > d->seedMax) && d->identityCache.contains(label));
    */
    return (d->identityCache.contains(label));
}


//--------------------------------------------------------------
// private methods

/**
 * NOTE: Takes care that there may be multiple values of attribute in identity's attributes.
 */
bool IdentityProvider::identityContains(const Identity& identity,
                                        const QString&  attribute,
                                        const QString&  value) const
{
    d->trainingLock.lockForRead();

    const QMultiMap<QString, QString> map          = identity.attributesMap();
    QMultiMap<QString, QString>::const_iterator it = map.constFind(attribute);

    for ( ; (it != map.constEnd()) && (it.key() == attribute) ; ++it)
    {
        if (it.value() == value)
        {
            d->trainingLock.unlock();
            return true;
        }
    }

    d->trainingLock.unlock();

    return false;
}

Identity IdentityProvider::findByAttribute(const QString& attribute,
                                           const QString& value) const
{
    d->trainingLock.lockForRead();

    for (const Identity& identity : std::as_const(d->identityCache))
    {
        if (identityContains(identity, attribute, value))
        {
            // cppcheck-suppress useStlAlgorithm
            d->trainingLock.unlock();

            return identity;
        }
    }

    d->trainingLock.unlock();

    return Identity();
}

/**
 * NOTE: Takes care that there may be multiple values of attribute in valueMap.
 */
Identity IdentityProvider::findByAttributes(const QString& attribute,
                                            const QMultiMap<QString, QString>& valueMap) const
{
    QMultiMap<QString, QString>::const_iterator it = valueMap.find(attribute);

    d->trainingLock.lockForRead();

    for (; (it != valueMap.end()) && (it.key() == attribute) ; ++it)
    {
        for (const Identity& identity : std::as_const(d->identityCache))
        {
            if (identityContains(identity, attribute, it.value()))
            {
                // cppcheck-suppress useStlAlgorithm
                d->trainingLock.unlock();

                return identity;
            }
        }
    }

    d->trainingLock.unlock();

    return Identity();
}

bool IdentityProvider::trainingRemoveConcurrent()
{
    QString hash;

    while (true)
    {
/*
        hash = self->removeQueue.front();
*/
        hash = d->removeQueue.pop_front();

        if (d->removeQueue.endSignal() != hash)
        {
            clearTraining(hash);
            hash.clear();

            FaceClassifier::instance()->retrain();
        }

        else
        {
            break;
        }
    }

    qCDebug(DIGIKAM_FACEDB_LOG) << "IdentityProvider::trainingRemoveConcurrent thread terminated";

    return true;
}

} // namespace Digikam
