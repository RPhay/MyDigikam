/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-04-30
 * Description : Layout for an item on image preview
 *
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QFlags>
#include <QObject>
#include <QPointF>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class GraphicsDImgItem;
class GraphicsDImgView;

class DIGIKAM_EXPORT SinglePhotoPreviewLayout : public QObject
{
    Q_OBJECT

public:

    enum SetZoomFlag
    {
        JustSetFactor  = 0,
        CenterView     = 1 << 0,
        SnapZoomFactor = 1 << 1
    };
    Q_DECLARE_FLAGS(SetZoomFlags, SetZoomFlag)

public:

    explicit SinglePhotoPreviewLayout(QObject* const parent);
    ~SinglePhotoPreviewLayout() override;

    /**
     * Set the graphics view, and associated scene, to operate on.
     */
    void setGraphicsView(GraphicsDImgView* const view);

    /**
     * Set to true to scale small images to fit to window.
     */
    void setScaleFitToWindow(bool value);

    /**
     * Set the item to layout. For a SinglePhoto layout, typically,
     * you can add only one item.
     */
    void addItem(GraphicsDImgItem* const item);

    bool   isFitToWindow()  const;
    double zoomFactor()     const;
    double realZoomFactor() const;

    /**
     * The zoom range for incrementing and decrementing.
     */
    double maxZoomFactor()  const;
    double minZoomFactor()  const;
    void   setMaxZoomFactor(double z);
    void   setMinZoomFactor(double z);
    bool   atMaxZoom()      const;
    bool   atMinZoom()      const;

Q_SIGNALS:

    void fitToWindowToggled(bool fitToWindow);
    void zoomFactorChanged(double);

public Q_SLOTS:

    void increaseZoom(const QPoint& viewportAnchor = QPoint());
    void decreaseZoom(const QPoint& viewportAnchor = QPoint());
    void setZoomFactor(double z, const QPoint& viewportAnchor = QPoint(), SetZoomFlags flags = JustSetFactor);
    void setZoomFactor(double z, SetZoomFlags flags);
    void setZoomFactorSnapped(double z);
    void fitToWindow();

    /// Toggle between fitToWindow and previous zoom factor
    void toggleFitToWindow();

    /// Toggle between fitToWindow and zoom factor 1
    void toggleFitToWindowOr100();

    /// Update settings when size of image or view changed
    void updateZoomAndSize();

protected:

    void updateLayout();

private:

    SinglePhotoPreviewLayout(); // Disable default constructor.

    class Private;
    Private* const d = nullptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(SinglePhotoPreviewLayout::SetZoomFlags)

} // namespace Digikam
