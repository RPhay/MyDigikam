// SPDX-FileCopyrightText: 2007-2009 David Roberts <dvdr18@gmail.com>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <QColor>

//FIXME: This class shouldn't be exposed but is needed by the worldclock plasmoid
#include "digikam_export.h"

namespace Marble
{
class MarbleClock;
class SunLocatorPrivate;
class Planet;

class DIGIKAM_EXPORT SunLocator : public QObject
{
    Q_OBJECT

 public:
    SunLocator( const MarbleClock *clock, const Planet *planet );
    ~SunLocator() override;

    qreal shading(qreal lon, qreal a, qreal c) const;
    static void shadePixel(QRgb &pixcol, qreal shade);
    static void shadePixelComposite(QRgb &pixcol, const QRgb &dpixcol, qreal shade);

    void  setPlanet( const Planet *planet );

    qreal getLon() const;
    qreal getLat() const;

 public Q_SLOTS:
    void update();

 Q_SIGNALS:
    void positionChanged( qreal lon, qreal lat );

 private:

    SunLocatorPrivate* const d = nullptr;

    Q_DISABLE_COPY( SunLocator )
};

} // Namespace Marble
