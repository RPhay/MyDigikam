// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#pragma once

#include <QString>

#include "digikam_export.h"

#include "GeoDocument.h"

namespace Marble
{

class DIGIKAM_EXPORT GeoSceneLicense : public GeoNode
{
 public:
    enum Attribution {
        Never,
        OptOut,
        OptIn,
        Always
    };

    GeoSceneLicense();

    QString license() const;
    QString shortLicense() const;
    Attribution attribution() const;

    void setLicense( const QString &license );
    void setShortLicense( const QString &license );
    void setAttribution(Attribution attr );

    const char *nodeType() const override;

 private:
    QString m_fullLicense;
    QString m_shortLicense;
    Attribution m_attribution;

};

} // namespace Marble
