/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef QTAVWIDGETS_GLOBAL_H
#define QTAVWIDGETS_GLOBAL_H

// Local includes

#include "VideoRenderer.h"
#include "digikam_export.h"

#define Q_AVWIDGETS_EXPORT DIGIKAM_EXPORT
/*
#ifdef BUILD_QTAVWIDGETS_STATIC
#define Q_AVWIDGETS_EXPORT
#else
#if defined(BUILD_QTAVWIDGETS_LIB)
#  undef Q_AVWIDGETS_EXPORT
#  define Q_AVWIDGETS_EXPORT Q_DECL_EXPORT
#else
#  undef Q_AVWIDGETS_EXPORT
#  define Q_AVWIDGETS_EXPORT Q_DECL_IMPORT //only for vc?
#endif
#endif //BUILD_QTAVWIDGETS_STATIC
*/
#define Q_AVWIDGETS_PRIVATE_EXPORT Q_AVWIDGETS_EXPORT

#if defined(BUILD_QTAVWIDGETS_LIB)
#   define QTAVWIDGETS_DEPRECATED
#else
#   define QTAVWIDGETS_DEPRECATED Q_DECL_DEPRECATED
#endif

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
Q_AVWIDGETS_EXPORT void registerRenderers();

} // namespace Widgets

extern Q_AVWIDGETS_EXPORT VideoRendererId VideoRendererId_Widget;
extern Q_AVWIDGETS_EXPORT VideoRendererId VideoRendererId_GraphicsItem;
extern Q_AVWIDGETS_EXPORT VideoRendererId VideoRendererId_GLWidget;
extern Q_AVWIDGETS_EXPORT VideoRendererId VideoRendererId_GDI;
extern Q_AVWIDGETS_EXPORT VideoRendererId VideoRendererId_Direct2D;
extern Q_AVWIDGETS_EXPORT VideoRendererId VideoRendererId_XV;
extern Q_AVWIDGETS_EXPORT VideoRendererId VideoRendererId_X11;
extern Q_AVWIDGETS_EXPORT VideoRendererId VideoRendererId_GLWidget2;
extern Q_AVWIDGETS_EXPORT VideoRendererId VideoRendererId_OpenGLWidget;

// popup a dialog
Q_AVWIDGETS_EXPORT void about();
Q_AVWIDGETS_EXPORT void aboutFFmpeg();
Q_AVWIDGETS_EXPORT void aboutQtAV();

} // namespace QtAV

#endif // QTAVWIDGETS_GLOBAL_H
