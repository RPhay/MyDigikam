/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-09-27
 * Description : Database engine SQL query
 *
 * SPDX-FileCopyrightText: 2009-2010 by Holger Foerster <Hamsi2k at freenet dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dbenginesqlquery.h"

namespace Digikam
{

DbEngineSqlQuery::DbEngineSqlQuery(const QSqlQuery& other)
    : QSqlQuery(other)
{
}

DbEngineSqlQuery::DbEngineSqlQuery(const QSqlDatabase& db)
    : QSqlQuery(db)
{
}

DbEngineSqlQuery& DbEngineSqlQuery::operator=(const DbEngineSqlQuery& other)
{
    QSqlQuery::operator=(other);
    m_query = other.m_query;

    return *this;
}

void DbEngineSqlQuery::addBindValueList(const QVariantList& values)
{
    Q_FOREACH (const QVariant& val, values)
    {
        QSqlQuery::addBindValue(val);
    }
}

bool DbEngineSqlQuery::prepare(const QString& query)
{
    bool result = QSqlQuery::prepare(query);
    m_query     = query;

    return result;
}

QString DbEngineSqlQuery::lastQuery() const
{
    return m_query;
}

} // namespace Digikam
