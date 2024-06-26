// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
//

#pragma once

#include <QChar>
#include <QRegularExpression>
#include <QString>

namespace Marble
{

namespace GeoString
{
    static const QRegularExpression combiningDiacriticalMarks(QString::fromUtf8("[\\x0300-\\x036F]+"));

    QString deaccent( const QString& accentString )
    {
        QString    result;

        result = accentString.normalized( QString::NormalizationForm_D ).remove( combiningDiacriticalMarks );
        result.replace(QChar(0x00F8), QLatin1Char('o'));
        result.replace(QChar(0x0142), QLatin1Char('l'));
        return result;
    }
}

} // namespace Marble
