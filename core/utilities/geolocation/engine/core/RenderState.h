// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Dennis Nienhüser <nienhueser@kde.org>
//

#pragma once

#include "digikam_export.h"
#include "MarbleGlobal.h"

#include <QDebug>

namespace Marble
{

class DIGIKAM_EXPORT RenderState
{
public:

    explicit RenderState( const QString &name = QString(), RenderStatus status=Complete );
    RenderState( const RenderState &other );
    RenderState& operator=( const RenderState &other );
    ~RenderState();

    RenderStatus status() const;
    QString name() const;
    int children() const;
    RenderState childAt( int index ) const;
    void addChild( const RenderState& child );
    operator QString() const;

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Marble
