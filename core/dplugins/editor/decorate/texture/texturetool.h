/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-03-10
 * Description : a tool to apply texture over an image
 *
 * SPDX-FileCopyrightText: 2005-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_EDITOR_TEXTURE_TOOL_H
#define DIGIKAM_EDITOR_TEXTURE_TOOL_H

// Qt includes

#include <QString>

// Local includes

#include "editortool.h"

using namespace Digikam;

namespace DigikamEditorTextureToolPlugin
{

class TextureTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit TextureTool(QObject* const parent);
    ~TextureTool()              override;

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

} // namespace DigikamEditorTextureToolPlugin

#endif // DIGIKAM_EDITOR_TEXTURE_TOOL_H
