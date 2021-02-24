/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-06-15
 * Description : a tool to replace part of the image using another
 *
 * Copyright (C) 2004-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2017      by Shaza Ismail Kaoud <shaza dot ismail dot k at gmail dot com>
 * Copyright (C) 2019      by Ahmed Fathi <ahmed dot fathi dot abdelmageed at gmail dot com>
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

#ifndef DIGIKAM_EDITOR_HEALING_CLONE_TOOL_H
#define DIGIKAM_EDITOR_HEALING_CLONE_TOOL_H

// C++ includes

#include <vector>

// Local includes

#include "editortool.h"
#include "dimg.h"

using namespace Digikam;

namespace DigikamEditorHealingCloneToolPlugin
{

class HealingCloneTool : public EditorTool
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * @param parent the parent for qt parent child mechanism
     */
    explicit HealingCloneTool(QObject* const parent);

    /**
     * Destructor.
     */
    ~HealingCloneTool()      override;

private Q_SLOTS:

    void slotResetSettings() override;
    void slotResized();

public Q_SLOTS:

    /**
     * @brief slotReplace called from the brush widget to start clone/heal
     * @param srcPoint the center point of brush source
     * @param dstPoint the center point of brush destination
     */
    void slotReplace(const QPoint& srcPoint, const QPoint& dstPoint);
    void slotRadiusChanged(int r);

    void slotLasso(const QPoint& dst);
    void slotResetLassoPoints();
    void slotContinuePolygon();
    void slotIncreaseBrushRadius();
    void slotDecreaseBrushRadius();
    void slotPushToUndoStack();
    void slotUndoClone();
    void slotRedoClone();

private:

    void readSettings()      override;
    void writeSettings()     override;
    void finalRendering()    override;

    /**
     * @brief clone the method responsible for the clone/heal of preview image
     * @param img the preview image is passed to this method by reference
     * @param srcPoint center of brush source
     * @param dstPoint center of brush destination
     * @param radius the radius of cloning brush
     */
    void clone(DImg* const img, const QPoint& srcPoint, const QPoint& dstPoint);
    std::vector<QPoint> interpolate(const QPoint& start,const QPoint& end);

    void updateLasso(std::vector<QPoint>& points);
    void initializeLassoFlags();
    void removeLassoPixels();
    void redrawLassoPixels();

    void refreshImage();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorHealingCloneToolPlugin

#endif // DIGIKAM_EDITOR_HEALING_CLONE_TOOL_H
