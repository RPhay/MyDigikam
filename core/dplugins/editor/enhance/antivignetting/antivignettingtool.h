/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-25
 * Description : a digiKam image tool to reduce
 *               vignetting on an image.
 *
 * SPDX-FileCopyrightText: 2005-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_EDITOR_ANTI_VIGNETTING_TOOL_H
#define DIGIKAM_EDITOR_ANTI_VIGNETTING_TOOL_H

// Local includes

#include "editortool.h"

using namespace Digikam;

namespace DigikamEditorAntivignettingToolPlugin
{

class AntiVignettingTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit AntiVignettingTool(QObject* const parent);
    ~AntiVignettingTool()       override;

private Q_SLOTS:

    void slotResetSettings()    override;

private:

    void writeSettings()        override;
    void readSettings()         override;
    void preparePreview()       override;
    void prepareFinal()         override;
    void setPreviewImage()      override;
    void setFinalImage()        override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorAntivignettingToolPlugin

#endif // DIGIKAM_EDITOR_ANTI_VIGNETTING_TOOL_H
