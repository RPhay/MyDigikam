/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : Identity management of recognition wrapper
 *
 * Copyright (C)      2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C)      2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
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

#include "facialrecognition_wrapper_p.h"

namespace Digikam
{
/**
 * NOTE: Takes care that there may be multiple values of attribute in identity's attributes
 */
bool FacialRecognitionWrapper::Private::identityContains(const Identity& identity,
                                                         const QString&  attribute,
                                                         const QString&  value)
{
    const QMap<QString, QString> map          = identity.attributesMap();
    QMap<QString, QString>::const_iterator it = map.constFind(attribute);

    for ( ; (it != map.constEnd()) && (it.key() == attribute) ; ++it)
    {
        if (it.value() == value)
        {
            return true;
        }
    }

    return false;
}

Identity FacialRecognitionWrapper::Private::findByAttribute(const QString& attribute,
                                                            const QString& value) const
{
    foreach (const Identity& identity, identityCache)
    {
        if (identityContains(identity, attribute, value))
        {
            return identity;
        }
    }

    return Identity();
}

/**
 * NOTE: Takes care that there may be multiple values of attribute in valueMap
 */
Identity FacialRecognitionWrapper::Private::findByAttributes(const QString& attribute,
                                                             const QMap<QString, QString>& valueMap) const
{
    QMap<QString, QString>::const_iterator it = valueMap.find(attribute);

    for ( ; (it != valueMap.end()) && (it.key() == attribute) ; ++it)
    {
        foreach (const Identity& identity, identityCache)
        {
            if (identityContains(identity, attribute, it.value()))
            {
                return identity;
            }
        }
    }

    return Identity();
}

// -----------------------------------------------------------------------

QList<Identity> FacialRecognitionWrapper::allIdentities() const
{
    if (!d || !d->dbAvailable)
    {
        return QList<Identity>();
    }

    QMutexLocker lock(&d->mutex);

    return (d->identityCache.values());
}

Identity FacialRecognitionWrapper::identity(int id) const
{
    if (!d || !d->dbAvailable)
    {
        return Identity();
    }

    QMutexLocker lock(&d->mutex);

    return (d->identityCache.value(id));
}

Identity FacialRecognitionWrapper::findIdentity(const QString& attribute, const QString& value) const
{
    if (!d || !d->dbAvailable || attribute.isEmpty())
    {
        return Identity();
    }

    QMutexLocker lock(&d->mutex);

    return (d->findByAttribute(attribute, value));
}

Identity FacialRecognitionWrapper::findIdentity(const QMap<QString, QString>& attributes) const
{
    if (!d || !d->dbAvailable || attributes.isEmpty())
    {
        return Identity();
    }

    QMutexLocker lock(&d->mutex);

    Identity match;

    // First and foremost, UUID

    QString uuid = attributes.value(QLatin1String("uuid"));
    match        = d->findByAttribute(QLatin1String("uuid"), uuid);

    if (!match.isNull())
    {
        return match;
    }

    // A negative UUID match, with a given UUID, precludes any further search

    if (!uuid.isNull())
    {
        return Identity();
    }

    // full name

    match = d->findByAttributes(QLatin1String("fullName"), attributes);

    if (!match.isNull())
    {
        return match;
    }

    // name

    match = d->findByAttributes(QLatin1String("name"), attributes);

    if (!match.isNull())
    {
        return match;
    }

    QMap<QString, QString>::const_iterator it;

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

        match = d->findByAttribute(it.key(), it.value());

        if (!match.isNull())
        {
            return match;
        }
    }

    return Identity();
}

Identity FacialRecognitionWrapper::addIdentity(const QMap<QString, QString>& attributes)
{
    if (!d || !d->dbAvailable)
    {
        return Identity();
    }

    QMutexLocker lock(&d->mutex);

    if (attributes.contains(QLatin1String("uuid")))
    {
        Identity matchByUuid = findIdentity(QLatin1String("uuid"), attributes.value(QLatin1String("uuid")));

        if (!matchByUuid.isNull())
        {
            // This situation is not well defined.

            qCDebug(DIGIKAM_FACESENGINE_LOG) << "Called addIdentity with a given UUID, and there is such a UUID already in the database."
                                             << "The existing identity is returned without adjusting properties!";

            return matchByUuid;
        }
    }

    Identity identity;
    {
        FaceDbOperationGroup group;
        int id = FaceDbAccess().db()->addIdentity();
        identity.setId(id);
        identity.setAttributesMap(attributes);
        identity.setAttribute(QLatin1String("uuid"), QUuid::createUuid().toString());
        FaceDbAccess().db()->updateIdentity(identity);
    }

    d->identityCache[identity.id()] = identity;

    return identity;
}

Identity FacialRecognitionWrapper::addIdentityDebug(const QMap<QString, QString>& attributes)
{
    Identity identity;
    {
        identity.setId(attributes[QLatin1String("name")].toInt());
        identity.setAttributesMap(attributes);
        identity.setAttribute(QLatin1String("uuid"), QUuid::createUuid().toString());
    }

    d->identityCache[identity.id()] = identity;

    return identity;
}

void FacialRecognitionWrapper::addIdentityAttributes(int id, const QMap<QString, QString>& attributes)
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    QMutexLocker lock(&d->mutex);

    QHash<int, Identity>::iterator it = d->identityCache.find(id);

    if (it != d->identityCache.end())
    {
        QMap<QString, QString> map = it->attributesMap();
        map.unite(attributes);
        it->setAttributesMap(map);
        FaceDbAccess().db()->updateIdentity(*it);
    }
}

void FacialRecognitionWrapper::addIdentityAttribute(int id, const QString& attribute, const QString& value)
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    QMutexLocker lock(&d->mutex);
    QHash<int, Identity>::iterator it = d->identityCache.find(id);

    if (it != d->identityCache.end())
    {
        QMap<QString, QString> map = it->attributesMap();
        map.insertMulti(attribute, value);
        it->setAttributesMap(map);
        FaceDbAccess().db()->updateIdentity(*it);
    }
}

void FacialRecognitionWrapper::setIdentityAttributes(int id, const QMap<QString, QString>& attributes)
{
    if (!d || !d->dbAvailable)
    {
            return;
    }

    QMutexLocker lock(&d->mutex);
    QHash<int, Identity>::iterator it = d->identityCache.find(id);

    if (it != d->identityCache.end())
    {
        it->setAttributesMap(attributes);
        FaceDbAccess().db()->updateIdentity(*it);
    }
}

void FacialRecognitionWrapper::deleteIdentity(const Identity& identityToBeDeleted)
{
    if (!d || !d->dbAvailable || identityToBeDeleted.isNull())
    {
        return;
    }

    QMutexLocker lock(&d->mutex);

    FaceDbAccess().db()->deleteIdentity(identityToBeDeleted.id());
    d->identityCache.remove(identityToBeDeleted.id());
}

void FacialRecognitionWrapper::deleteIdentities(QList<Identity> identitiesToBeDeleted)
{
    QList<Identity>::iterator identity = identitiesToBeDeleted.begin();

    while (identity != identitiesToBeDeleted.end())
    {
        deleteIdentity(*identity);

        identity = identitiesToBeDeleted.erase(identity);
    }
}

} // namespace Digikam
