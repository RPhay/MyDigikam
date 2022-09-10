/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef QTAV_OPENGL_RENDERER_BASE_P_H
#define QTAV_OPENGL_RENDERER_BASE_P_H

#include "OpenGLRendererBase.h"

// Local includes

#include "VideoRenderer_p.h"
#include "OpenGLVideo.h"

namespace QtAV
{

class DIGIKAM_EXPORT OpenGLRendererBasePrivate : public VideoRendererPrivate
{
public:

    explicit OpenGLRendererBasePrivate(QPaintDevice* const pd);
    virtual ~OpenGLRendererBasePrivate();

    void setupAspectRatio();

public:

    QPainter*   painter       = nullptr;
    OpenGLVideo glv;
    QMatrix4x4  matrix;
    bool        frame_changed = false;
};

} // namespace QtAV

#endif // QTAV_OPENGL_RENDER_BASE_P_H
