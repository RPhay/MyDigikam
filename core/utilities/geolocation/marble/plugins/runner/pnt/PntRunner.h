// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLE_PNT_RUNNER_H
#define MARBLE_PNT_RUNNER_H

#include "ParsingRunner.h"

namespace Marble
{

class PntRunner : public ParsingRunner
{
    Q_OBJECT

public:

    explicit PntRunner(QObject *parent = nullptr);
    ~PntRunner() override;

    GeoDataDocument* parseFile( const QString &fileName, DocumentRole role, QString& error ) override;
};

}

#endif // MARBLE_PNT_RUNNER_H
