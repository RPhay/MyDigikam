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

#pragma once

#include <QDataStream>

#include "ParsingRunner.h"

namespace Marble
{

class GeoDataLineString;

class Pn2Runner : public ParsingRunner
{
    Q_OBJECT

public:

    explicit Pn2Runner(QObject *parent = nullptr);
    ~Pn2Runner() override;

    GeoDataDocument* parseFile( const QString &fileName, DocumentRole role, QString& error ) override;

private:

    static bool errorCheckLat( qint16 lat );
    static bool errorCheckLon( qint16 lon );
    static bool importPolygon( QDataStream &stream, GeoDataLineString* linestring, quint32 nrAbsoluteNodes );

    GeoDataDocument* parseForVersion1( const QString &fileName, DocumentRole role );
    GeoDataDocument* parseForVersion2( const QString &fileName, DocumentRole role );

private:

    QDataStream     m_stream;
    quint8          m_fileHeaderVersion;
    quint32         m_fileHeaderPolygons;
    bool            m_isMapColorField;       // Whether the file contains color indexes
};

} // namespace Marble
