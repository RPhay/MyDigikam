/*
    SPDX-FileCopyrightText: 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
    SPDX-FileCopyrightText: 2010-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#pragma once

#include <QWidget>

#include "digikam_export.h"

namespace Marble
{

class DIGIKAM_EXPORT TileLevelRangeWidget: public QWidget
{
    Q_OBJECT

public:

    explicit TileLevelRangeWidget( QWidget * const parent = nullptr, Qt::WindowFlags const f = Qt::WindowFlags() );
    ~TileLevelRangeWidget() override;

    QSize sizeHint() const override;

    void setAllowedLevelRange( int const minimumLevel, int const maximumLevel );
    void setDefaultLevel( int const );

    int topLevel() const;
    int bottomLevel() const;

Q_SIGNALS:

    void topLevelChanged( int );
    void bottomLevelChanged( int );

private Q_SLOTS:

    void setMaximumTopLevel( int const );
    void setMinimumBottomLevel( int const );

private:

    Q_DISABLE_COPY( TileLevelRangeWidget )
    class Private;
    Private* const d = nullptr;
};

} // namespace Marble
