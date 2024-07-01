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

#include <QObject>

#include "GeoDataDocument.h"

#include "digikam_export.h"

namespace Marble
{

class PluginManager;

class DIGIKAM_EXPORT ParsingRunnerManager : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor.
     * @param pluginManager The plugin manager that gives access to RunnerPlugins
     * @param parent Optional parent object
     */
    explicit ParsingRunnerManager( const PluginManager *pluginManager, QObject *parent = nullptr );

    ~ParsingRunnerManager() override;

    /**
     * Parse the file using the runners for various formats
     * @see parseFile is asynchronous with results returned using the
     * @see parsingFinished signal.
     * @see openFile is blocking.
     * @see parsingFinished signal indicates all runners are finished.
     */
    void parseFile( const QString &fileName, DocumentRole role = UserDocument );
    GeoDataDocument *openFile( const QString &fileName, DocumentRole role = UserDocument, int timeout = 30000 );

Q_SIGNALS:

    /**
     * The file was parsed and potential error message
     */
    void parsingFinished( GeoDataDocument *document, const QString &error = QString() );

    /**
     * Emitted whenever all runners are finished for the query
     */
    void parsingFinished();

private:

    Q_PRIVATE_SLOT( d, void cleanupParsingTask() )
    Q_PRIVATE_SLOT( d, void addParsingResult( GeoDataDocument *document, const QString &error ) )

private:

    class Private;
    friend class Private;
    Private* const d = nullptr;
};

} // namespace Marble
