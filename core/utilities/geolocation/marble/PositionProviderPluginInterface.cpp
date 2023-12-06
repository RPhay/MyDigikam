// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
//

#include "PositionProviderPluginInterface.h"

#include <klocalizedstring.h>

namespace Marble
{

PositionProviderPluginInterface::~PositionProviderPluginInterface()
{
    // nothing to do
}

QString PositionProviderPluginInterface::error() const
{
    // Subclasses are expected to override this, but we provide
    // a default implementation

    if ( status() == PositionProviderStatusError )
    {
        return i18n( "Unknown error" );
    }

    return QString();
}

} // namespace Marble
