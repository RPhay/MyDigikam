/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-12-06
 * Description : Black and White conversion tool.
 *
 * Copyright (C) 2004-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_EDITOR_BW_SEPIA_TOOL_H
#define DIGIKAM_EDITOR_BW_SEPIA_TOOL_H

// Local includes

#include "editortool.h"

using namespace Digikam;

namespace DigikamEditorBWSepiaToolPlugin
{

class BWSepiaTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit BWSepiaTool(QObject* const parent);
    ~BWSepiaTool() override;

private Q_SLOTS:

    void slotInit() override;
    void slotSaveAsSettings() override;
    void slotLoadSettings() override;
    void slotResetSettings() override;

private:

    void readSettings() override;
    void writeSettings() override;
    void preparePreview() override;
    void prepareFinal() override;
    void setPreviewImage() override;
    void setFinalImage() override;
    void slotScaleChanged() override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorBWSepiaToolPlugin

#endif // DIGIKAM_EDITOR_BW_SEPIA_TOOL_H
