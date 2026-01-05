/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-03-05
 * Description : digiKam Survey tool - The view.
 *
 * SPDX-FileCopyrightText: 2007-2026 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QFrame>
#include <QString>

// Local includes

#include "iteminfo.h"

namespace Digikam
{

class PreviewSettings;

class SurveyView : public QWidget
{
    Q_OBJECT

public:

    explicit SurveyView(QWidget* const parent = nullptr);
    ~SurveyView()             override;

    void   setItemInfo(const ItemInfo& info = ItemInfo());

    ItemInfo itemInfo() const;

    void   setPreviewSettings(const PreviewSettings& settings);

    void   toggleFullScreen(bool set);

    double zoomMax()    const;
    double zoomMin()    const;

    bool   maxZoom()    const;
    bool   minZoom()    const;

    void   reload();

Q_SIGNALS:

    void signalPreviewLoaded(bool);

    void signalZoomFactorChanged(double);

    void signalDroppedItems(const ItemInfoList&);

    void signalPopupTagsView();

    void signalSlideShowCurrent();

    void signalDeleteItem(const ItemInfo&);
    void signalEditItem(const ItemInfo&);

    void signalPreviewStartedLoading();
    void signalPreviewLoadingProgress(float progress);
    void signalPreviewLoadingComplete();

public Q_SLOTS:

    void slotDecreaseZoom();
    void slotIncreaseZoom();
    void slotZoomSliderChanged(int);
    void setZoomFactor(double z);
    void slotFitToWindow();
    void slotZoomTo100();

private Q_SLOTS:

    void slotZoomFactorChanged(double);
    void slotPreviewLoaded(bool);
    void slotDeleteItem();

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
