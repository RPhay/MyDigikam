/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "GeoSceneProperty.h"

// Local includes

#include "GeoSceneTypes.h"
#include "digikam_debug.h"

namespace Marble
{

GeoSceneProperty::GeoSceneProperty(const QString& name)
    : m_name(name),
      m_available(false),
      m_defaultValue(false),
      m_value(false)
{
}

const char* GeoSceneProperty::nodeType() const
{
    return GeoSceneTypes::GeoScenePropertyType;
}

QString GeoSceneProperty::name() const
{
    return m_name;
}

bool GeoSceneProperty::available() const
{
    return m_available;
}

void GeoSceneProperty::setAvailable(bool available)
{
    m_available = available;
}

bool GeoSceneProperty::defaultValue() const
{
    return m_defaultValue;
}

void GeoSceneProperty::setDefaultValue(bool defaultValue)
{
    m_defaultValue = defaultValue;
    setValue(defaultValue);
}

bool GeoSceneProperty::value() const
{
    return m_value;
}

void GeoSceneProperty::setValue(bool value)
{
    if (m_value == value)
    {
        return;
    }

    m_value = value;
    //    qCDebug(DIGIKAM_MARBLE_LOG) << "GeoSceneProperty: Setting " << m_name << "to" << m_value;
    Q_EMIT valueChanged(m_name, m_value);
}

} // namespace Marble

#include "moc_GeoSceneProperty.cpp"
