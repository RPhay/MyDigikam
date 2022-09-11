/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-08-24
 * Description : a tool to reduce CCD noise.
 *
 * SPDX-FileCopyrightText: 2004-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_EDITOR_NOISE_REDUCTION_TOOL_H
#define DIGIKAM_EDITOR_NOISE_REDUCTION_TOOL_H

// Local includes

#include "editortool.h"

using namespace Digikam;

namespace DigikamEditorNoiseReductionToolPlugin
{

class NoiseReductionTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit NoiseReductionTool(QObject* const parent);
    ~NoiseReductionTool()       override;

private:

    void readSettings()         override;
    void writeSettings()        override;
    void preparePreview()       override;
    void prepareFinal()         override;
    void setPreviewImage()      override;
    void setFinalImage()        override;
    void analyserCompleted()    override;

private Q_SLOTS:

    void slotResetSettings()    override;
    void slotLoadSettings()     override;
    void slotSaveAsSettings()   override;
    void slotEstimateNoise();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorNoiseReductionToolPlugin

#endif // DIGIKAM_EDITOR_NOISE_REDUCTION_TOOL_H
