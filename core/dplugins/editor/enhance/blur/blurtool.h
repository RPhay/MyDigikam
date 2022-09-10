/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-07-09
 * Description : a tool to blur an image
 *
 * SPDX-FileCopyrightText: 2004-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009      by Andi Clemens <andi dot clemens at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_EDITOR_BLUR_TOOL_H
#define DIGIKAM_EDITOR_BLUR_TOOL_H

// Local includes

#include "editortool.h"

using namespace Digikam;

namespace DigikamEditorBlurToolPlugin
{

class BlurTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit BlurTool(QObject* const parent);
    ~BlurTool()                 override;

private Q_SLOTS:

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

} // namespace DigikamEditorBlurToolPlugin

#endif // DIGIKAM_EDITOR_BLUR_TOOL_H
