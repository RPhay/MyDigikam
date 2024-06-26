/*
    SPDX-FileCopyrightText: 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
    SPDX-FileCopyrightText: 2010-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#pragma once

#include <QtGlobal>
#include "digikam_export.h"

class QRect;

namespace Marble
{

class DIGIKAM_EXPORT TileCoordsPyramid
{
 public:
    TileCoordsPyramid( int const topLevel, int const bottomLevel );
    TileCoordsPyramid( TileCoordsPyramid const & other );
    TileCoordsPyramid();
    TileCoordsPyramid & operator=( TileCoordsPyramid const & rhs );
    ~TileCoordsPyramid();

    int topLevel() const;
    int bottomLevel() const;
    void setBottomLevelCoords( QRect const & coords );
    QRect coords( int const level ) const;

    void setValidTileLevels( const QVector<int>& validLevels );
    QVector<int> validTileLevels();

    /**
      * @brief returns the number of tiles covered by one pyramid
      **/
    qint64 tilesCount() const;

 private:
    void swap( TileCoordsPyramid & other );
    class Private;
    Private * d; // not const, needs to be swapable
};

} // namespace Marble
