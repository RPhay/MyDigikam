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

#include "surveyview.h"

// Qt includes

#include <QGridLayout>
#include <QLabel>
#include <QApplication>

// Local includes

#include "digikam_debug.h"
#include "dimg.h"
#include "dzoombar.h"
#include "thumbnailsize.h"
#include "lighttablepreview.h"
#include "singlephotopreviewlayout.h"
#include "dimgpreviewitem.h"

namespace Digikam
{

class Q_DECL_HIDDEN SurveyView::Private
{
public:

    Private() = default;

public:

    QGridLayout*       grid    = nullptr;

    LightTablePreview* preview = nullptr;
};

SurveyView::SurveyView(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    setAttribute(Qt::WA_DeleteOnClose);

    d->grid                 = new QGridLayout();
    setLayout(d->grid);

    d->preview              = new LightTablePreview(this);

    d->grid->addWidget(d->preview, 0, 0, 1, 1);
    d->grid->setColumnStretch(0, 10);
    d->grid->setRowStretch(0, 10);
    d->grid->setContentsMargins(0, 0, 0, 0);
    d->grid->setSpacing(0);

    connect(d->preview, SIGNAL(signalPopupTagsView()),
            this, SIGNAL(signalPopupTagsView()));

    connect(d->preview->layout(), SIGNAL(zoomFactorChanged(double)),
            this, SLOT(slotZoomFactorChanged(double)));

    connect(d->preview, SIGNAL(signalDroppedItems(ItemInfoList)),
            this, SIGNAL(signalDroppedItems(ItemInfoList)));

    connect(d->preview, SIGNAL(signalSlideShowCurrent()),
            this, SIGNAL(signalSlideShowCurrent()));

    connect(d->preview, SIGNAL(signalPreviewLoaded(bool)),
            this, SLOT(slotPreviewLoaded(bool)));

    connect(d->preview, SIGNAL(leftButtonClicked()),
            this, SIGNAL(signalPanelLeftButtonClicked()));

    connect(d->preview, SIGNAL(signalDeleteItem()),
            this, SLOT(slotDeleteItem()));

    connect(d->preview, SIGNAL(signalStartedLoading()),
            this, SIGNAL(signalPreviewStartedLoading()));

    connect(d->preview, SIGNAL(signalLoadingProgress(float)),
            this, SIGNAL(signalPreviewLoadingProgress(float)));

    connect(d->preview, SIGNAL(signalLoadingComplete()),
            this, SIGNAL(signalPreviewLoadingComplete()));
}

SurveyView::~SurveyView()
{
    delete d;
}

void SurveyView::setPreviewSettings(const PreviewSettings& settings)
{
    d->preview->previewItem()->setPreviewSettings(settings);
}

void SurveyView::slotDecreaseZoom()
{
    d->preview->layout()->decreaseZoom();
}

void SurveyView::slotIncreaseZoom()
{
    d->preview->layout()->increaseZoom();
}

void SurveyView::setZoomFactor(double z)
{
    d->preview->layout()->setZoomFactor(z);
}

void SurveyView::slotZoomTo100()
{
    d->preview->layout()->toggleFitToWindowOr100();
}

void SurveyView::slotFitToWindow()
{
    d->preview->layout()->fitToWindow();
}

double SurveyView::zoomMax() const
{
    return d->preview->layout()->maxZoomFactor();
}

double SurveyView::zoomMin() const
{
    return d->preview->layout()->minZoomFactor();
}

bool SurveyView::maxZoom() const
{
    return d->preview->layout()->atMaxZoom();
}

bool SurveyView::minZoom() const
{
    return d->preview->layout()->atMinZoom();
}

void SurveyView::slotZoomSliderChanged(int size)
{
    double zmin = d->preview->layout()->minZoomFactor();
    double zmax = d->preview->layout()->maxZoomFactor();
    double z    = DZoomBar::zoomFromSize(size, zmin, zmax);
    d->preview->layout()->setZoomFactorSnapped(z);
}

void SurveyView::reload()
{
    d->preview->previewItem()->reload();
}

void SurveyView::slotZoomFactorChanged(double zoom)
{
    Q_EMIT signalZoomFactorChanged(zoom);
}

ItemInfo SurveyView::itemInfo() const
{
    return d->preview->getItemInfo();
}

void SurveyView::setItemInfo(const ItemInfo& info)
{
    d->preview->setItemInfo(info);

    if (info.isNull())
    {
        d->preview->showDragAndDropMessage();
    }
}

void SurveyView::slotPreviewLoaded(bool success)
{
    Q_EMIT signalPreviewLoaded(success);
}

void SurveyView::slotDeleteItem()
{
    Q_EMIT signalDeleteItem(d->preview->getItemInfo());
}

void SurveyView::toggleFullScreen(bool set)
{
    d->preview->toggleFullScreen(set);
}

} // namespace Digikam

#include "moc_surveyview.cpp"
