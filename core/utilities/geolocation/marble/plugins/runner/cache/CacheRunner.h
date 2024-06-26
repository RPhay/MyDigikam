// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#pragma once

#include "ParsingRunner.h"

namespace Marble
{

class CacheRunner : public ParsingRunner
{
    Q_OBJECT

public:

    explicit CacheRunner(QObject* const parent = nullptr);
    ~CacheRunner() override;

    GeoDataDocument* parseFile( const QString &fileName, DocumentRole role, QString& error ) override;
};

} // namespace Marble
