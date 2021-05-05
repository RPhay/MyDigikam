/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-03-27
 * Description : a digiKam image tool for fixing dots produced by
 *               hot/stuck/dead pixels from a CCD.
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2005-2006 by Unai Garro <ugarro at users dot sourceforge dot net>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "hotpixelstool.h"

// Qt includes

#include <QGridLayout>
#include <QIcon>
#include <QApplication>
#include <QPointer>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "dcombobox.h"
#include "daboutdata.h"
#include "dimg.h"
#include "dimgfiltermanager.h"
#include "editortooliface.h"
#include "editortoolsettings.h"
#include "hotpixelfixer.h"
#include "hotpixelsettings.h"
#include "imageiface.h"
#include "imageregionwidget.h"

namespace DigikamEditorHotPixelsToolPlugin
{

class Q_DECL_HIDDEN HotPixelsTool::Private
{
public:

    explicit Private()
      : previewWidget(nullptr),
        gboxSettings (nullptr),
        hpSettings   (nullptr)
    {
    }

    ImageRegionWidget*   previewWidget;
    EditorToolSettings*  gboxSettings;
    HotPixelSettings*    hpSettings;
};

// --------------------------------------------------------

HotPixelsTool::HotPixelsTool(QObject* const parent)
    : EditorToolThreaded(parent),
      d                 (new Private)
{
    setObjectName(QLatin1String("hotpixels"));
    setToolName(i18n("Hot Pixels"));
    setToolIcon(QIcon::fromTheme(QLatin1String("hotpixels")));

    // -------------------------------------------------------------

    d->gboxSettings  = new EditorToolSettings(nullptr);
    d->gboxSettings->setButtons(EditorToolSettings::Default|
                                EditorToolSettings::Ok|
                                EditorToolSettings::Cancel|
                                EditorToolSettings::Try);

    d->hpSettings    = new HotPixelSettings(d->gboxSettings->plainPage());

    // -------------------------------------------------------------

    d->previewWidget = new ImageRegionWidget;

    setToolSettings(d->gboxSettings);
    setToolView(d->previewWidget);
    setPreviewModeMask(PreviewToolBar::AllPreviewModes);

    // -------------------------------------------------------------

    connect(d->hpSettings, SIGNAL(signalSettingsChanged()),
            this, SLOT(slotPreview()));

    connect(d->hpSettings, SIGNAL(signalHotPixels(QPolygon)),
            this, SLOT(slotHotPixels(QPolygon)));
}

HotPixelsTool::~HotPixelsTool()
{
    delete d;
}

void HotPixelsTool::registerFilter()
{
    DImgFilterManager::instance()->addGenerator(new BasicDImgFilterGenerator<HotPixelFixer>());
}

void HotPixelsTool::slotResetSettings()
{
    d->hpSettings->resetToDefault();
}

void HotPixelsTool::readSettings()
{
    KConfigGroup group = KSharedConfig::openConfig()->group(d->hpSettings->configGroupName());
    d->hpSettings->readSettings(group);
}

void HotPixelsTool::writeSettings()
{
    KConfigGroup group = KSharedConfig::openConfig()->group(d->hpSettings->configGroupName());
    d->hpSettings->writeSettings(group);
}

void HotPixelsTool::preparePreview()
{
    DImg image            = d->previewWidget->getOriginalRegionImage();
    HotPixelContainer prm = d->hpSettings->settings();

    QList<HotPixelProps> hotPixelsRegion;
    QRect area            = d->previewWidget->getOriginalImageRegionToRender();

    for (QList<HotPixelProps>::const_iterator it = prm.hotPixelsList.constBegin() ;
         it != prm.hotPixelsList.constEnd() ; ++it)
    {
        HotPixelProps hp = (*it);

        if (area.contains( hp.rect ))
        {
            hp.rect.moveTopLeft(QPoint( hp.rect.x()-area.x(), hp.rect.y()-area.y() ));
            hotPixelsRegion.append(hp);
        }
    }

    prm.hotPixelsList = hotPixelsRegion;

    setFilter(dynamic_cast<DImgThreadedFilter*>(new HotPixelFixer(&image, this, prm)));
}

void HotPixelsTool::prepareFinal()
{
    HotPixelContainer prm = d->hpSettings->settings();

    ImageIface iface;
    setFilter(dynamic_cast<DImgThreadedFilter*>(new HotPixelFixer(iface.original(), this, prm)));
}

void HotPixelsTool::setPreviewImage()
{
    d->previewWidget->setPreviewImage(filter()->getTargetImage());
}

void HotPixelsTool::setFinalImage()
{
    ImageIface iface;
    iface.setOriginal(i18n("Hot Pixels Correction"), filter()->filterAction(), filter()->getTargetImage());
}

void HotPixelsTool::slotHotPixels(const QPolygon& pointList)
{
    d->previewWidget->setHighLightPoints(pointList);
    slotPreview();
}

} // namespace DigikamEditorHotPixelsToolPlugin
