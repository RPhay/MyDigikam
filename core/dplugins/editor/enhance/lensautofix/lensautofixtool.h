/* ============================================================
 *
 * Date        : 2008-02-10
 * Description : a tool to fix automatically camera lens aberrations
 *
 * SPDX-FileCopyrightText: 2008      by Adrian Schroeter <adrian at suse dot de>
 * SPDX-FileCopyrightText: 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_EDITOR_LENS_AUTO_FIX_TOOL_H
#define DIGIKAM_EDITOR_LENS_AUTO_FIX_TOOL_H

// Local includes

#include "editortool.h"

using namespace Digikam;

namespace DigikamEditorLensAutoFixToolPlugin
{

class LensAutoFixTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit LensAutoFixTool(QObject* const parent);
    ~LensAutoFixTool()          override;

private Q_SLOTS:

    void slotLensChanged();
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

} // namespace DigikamEditorLensAutoFixToolPlugin

#endif // DIGIKAM_EDITOR_LENS_AUTO_FIX_TOOL_H
