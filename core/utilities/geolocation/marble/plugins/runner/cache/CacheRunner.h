// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLE_CACHE_RUNNER_H
#define MARBLE_CACHE_RUNNER_H

#include "ParsingRunner.h"

namespace Marble
{

class CacheRunner : public ParsingRunner
{
    Q_OBJECT

public:

    explicit CacheRunner(QObject *parent = nullptr);
    ~CacheRunner() override;

    GeoDataDocument* parseFile( const QString &fileName, DocumentRole role, QString& error ) override;
};

}
#endif // MARBLE_CACHE_RUNNER_H
