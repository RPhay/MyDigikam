/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-03-10
 * Description : a tool to apply texture over an image
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "texturetool.h"

// Qt includes

#include <QGridLayout>
#include <QImage>
#include <QLabel>
#include <QIcon>

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <klocalizedstring.h>

// Local includes

#include "dimg.h"
#include "editortoolsettings.h"
#include "imageiface.h"
#include "imageregionwidget.h"
#include "texturesettings.h"
#include "texturefilter.h"

namespace DigikamEditorTextureToolPlugin
{

class Q_DECL_HIDDEN TextureTool::Private
{
public:

    explicit Private()
      : configGroupName(QLatin1String("texture Tool")),
        settingsView   (nullptr),
        previewWidget  (nullptr),
        gboxSettings   (nullptr)
    {
    }

    const QString       configGroupName;

    TextureSettings*    settingsView;
    ImageRegionWidget*  previewWidget;
    EditorToolSettings* gboxSettings;
};

// --------------------------------------------------------

TextureTool::TextureTool(QObject* const parent)
    : EditorToolThreaded(parent),
      d                 (new Private)
{
    setObjectName(QLatin1String("texture"));

    // -------------------------------------------------------------

    d->gboxSettings      = new EditorToolSettings(nullptr);
    d->previewWidget     = new ImageRegionWidget;
    d->settingsView      = new TextureSettings(d->gboxSettings->plainPage());

    // -------------------------------------------------------------

    setToolSettings(d->gboxSettings);
    setToolView(d->previewWidget);
    setPreviewModeMask(PreviewToolBar::AllPreviewModes);

    // -------------------------------------------------------------

    connect(d->settingsView, SIGNAL(signalSettingsChanged()),
            this, SLOT(slotTimer()));
}

TextureTool::~TextureTool()
{
    delete d;
}

void TextureTool::readSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);

    d->settingsView->readSettings(group);
}

void TextureTool::writeSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);

    d->settingsView->writeSettings(group);

    group.sync();
}

void TextureTool::slotResetSettings()
{
    d->settingsView->resetToDefault();
}

void TextureTool::preparePreview()
{
    DImg image                = d->previewWidget->getOriginalRegionImage();
    TextureContainer settings = d->settingsView->settings();
    setFilter(new TextureFilter(&image, this, settings));
}

void TextureTool::prepareFinal()
{
    ImageIface iface;
    TextureContainer settings = d->settingsView->settings();
    setFilter(new TextureFilter(iface.original(), this, settings));
}

void TextureTool::setPreviewImage()
{
    d->previewWidget->setPreviewImage(filter()->getTargetImage());
}

void TextureTool::setFinalImage()
{
    ImageIface iface;
    iface.setOriginal(i18n("Texture"), filter()->filterAction(), filter()->getTargetImage());
}

} // namespace DigikamEditorTextureToolPlugin
