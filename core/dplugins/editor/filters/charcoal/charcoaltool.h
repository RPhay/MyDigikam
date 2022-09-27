/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-08-26
 * Description : a digikam image editor tool to
 *               simulate charcoal drawing.
 *
 * SPDX-FileCopyrightText: 2004-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_EDITOR_CHARCOAL_TOOL_H
#define DIGIKAM_EDITOR_CHARCOAL_TOOL_H

// Local includes

#include "editortool.h"

using namespace Digikam;

namespace DigikamEditorCharcoalToolPlugin
{

class CharcoalTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit CharcoalTool(QObject* const parent);
    ~CharcoalTool()             override;

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

} // namespace DigikamEditorCharcoalToolPlugin

#endif // DIGIKAM_EDITOR_CHARCOAL_TOOL_H
