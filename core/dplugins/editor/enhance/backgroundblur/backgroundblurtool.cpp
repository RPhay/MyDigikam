/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2025-08-24
 * Description : image editor plugin to blur the background of an image
 *
 * SPDX-FileCopyrightText: 2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "backgroundblurtool.h"

// Qt includes

#include <QGridLayout>
#include <QLabel>
#include <QIcon>

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <klocalizedstring.h>

// Local includes

#include "dnuminput.h"
#include "blurfilter.h"
#include "editortoolsettings.h"
#include "imageiface.h"
#include "imageguidewidget.h"
#include "backgroundblurfilter.h"

namespace DigikamEditorBackgroundBlurToolPlugin
{

class Q_DECL_HIDDEN BackgroundBlurTool::Private
{
public:

    Private() = default;

public:

    const QString configGroupName                 = QLatin1String("backgroundblur Tool");
    const QString configRadiusAdjustmentEntry     = QLatin1String("RadiusAdjustment");
    const QString configTransitionAdjustmentEntry = QLatin1String("TransitionAdjustment");

    DIntNumInput*        radiusInput              = nullptr;
    DIntNumInput*        transitionInput          = nullptr;
    ImageGuideWidget*    previewWidget            = nullptr;
    EditorToolSettings*  gboxSettings             = nullptr;
};

// --------------------------------------------------------

BackgroundBlurTool::BackgroundBlurTool(QObject* const parent)
    : EditorToolThreaded(parent),
      d                 (new Private)
{
    setObjectName(QLatin1String("backgroundblur"));
    setToolHelp(QLatin1String("backgroundblur.anchor"));

    d->gboxSettings     = new EditorToolSettings(nullptr);
    d->previewWidget    = new ImageGuideWidget(nullptr, false, ImageGuideWidget::PickColorMode);
    setToolView(d->previewWidget);
    setPreviewModeMask(PreviewToolBar::UnSplitPreviewModes);

    // --------------------------------------------------------

    QLabel* const label = new QLabel(i18n("Smoothness:"));
    d->radiusInput      = new DIntNumInput();
    d->radiusInput->setRange(0, 100, 1);
    d->radiusInput->setDefaultValue(0);
    d->radiusInput->setWhatsThis(i18n("A smoothness of 0 has no effect, "
                                      "1 and above determine the Gaussian blur matrix radius "
                                      "that determines how much to blur the background of the subject."));

    // --------------------------------------------------------

    QLabel* const label2 = new QLabel(i18n("Progressive Transition:"));
    d->transitionInput   = new DIntNumInput();
    d->transitionInput->setRange(0, 100, 1);
    d->transitionInput->setDefaultValue(0);
    d->transitionInput->setWhatsThis(i18n("A progressive blur transitions of 0 has no effect, "
                                          "1 and above add a variable blur near to far of the subject."
                                          "This allow to simulate a depth of field from a lens."));

    // --------------------------------------------------------

    const int spacing       = d->gboxSettings->spacingHint();
    QGridLayout* const grid = new QGridLayout();
    grid->addWidget(label,              0, 0, 1, 2);
    grid->addWidget(d->radiusInput,     1, 0, 1, 2);
    grid->addWidget(label2,             2, 0, 1, 2);
    grid->addWidget(d->transitionInput, 3, 0, 1, 2);
    grid->setRowStretch(4, 10);
    grid->setContentsMargins(spacing, spacing, spacing, spacing);
    grid->setSpacing(spacing);
    d->gboxSettings->plainPage()->setLayout(grid);

    // --------------------------------------------------------

    setToolSettings(d->gboxSettings);
    setToolView(d->previewWidget);

    // --------------------------------------------------------

    connect(d->radiusInput, SIGNAL(valueChanged(int)),
            this, SLOT(slotTimer()));

    connect(d->transitionInput, SIGNAL(valueChanged(int)),
            this, SLOT(slotTimer()));
}

BackgroundBlurTool::~BackgroundBlurTool()
{
    delete d;
}

void BackgroundBlurTool::readSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);
    d->radiusInput->setValue(group.readEntry(d->configRadiusAdjustmentEntry, d->radiusInput->defaultValue()));
    d->transitionInput->setValue(group.readEntry(d->configTransitionAdjustmentEntry, d->transitionInput->defaultValue()));
}

void BackgroundBlurTool::writeSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);
    group.writeEntry(d->configRadiusAdjustmentEntry, d->radiusInput->value());
    group.writeEntry(d->configTransitionAdjustmentEntry, d->transitionInput->value());
    config->sync();
}

void BackgroundBlurTool::slotResetSettings()
{
    d->radiusInput->blockSignals(true);
    d->transitionInput->blockSignals(true);
    d->radiusInput->slotReset();
    d->transitionInput->slotReset();
    d->radiusInput->blockSignals(false);
    d->transitionInput->blockSignals(false);
}

void BackgroundBlurTool::preparePreview()
{
    ImageIface* const iface  = d->previewWidget->imageIface();
    DImg preview             = iface->preview();

    qDebug() << "Original preview:" << preview.size() << preview.bitsDepth() << preview.hasAlpha();
    qDebug() << "Ratio original  :" << (double)iface->originalSize().width() / (double)iface->originalSize().height();
    qDebug() << "Ratio preview   :" << (double)preview.size().width()        / (double)preview.size().height();

    QRect orgSelection       = iface->selectionRect();
    double xratio            = (double)iface->originalSize().width()  / (double)iface->previewSize().width();
    double yratio            = (double)iface->originalSize().height() / (double)iface->previewSize().height();

    QRect selection;
    selection.setTopLeft(QPoint(orgSelection.topLeft().x() / xratio,         orgSelection.topLeft().y()     / yratio));
    selection.setBottomRight(QPoint(orgSelection.bottomRight().x() / xratio, orgSelection.bottomRight().y() / yratio));

    setFilter(new BackgroundBlurFilter(&preview,
                                       selection,
                                       d->radiusInput->value(),
                                       d->transitionInput->value(),
                                       this));
}

void BackgroundBlurTool::setPreviewImage()
{
    ImageIface* const iface = d->previewWidget->imageIface();
    DImg preview            = filter()->getTargetImage();

    qDebug() << "Target preview  :" << preview.size() << preview.bitsDepth() << preview.hasAlpha();

    iface->setPreview(preview);
    d->previewWidget->updatePreview();
}

void BackgroundBlurTool::prepareFinal()
{
    ImageIface iface;
    QRect selection = iface.selectionRect();
    setFilter(new BackgroundBlurFilter(iface.original(),
                                       selection,
                                       d->radiusInput->value(),
                                       d->transitionInput->value(),
                                       this));
}

void BackgroundBlurTool::setFinalImage()
{
    ImageIface iface;
    iface.setOriginal(i18n("Background Blur"), filter()->filterAction(), filter()->getTargetImage());
}

} // namespace DigikamEditorBackgroundBlurToolPlugin

#include "moc_backgroundblurtool.cpp"
