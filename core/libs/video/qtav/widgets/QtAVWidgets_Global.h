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

#ifndef QTAV_WIDGETS_GLOBAL_H
#define QTAV_WIDGETS_GLOBAL_H

// Local includes

#include "VideoRenderer.h"
#include "digikam_export.h"

namespace QtAV
{

namespace Widgets
{

/*!
 * \brief registerRenderers
 * register built-in renderers.
 * If you do not explicitly use any var, function or class in this module in your code,
 * QtAVWidgets module maybe not linked to your program and renderers will not be available.
 * Then you have to call registerRenderers() to ensure QtAVWidgets module is linked.
 */
DIGIKAM_EXPORT void registerRenderers();

} // namespace Widgets

extern DIGIKAM_EXPORT VideoRendererId VideoRendererId_Widget;
extern DIGIKAM_EXPORT VideoRendererId VideoRendererId_GraphicsItem;
extern DIGIKAM_EXPORT VideoRendererId VideoRendererId_GLWidget;
extern DIGIKAM_EXPORT VideoRendererId VideoRendererId_GDI;
extern DIGIKAM_EXPORT VideoRendererId VideoRendererId_Direct2D;
extern DIGIKAM_EXPORT VideoRendererId VideoRendererId_XV;
extern DIGIKAM_EXPORT VideoRendererId VideoRendererId_X11;
extern DIGIKAM_EXPORT VideoRendererId VideoRendererId_GLWidget2;
extern DIGIKAM_EXPORT VideoRendererId VideoRendererId_OpenGLWidget;

// popup a dialog

DIGIKAM_EXPORT void about();
DIGIKAM_EXPORT void aboutFFmpeg();
DIGIKAM_EXPORT void aboutQtAV();

} // namespace QtAV

#endif // QTAV_WIDGETS_GLOBAL_H
