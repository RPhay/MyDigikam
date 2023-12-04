// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2004-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
// SPDX-FileCopyrightText: 2010 Bastian Holst <bastianholst@gmx.de>
// SPDX-FileCopyrightText: 2011-2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

// Self
#include "CelestialSortFilterProxyModel.h"

namespace Marble {

CelestialSortFilterProxyModel::CelestialSortFilterProxyModel()
{
    setupPriorities();
    setupMoonsList();
    setupDwarfsList();
}

CelestialSortFilterProxyModel::~CelestialSortFilterProxyModel() {}


QVariant CelestialSortFilterProxyModel::data( const QModelIndex &index, int role ) const
{
    QVariant var = QSortFilterProxyModel::data( index, role );
    if ( role == Qt::DisplayRole && index.column() == 0 ) {
        QString newOne = var.toString();
        if (newOne == tr("Moon")) {
            return QString(QLatin1String("  ") + tr("Moon"));
        } else if ( m_moons.contains( newOne.toLower() ) ) {
            return QString(QLatin1String("  ") + newOne + QLatin1String(" (") + tr("moon") + QLatin1Char(')'));
        } else if ( m_dwarfs.contains( newOne.toLower() ) ) {
            return QString(newOne + QLatin1String(" (") + tr("dwarf planet") + QLatin1Char(')'));
        }
        return newOne;
    } else {
        return var;
    }
}


void CelestialSortFilterProxyModel::setupPriorities()
{
    // TODO: create priority on the model side (Planet Class) by taking the distance to the "home planet/home star" into account
    // here we will set m_priority for default order
    int prefix = 100;

    m_priority[QString::fromUtf8("sun")] = prefix;
    m_priority[QString::fromUtf8("mercury")] = prefix--;
    m_priority[QString::fromUtf8("venus")] = prefix--;
    m_priority[QString::fromUtf8("earth")] = prefix--;
    m_priority[QString::fromUtf8("moon")] = prefix--;
    m_priority[QString::fromUtf8("mars")] = prefix--;

    m_priority[QString::fromUtf8("jupiter")] = prefix--;
    // Moons of Jupiter
    m_priority[QString::fromUtf8("io")] = prefix--;
    m_priority[QString::fromUtf8("europa")] = prefix--;
    m_priority[QString::fromUtf8("ganymede")] = prefix--;
    m_priority[QString::fromUtf8("callisto")] = prefix--;

    m_priority[QString::fromUtf8("saturn")] = prefix--;
    // Moons of Saturn
    m_priority[QString::fromUtf8("mimas")] = prefix--;
    m_priority[QString::fromUtf8("enceladus")] = prefix--;
    m_priority[QString::fromUtf8("thetys")] = prefix--;
    m_priority[QString::fromUtf8("dione")] = prefix--;
    m_priority[QString::fromUtf8("rhea")] = prefix--;
    m_priority[QString::fromUtf8("titan")] = prefix--;
    m_priority[QString::fromUtf8("iapetus")] = prefix--;

    m_priority[QString::fromUtf8("uranus")] = prefix--;
    m_priority[QString::fromUtf8("neptune")] = prefix--;
    m_priority[QString::fromUtf8("pluto")] = prefix--;
    m_priority[QString::fromUtf8("ceres")] = prefix--;
}

void CelestialSortFilterProxyModel::setupMoonsList()
{
    m_moons.push_back(QString::fromUtf8("moon"));
    m_moons.push_back(QString::fromUtf8("europa"));
    m_moons.push_back(QString::fromUtf8("ganymede"));
    m_moons.push_back(QString::fromUtf8("callisto"));
    m_moons.push_back(QString::fromUtf8("mimas"));
    m_moons.push_back(QString::fromUtf8("enceladus"));
    m_moons.push_back(QString::fromUtf8("thetys"));
    m_moons.push_back(QString::fromUtf8("dione"));
    m_moons.push_back(QString::fromUtf8("rhea"));
    m_moons.push_back(QString::fromUtf8("titan"));
    m_moons.push_back(QString::fromUtf8("iapetus"));
}

void CelestialSortFilterProxyModel::setupDwarfsList()
{
    m_dwarfs.push_back(QString::fromUtf8("pluto"));
    m_dwarfs.push_back(QString::fromUtf8("ceres"));
}

bool CelestialSortFilterProxyModel::lessThan( const QModelIndex &left, const QModelIndex &right ) const
{
    const QString nameLeft = sourceModel()->index( left.row(), 1 ).data().toString();
    const QString nameRight = sourceModel()->index( right.row(), 1 ).data().toString();
    const QString first = nameLeft.toLower();
    const QString second = nameRight.toLower();

    // both are in the list
    if ( m_priority.contains( first ) && m_priority.contains( second ) ) {
        return m_priority[first] > m_priority[second];
    }

    // only left in the list
    if ( m_priority.contains( first ) && !m_priority.contains( second ) ) {
        return true;
    }

    // only right in the list
    if (!m_priority.contains( first ) && m_priority.contains( second ) ) {
        return false;
    }

    return QSortFilterProxyModel::lessThan( left, right );
}

}

#include "moc_CelestialSortFilterProxyModel.cpp"
