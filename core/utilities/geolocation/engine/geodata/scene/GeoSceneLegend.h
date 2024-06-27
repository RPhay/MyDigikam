/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QVector>
#include "GeoDocument.h"

#include "digikam_export.h"

namespace Marble
{

class GeoSceneSection;

class GeoSceneLegendPrivate;

/**
 * @short Legend of a GeoScene document.
 */

class DIGIKAM_EXPORT GeoSceneLegend : public GeoNode
{
public:

    GeoSceneLegend();
    ~GeoSceneLegend() override;

    const char* nodeType() const override;

    /**
     * @brief  Add a section to the legend
     * @param  section  the new section
     */
    void addSection( const GeoSceneSection* );
    QVector<const GeoSceneSection*> sections() const;

private:

    Q_DISABLE_COPY( GeoSceneLegend )

    GeoSceneLegendPrivate* const d = nullptr;
};

} // namespace Marble
