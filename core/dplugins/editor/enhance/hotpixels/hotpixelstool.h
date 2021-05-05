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

#ifndef DIGIKAM_EDITOR_HOT_PIXELS_TOOL_H
#define DIGIKAM_EDITOR_HOT_PIXELS_TOOL_H

// Qt includes

#include <QList>
#include <QPolygon>

// Local includes

#include "editortool.h"
#include "hotpixelprops.h"

class QUrl;

using namespace Digikam;

namespace DigikamEditorHotPixelsToolPlugin
{

class HotPixelsTool : public Digikam::EditorToolThreaded
{
    Q_OBJECT

public:

    explicit HotPixelsTool(QObject* const parent);
    ~HotPixelsTool()            override;

    static void registerFilter();

private Q_SLOTS:

    void slotHotPixels(const QPolygon&);
    void slotResetSettings()    override;

private:

    void readSettings()         override;
    void writeSettings()        override;
    void preparePreview()       override;
    void prepareFinal()         override;
    void setPreviewImage()      override;
    void setFinalImage()        override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorHotPixelsToolPlugin

#endif // DIGIKAM_EDITOR_HOT_PIXELS_TOOL_H
