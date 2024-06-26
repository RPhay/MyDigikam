// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#pragma once

#include <QString>
#include <QVariant>

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataDataPrivate
{
public:

    QVariant m_value;
    QString  m_name;
    QString  m_displayName;
};

} // namespace Marble
