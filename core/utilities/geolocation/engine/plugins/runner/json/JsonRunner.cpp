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

#include "JsonRunner.h"

// Qt includes

#include <QFile>

// Local includes

#include "JsonParser.h"
#include "GeoDataDocument.h"
#include "digikam_debug.h"

namespace Marble
{

JsonRunner::JsonRunner(QObject* parent) :
    ParsingRunner(parent)
{
}

JsonRunner::~JsonRunner()
{
}

GeoDataDocument* JsonRunner::parseFile(const QString& fileName, DocumentRole role, QString& error)
{
    // Check that the file exists
    QFile file(fileName);

    if (! file.exists())
    {
        error = QStringLiteral("File %1 does not exist").arg(fileName);
        qCDebug(DIGIKAM_MARBLE_LOG) << error;
        return nullptr;
    }

    // Open file in the correct mode
    file.open(QIODevice::ReadOnly);

    // Create parser
    JsonParser parser;

    // Start parsing
    if (! parser.read(&file))
    {
        error = QStringLiteral("Could not parse GeoJSON from %1").arg(fileName);
        qCDebug(DIGIKAM_MARBLE_LOG) << error;
        return nullptr;
    }

    GeoDataDocument* document = parser.releaseDocument();
    file.close();

    document->setDocumentRole(role);
    document->setFileName(fileName);

    return document;
}

} // namespace Marble

#include "moc_JsonRunner.cpp"
