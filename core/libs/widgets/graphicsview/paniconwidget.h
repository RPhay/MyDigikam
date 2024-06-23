/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-08-22
 * Description : a generic widget to display a panel to choose
 *               a rectangular image area.
 *
 * SPDX-FileCopyrightText: 1997      by Tim D. Gilman <tdgilman at best dot org>
 * SPDX-FileCopyrightText: 1998-2001 by Mirko Boehm <mirko at kde dot org>
 * SPDX-FileCopyrightText: 2007      by John Layt <john at layt dot net>
 * SPDX-FileCopyrightText: 2004-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QWidget>
#include <QRect>
#include <QImage>
#include <QPixmap>
#include <QHideEvent>
#include <QMouseEvent>
#include <QTimerEvent>
#include <QPaintEvent>
#include <QFrame>

// Local includes

#include "digikam_export.h"
#include "dimg.h"

class QToolButton;

namespace Digikam
{

/**
 * Frame with popup menu behavior to host PanIconWidget.
 */
class DIGIKAM_EXPORT PanIconFrame : public QFrame
{
    Q_OBJECT

public:

    explicit PanIconFrame(QWidget* const parent = nullptr);
    ~PanIconFrame()                         override;

    /**
     * Set the main widget. You cannot set the main widget from the constructor,
     * since it must be a child of the frame itselfes.
     * Be careful: the size is set to the main widgets size. It is up to you to
     * set the main widgets correct size before setting it as the main
     * widget.
     */
    void setMainWidget(QWidget* const main);

    /**
     * Open the popup window at position pos.
     */
    void popup(const QPoint& pos);

    /**
     * Execute the popup window.
     */
    int exec(const QPoint& pos);

    /**
     * Execute the popup window.
     */
    int exec(int x, int y);

    /**
     * The resize event. Simply resizes the main widget to the whole
     * widgets client size.
     */
    void resizeEvent(QResizeEvent* resize)  override;

Q_SIGNALS:

    void leaveModality();

protected:

    /**
     * Catch key press events.
     */
    void keyPressEvent(QKeyEvent* e)        override;

public Q_SLOTS:

    /**
     * Close the popup window. This is called from the main widget, usually.
     * @p r is the result returned from exec().
     */
    void close(int r);

private:

    class Private;
    friend class Private;
    Private* const d = nullptr;

    Q_DISABLE_COPY(PanIconFrame)
};

// ---------------------------------------------------------------------------------

class DIGIKAM_EXPORT PanIconWidget : public QWidget
{
    Q_OBJECT

public:

    explicit PanIconWidget(QWidget* const parent = nullptr);
    ~PanIconWidget()                        override;

    static QToolButton* button();

    void setImage(int previewWidth, int previewHeight, const QImage& fullOriginalImage);
    void setImage(int previewWidth, int previewHeight, const DImg& fullOriginalImage);
    void setImage(const QImage& scaledPreviewImage, const QSize& fullImageSize);

    void  setRegionSelection(const QRect& regionSelection);
    QRect getRegionSelection() const;
    void  setCenterSelection();

    void  setCursorToLocalRegionSelectionCenter();
    void  setMouseFocus();

Q_SIGNALS:

    /**
     * Emitted when selection have been moved with mouse.
     * 'targetDone' boolean value is used for indicate if the mouse have been released.
     */
    void signalSelectionMoved(const QRect& rect, bool targetDone);

    void signalSelectionTakeFocus();

    void signalHidden();

public Q_SLOTS:

    void slotZoomFactorChanged(double);

protected:

    void showEvent(QShowEvent*)             override;
    void hideEvent(QHideEvent*)             override;
    void paintEvent(QPaintEvent*)           override;
    void mousePressEvent(QMouseEvent*)      override;
    void mouseReleaseEvent(QMouseEvent*)    override;
    void mouseMoveEvent(QMouseEvent*)       override;

    /**
     * Recalculate the target selection position and emit 'signalSelectionMoved'.
     */
    void regionSelectionMoved(bool targetDone);

protected Q_SLOTS:

    void slotFlickerTimer();

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
