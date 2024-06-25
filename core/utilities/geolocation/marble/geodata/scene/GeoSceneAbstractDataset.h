/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QString>

#include "digikam_export.h"

#include "GeoDocument.h"

namespace Marble
{

/**
 * @short Contents used inside a layer.
 */
class DIGIKAM_EXPORT GeoSceneAbstractDataset : public GeoNode
{
 public:
    ~GeoSceneAbstractDataset() override {};

    QString name() const;

    QString fileFormat() const;
    void setFileFormat( const QString& fileFormat );

    int expire() const;
    void setExpire( int expire );

 protected:
    explicit GeoSceneAbstractDataset( const QString& name );

 private:
    QString m_name;
    QString m_fileFormat;
    int     m_expire;
};

} // namespace Marble
