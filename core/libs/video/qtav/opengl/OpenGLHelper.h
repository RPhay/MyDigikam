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

#ifndef QTAV_OPENGL_HELPER_H
#define QTAV_OPENGL_HELPER_H

#ifndef QT_NO_OPENGL
#   include "VideoFormat.h"
#   include "gl_api.h"

// for dynamicgl. qglfunctions before qt5.3 does not have portable gl functions

#   ifdef QT_OPENGL_DYNAMIC
#       define DYGL(glFunc) QOpenGLContext::currentContext()->functions()->glFunc
#   else
#       define DYGL(glFunc) glFunc
#   endif

#   define EGL_ENSURE(x, ...)                                                               \
    do {                                                                                    \
        if (!(x)) {                                                                         \
            EGLint err = eglGetError();                                                     \
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()                                      \
                << QString::asprintf("EGL error@%d<<%s. " #x ": %#x %s",                    \
                    __LINE__, __FILE__, err, eglQueryString(eglGetCurrentDisplay(), err));  \
            return __VA_ARGS__;                                                             \
        }                                                                                   \
    } while(0)

#   define EGL_WARN(x, ...)                                                                 \
    do {                                                                                    \
        if (!(x)) {                                                                         \
            EGLint err = eglGetError();                                                     \
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()                                      \
                << QString::asprintf("EGL error@%d<<%s. " #x ": %#x %s",                    \
                    __LINE__, __FILE__, err, eglQueryString(eglGetCurrentDisplay(), err));  \
        }                                                                                   \
    } while(0)


#   define WGL_ENSURE(x, ...)                                                               \
    do {                                                                                    \
        if (!(x)) {                                                                         \
            qCWarning(DIGIKAM_QTAV_LOG_WARN)                                                \
                << "WGL error " << __FILE__ << "@" << __LINE__                              \
                << " " << #x << ": " << qt_error_string(GetLastError());                    \
            return __VA_ARGS__;                                                             \
        }                                                                                   \
    } while(0)

#   define WGL_WARN(x, ...)                                                                 \
    do {                                                                                    \
        if (!(x)) {                                                                         \
            qCWarning(DIGIKAM_QTAV_LOG_WARN)                                                \
                << "WGL error " << __FILE__ << "@" << __LINE__                              \
                << " " << #x << ": " << qt_error_string(GetLastError());                    \
        }                                                                                   \
    } while(0)

class QMatrix4x4;

namespace QtAV
{

namespace OpenGLHelper
{

QString removeComments(const QString& code);

QByteArray compatibleShaderHeader(QOpenGLShader::ShaderType type);

int GLSLVersion();

bool isEGL();

bool isOpenGLES();

/*!
 * \brief hasExtensionEGL
 * Test if any of the given extensions is supported
 * \param exts Ends with nullptr
 * \return true if one of extension is supported
 */
bool hasExtensionEGL(const char* exts[]);

bool hasRG();

bool has16BitTexture();

/**
 * set by user (environment var "QTAV_TEXTURE16_DEPTH=8 or 16", default now is 8)
 */
int depth16BitTexture();

/**
 * set by user (environment var "QTAV_GL_DEPRECATED=1")
 */
bool useDeprecatedFormats();

/*!
 * \brief hasExtension
 * Test if any of the given extensions is supported. Current OpenGL context must be valid.
 * \param exts Ends with nullptr
 * \return true if one of extension is supported
 */
bool hasExtension(const char* exts[]);

bool isPBOSupported();

/*!
 * \brief videoFormatToGL
 * \param fmt
 * \param internal_format an array with size fmt.planeCount()
 * \param data_format an array with size fmt.planeCount()
 * \param data_type an array with size fmt.planeCount()
 * \param mat channel reorder matrix used in glsl
 * \return false if fmt is not supported
 */
bool videoFormatToGL(const VideoFormat& fmt, GLint* internal_format, GLenum* data_format,
                     GLenum* data_type, QMatrix4x4* mat = nullptr);

int bytesOfGLFormat(GLenum format, GLenum dataType = GL_UNSIGNED_BYTE);

} // namespace OpenGLHelper

} // namespace QtAV

#else

namespace QtAV
{

namespace OpenGLHelper
{

#   define DYGL(f) f

inline bool isOpenGLES()
{
    return false;
}

} // namespace OpenGLHelper

} // namespace QtAV

#endif // QT_NO_OPENGL

#endif // QTAV_OPENGL_HELPER_H
