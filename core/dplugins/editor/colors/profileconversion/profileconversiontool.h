/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-09-19
 * Description : a tool for color space conversion
 *
 * Copyright (C) 2009-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DIGIKAM_EDITOR_PROFILE_CONVERSION_TOOL_H
#define DIGIKAM_EDITOR_PROFILE_CONVERSION_TOOL_H

// Local includes

#include "editortool.h"
#include "iccprofile.h"

using namespace Digikam;

namespace DigikamEditorProfileConversionToolPlugin
{

class ProfileConversionTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit ProfileConversionTool(QObject* const parent);
    ~ProfileConversionTool()    override;

    static QStringList favoriteProfiles();
    static void fastConversion(const IccProfile& profile);

private Q_SLOTS:

    void slotResetSettings()    override;
    void slotCurrentProfInfo();
    void slotProfileChanged();

private:

    void readSettings()         override;
    void writeSettings()        override;
    void preparePreview()       override;
    void prepareFinal()         override;
    void setPreviewImage()      override;
    void setFinalImage()        override;

    void updateTransform();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorProfileConversionToolPlugin

#endif // DIGIKAM_EDITOR_PROFILE_CONVERSION_TOOL_H
