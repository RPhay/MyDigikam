/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-02-14
 * Description : a digiKam image tool for to apply a color
 *               effect to an image.
 *
 * SPDX-FileCopyrightText: 2004-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_EDITOR_COLOR_FX_TOOL_H
#define DIGIKAM_EDITOR_COLOR_FX_TOOL_H

// Local includes

#include "editortool.h"
#include "dimg.h"

using namespace Digikam;

namespace DigikamEditorColorFxToolPlugin
{

class ColorFxTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit ColorFxTool(QObject* const parent);
    ~ColorFxTool()              override;

private:

    void readSettings()         override;
    void writeSettings()        override;
    void preparePreview()       override;
    void prepareFinal()         override;
    void setPreviewImage()      override;
    void setFinalImage()        override;

private Q_SLOTS:

    void slotInit()             override;
    void slotResetSettings()    override;
    void slotColorSelectedFromTarget(const Digikam::DColor& color);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorColorFxToolPlugin

#endif // DIGIKAM_EDITOR_COLOR_FX_TOOL_H
