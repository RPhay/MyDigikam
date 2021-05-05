/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-07-20
 * Description : image histogram adjust levels.
 *
 * Copyright (C) 2004-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_EDITOR_ADJUST_LEVELS_TOOL_H
#define DIGIKAM_EDITOR_ADJUST_LEVELS_TOOL_H

// Local includes

#include "editortool.h"
#include "dcolor.h"

using namespace Digikam;

namespace DigikamEditorAdjustLevelsToolPlugin
{

class AdjustLevelsTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit AdjustLevelsTool(QObject* const parent);
    ~AdjustLevelsTool()                     override;

private Q_SLOTS:

    void slotSaveAsSettings()               override;
    void slotLoadSettings()                 override;
    void slotResetSettings()                override;
    void slotResetCurrentChannel();
    void slotAutoLevels();
    void slotChannelChanged()               override;
    void slotScaleChanged()                 override;
    void slotAdjustSliders();
    void slotGammaInputchanged(double val);
    void slotAdjustMinInputSpinBox(double val);
    void slotAdjustMaxInputSpinBox(double val);
    void slotAdjustMinOutputSpinBox(double val);
    void slotAdjustMaxOutputSpinBox(double val);
    void slotSpotColorChanged(const Digikam::DColor& color);
    void slotColorSelectedFromTarget(const Digikam::DColor& color);
    void slotPickerColorButtonActived(int);
    void slotShowInputHistogramGuide(double v);
    void slotShowOutputHistogramGuide(double v);

private:

    void readSettings()                     override;
    void writeSettings()                    override;
    void preparePreview()                   override;
    void prepareFinal()                     override;
    void abortPreview();
    void setPreviewImage()                  override;
    void setFinalImage()                    override;

    void adjustSliders(int minIn, double gamIn, int maxIn, int minOut, int maxOut);
    void adjustSlidersAndSpinboxes(int minIn, double gamIn, int maxIn, int minOut, int maxOut);
    bool eventFilter(QObject*, QEvent*)     override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorAdjustLevelsToolPlugin

#endif // DIGIKAM_EDITOR_ADJUST_LEVELS_TOOL_H
