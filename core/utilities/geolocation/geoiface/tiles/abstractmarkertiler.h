/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-01
 * Description : An abstract base class for tiling of markers
 *
 * SPDX-FileCopyrightText: 2009-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2011 by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ABSTRACT_MARKER_TILER_H
#define DIGIKAM_ABSTRACT_MARKER_TILER_H

// Qt includes

#include <QBitArray>
#include <QObject>
#include <QPoint>

// Local includes

#include "tileindex.h"
#include "geoifacetypes.h"
#include "digikam_export.h"
#include "geogroupstate.h"

namespace Digikam
{

class DIGIKAM_EXPORT AbstractMarkerTiler : public QObject
{
    Q_OBJECT

public:

    enum TilerFlag
    {
        FlagNull    = 0,
        FlagMovable = 1
    };

    Q_DECLARE_FLAGS(TilerFlags, TilerFlag)

public:

    class ClickInfo
    {
    public:

        ClickInfo() = default;

        TileIndex::List tileIndicesList;
        QVariant        representativeIndex;
        GeoGroupState   groupSelectionState;
        GeoMouseModes   currentMouseMode;
    };

public:

    class Tile
    {
    public:

        Tile() = default;
        ~Tile();

    public:

        Tile* getChild(const int linearIndex);

        Tile* addChild(const int linearIndex, Tile* tilePointer);

        /**
         * @brief Sets the pointer to a child tile to zero and deletes the child.
         */
        void deleteChild(Tile* const childTile, const int knownLinearIndex = -1);

        bool childrenEmpty() const;

        /**
         * @brief returns the next non empty child index or -1.
         */
        int nextNonEmptyIndex(int linearIndex) const;

    private:

        // Disable
        Tile(const Tile&)            = delete;
        Tile& operator=(const Tile&) = delete;

    private:

        static int maxChildCount();

        void prepareForChildren();

    private:

        QVector<Tile*> children;
        QVector<int>   nonEmptyIndices;
    };

public:

    class NonEmptyIterator
    {
    public:

        NonEmptyIterator(AbstractMarkerTiler* const model, const int level);
        NonEmptyIterator(AbstractMarkerTiler* const model, const int level, const TileIndex& startIndex, const TileIndex& endIndex);
        NonEmptyIterator(AbstractMarkerTiler* const model, const int level, const GeoCoordinates::PairList& normalizedMapBounds);
        ~NonEmptyIterator();

        bool                 atEnd()        const;
        TileIndex            nextIndex();
        TileIndex            currentIndex() const;
        AbstractMarkerTiler* model()        const;

    private:

        bool initializeNextBounds();

    private:

        // Disable
        NonEmptyIterator(const NonEmptyIterator&)            = delete;
        NonEmptyIterator& operator=(const NonEmptyIterator&) = delete;

    private:

        class Private;
        Private* const d = nullptr;
    };

public:

    explicit AbstractMarkerTiler(QObject* const parent = nullptr);
    ~AbstractMarkerTiler() override;

    /// these have to be implemented
    virtual TilerFlags tilerFlags() const;
    virtual Tile* tileNew()                                                                                 = 0;
    virtual void prepareTiles(const GeoCoordinates& upperLeft, const GeoCoordinates& lowerRight, int level) = 0;
    virtual void regenerateTiles()                                                                          = 0;
    virtual Tile* getTile(const TileIndex& tileIndex, const bool stopIfEmpty)                               = 0;
    virtual int getTileMarkerCount(const TileIndex& tileIndex)                                              = 0;
    virtual int getTileSelectedCount(const TileIndex& tileIndex)                                            = 0;

    /// these should be implemented for thumbnail handling
    virtual QVariant getTileRepresentativeMarker(const TileIndex& tileIndex, const int sortKey)             = 0;
    virtual QVariant bestRepresentativeIndexFromList(const QList<QVariant>& indices, const int sortKey)     = 0;
    virtual QPixmap pixmapFromRepresentativeIndex(const QVariant& index, const QSize& size)                 = 0;
    virtual bool indicesEqual(const QVariant& a, const QVariant& b) const                                   = 0;
    virtual GeoGroupState getTileGroupState(const TileIndex& tileIndex)                                     = 0;
    virtual GeoGroupState getGlobalGroupState()                                                             = 0;

    /// these can be implemented if you want to react to actions in geolocation interface
    virtual void onIndicesClicked(const ClickInfo& clickInfo);
    virtual void onIndicesMoved(const TileIndex::List& tileIndicesList, const GeoCoordinates& targetCoordinates,
                                const QPersistentModelIndex& targetSnapIndex);

    virtual void setActive(const bool state)                                                                = 0;
    Tile* rootTile();
    bool indicesEqual(const QIntList& a, const QIntList& b, const int upToLevel) const;
    bool isDirty() const;
    void setDirty(const bool state = true);
    void resetRootTile();

Q_SIGNALS:

    void signalTilesOrSelectionChanged();
    void signalThumbnailAvailableForIndex(const QVariant& index, const QPixmap& pixmap);

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam

Q_DECLARE_OPERATORS_FOR_FLAGS(Digikam::AbstractMarkerTiler::TilerFlags)

#endif // DIGIKAM_ABSTRACT_MARKER_TILER_H
