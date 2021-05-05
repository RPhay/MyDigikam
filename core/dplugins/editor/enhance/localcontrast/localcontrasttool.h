/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-08-09
 * Description : a tool to enhance image with local contrasts (as human eye does).
 *
 * Copyright (C) 2009      by Julien Pontabry <julien dot pontabry at gmail dot com>
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_EDITOR_LOCAL_CONTRAST_TOOL_H
#define DIGIKAM_EDITOR_LOCAL_CONTRAST_TOOL_H

// Local includes

#include "editortool.h"

using namespace Digikam;

namespace DigikamEditorLocalContrastToolPlugin
{

class LocalContrastTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit LocalContrastTool(QObject* const parent);
    ~LocalContrastTool()        override;

private:

    void readSettings()         override;
    void writeSettings()        override;
    void preparePreview()       override;
    void prepareFinal()         override;
    void setPreviewImage()      override;
    void setFinalImage()        override;

private Q_SLOTS:

    void slotSaveAsSettings()   override;
    void slotLoadSettings()     override;
    void slotResetSettings()    override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorLocalContrastToolPlugin

#endif // DIGIKAM_EDITOR_LOCAL_CONTRAST_TOOL_H
