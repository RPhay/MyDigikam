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
#include <QGroupBox>
#include <QHBoxLayout>
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
    QLabel*              maskPreview              = nullptr;
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
    d->radiusInput->setWhatsThis(i18n("Set this value to determine the matrix radius that "
                                      "determines how much to blur the background around the subject."));

    // --------------------------------------------------------

    QLabel* const label2 = new QLabel(i18n("Progressive Transition:"));
    d->transitionInput   = new DIntNumInput();
    d->transitionInput->setRange(0, 100, 1);
    d->transitionInput->setDefaultValue(0);
    d->transitionInput->setWhatsThis(i18n("Set this value to apply a variable blur near to far of the subject."
                                          "This allow to simulate a depth of field from a lens."));

    // --------------------------------------------------------

    QGroupBox* const maskGB = new QGroupBox(i18n("Subject Mask"));
    QGridLayout* const glay = new QGridLayout(maskGB);
    d->maskPreview          = new QLabel(maskGB);
    d->maskPreview->setWhatsThis(i18n("This view show the segmentation of the subject determined from the "
                                      "selection. The resulting green mask is superposed to the original image to "
                                      "see if the subject have been isolated properly by the segmentation process. "
                                      "The green area is the subject where the blur effect will not applied."));
    d->maskPreview->setFixedSize(256, 256);
    d->maskPreview->setAlignment(Qt::AlignCenter);
    glay->addWidget(d->maskPreview, 0, 0, Qt::AlignCenter);
    maskGB->setLayout(glay);

    // --------------------------------------------------------

    const int spacing       = d->gboxSettings->spacingHint();
    QGridLayout* const grid = new QGridLayout();
    grid->addWidget(label,              0, 0, 1, 2);
    grid->addWidget(d->radiusInput,     1, 0, 1, 2);
    grid->addWidget(label2,             2, 0, 1, 2);
    grid->addWidget(d->transitionInput, 3, 0, 1, 2);
    grid->addWidget(maskGB,             4, 0, 1, 2);
    grid->setRowStretch(5, 10);
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
    QRect orgSelection       = iface->selectionRect();

    // Compute the scale factor between original and preview sizes.

    float scaleFactor = static_cast<float>(iface->originalSize().width()) / iface->previewSize().width();

    QRect selection;
    selection.setTopLeft(QPoint(orgSelection.topLeft().x() / scaleFactor,
                                orgSelection.topLeft().y() / scaleFactor));
    selection.setBottomRight(QPoint(orgSelection.bottomRight().x() / scaleFactor,
                                    orgSelection.bottomRight().y() / scaleFactor));


    BackgroundBlurFilter* const filter = new BackgroundBlurFilter(&preview,
                                                                  selection,
                                                                  d->radiusInput->value() / scaleFactor,
                                                                  d->transitionInput->value(),
                                                                  this);
    connect(filter, SIGNAL(signalSegmentedMask(QImage)),
            this, SLOT(slotPreviewMask(QImage)));

    setFilter(filter);
}

void BackgroundBlurTool::slotPreviewMask(const QImage& mask)
{
    QPixmap pixmap = QPixmap::fromImage(mask);

    d->maskPreview->setPixmap(pixmap);
}

void BackgroundBlurTool::setPreviewImage()
{
    ImageIface* const iface = d->previewWidget->imageIface();
    DImg preview            = filter()->getTargetImage();

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
