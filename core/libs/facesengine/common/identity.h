/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2013-05-18
 * Description : Wrapper class for face recognition
 *
 * SPDX-FileCopyrightText:      2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2014-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FACESENGINE_IDENTITY_H
#define DIGIKAM_FACESENGINE_IDENTITY_H

// Qt includes

#include <QMultiMap>
#include <QString>
#include <QExplicitlySharedDataPointer>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT Identity
{

public:

    /**
     * Wraps a face recognition Identity.
     * An identity refers to a natural person.
     * There is an internal id which is used the FacesEngine storage,
     * and a number of attributes which map the identity to the outside.
     * Prespecified attributes:
     * "fullName"   The full name as on the ID card, e.g. "Peter Brown"
     * "name"       The person's name without further specification, e.g. "Peter" or "Dad"
     * "uuid"        A UUID that is assigned to each new identity at creation.
     *
     * For fullName and name, multiple values are allowed.
     *
     * Attributes can be used to map an identity to other fields and services where
     * natural persons play a role.
     */
    Identity();
    Identity(const Identity& other);
    ~Identity();

    Identity& operator=(const Identity& other);

    bool isNull()                               const;
    bool operator==(const Identity& other)      const;

    /**
     * Id value accessor.
     */
    int  id()                                   const;
    void setId(int id);

    /**
     * Attribute value accessor.
     */
    QString attribute(const QString& att)       const;
    void    setAttribute(const QString& att,
                         const QString& val);

    /**
     * Attributes map accessor.
     */
    QMultiMap<QString, QString> attributesMap() const;
    void setAttributesMap(const QMultiMap<QString, QString>& attributes);

private:

    class Private;
    QExplicitlySharedDataPointer<Private> d;
};

} // namespace Digikam

#endif // DIGIKAM_FACESENGINE_IDENTITY_H
