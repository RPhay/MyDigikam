/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 02-02-2012
 * Description : Face database interface to train identities.
 *
 * Copyright (C) 2012-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
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

#include "facedb_p.h"

namespace Digikam
{

int FaceDb::addIdentity() const
{
    QVariant id;
    d->db->execSql(QLatin1String("INSERT INTO Identities (`type`) VALUES (0);"),
                   nullptr, &id);

    return id.toInt();
}

void FaceDb::updateIdentity(const Identity& p)
{
    d->db->execSql(QLatin1String("DELETE FROM IdentityAttributes WHERE id=?;"),
                   p.id());

    const QMap<QString, QString> map = p.attributesMap();
    QMap<QString, QString>::const_iterator it;

    for (it = map.constBegin() ; it != map.constEnd() ; ++it)
    {
        d->db->execSql(QLatin1String("INSERT INTO IdentityAttributes (id, attribute, `value`) VALUES (?, ?,?);"),
                       p.id(), it.key(), it.value());
    }
}

void FaceDb::deleteIdentity(int id)
{
    // Triggers do the rest

    d->db->execSql(QLatin1String("DELETE FROM Identities WHERE id=?;"), id);
}

void FaceDb::deleteIdentity(const QString& uuid)
{
    QList<QVariant> ids;
    d->db->execSql(QLatin1String("SELECT Identities.id FROM Identities LEFT JOIN IdentityAttributes ON "
                                 " Identities.id=IdentityAttributes.id WHERE "
                                 " IdentityAttributes.attribute='uuid' AND IdentityAttributes.value=?;"),
                   uuid, &ids);

    if (ids.size() == 1)
    {
        deleteIdentity(ids.first().toInt());
    }
    else
    {
        qCWarning(DIGIKAM_FACEDB_LOG) << "Cannot delete identity with uuid "
                                      << uuid << ". There are " << ids.size()
                                      << " identities with this uuid.";
    }
}

void FaceDb::clearIdentities()
{
    d->db->execSql(QLatin1String("DELETE FROM Identities;"));
}

QList<Identity> FaceDb::identities() const
{
    QList<QVariant> ids;
    QList<Identity> results;
    d->db->execSql(QLatin1String("SELECT id FROM Identities;"),
                   &ids);

    foreach (const QVariant& v, ids)
    {
        QList<QVariant> values;
        Identity p;
        p.setId(v.toInt());
        d->db->execSql(QLatin1String("SELECT attribute, `value` FROM IdentityAttributes WHERE id=?;"),
                       p.id(), &values);

        for (QList<QVariant>::const_iterator it = values.constBegin() ; it != values.constEnd() ; )
        {
            QString attribute = it->toString();
            ++it;
            QString value     = it->toString();
            ++it;

            p.setAttribute(attribute, value);
        }

        results << p;
    }

    return results;
}

QList<int> FaceDb::identityIds() const
{
    QList<QVariant> ids;
    d->db->execSql(QLatin1String("SELECT id FROM Identities;"),
                   &ids);

    QList<int> results;

    foreach (const QVariant& var, ids)
    {
        results << var.toInt();
    }

    return results;
}

int FaceDb::getNumberOfIdentities() const
{
    QList<QVariant> values;

    d->db->execSql(QString::fromUtf8("SELECT COUNT(id) FROM Identities;"),
                   &values);

    if (values.isEmpty())
    {
        return 0;
    }

    return values.first().toInt();
}

} // namespace Digikam
