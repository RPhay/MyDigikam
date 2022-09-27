/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#define IOS_USE_PRIVATE 0 // private symbols are forbidden by app store

// Local includes

#include "SurfaceInteropCV.h"

#ifdef Q_OS_IOS
#   import <OpenGLES/EAGL.h>
#   include "CoreVideo/CVOpenGLESTextureCache.h"
#   ifdef __IPHONE_11_0 // always defined in new sdk
#       import <OpenGLES/EAGLIOSurface.h>
#   endif // __IPHONE_11_0
#   if COREVIDEO_SUPPORTS_IOSURFACE && IOS_USE_PRIVATE

// Declare the private API if IOSurface is supported in SDK.
// Thus we can use IOSurface on any iOS version even with old SDK and compiler

@interface EAGLContext()
- (BOOL)texImageIOSurface:(IOSurfaceRef)ioSurface target:(NSUInteger)target internalFormat:(NSUInteger)internalFormat width:(uint32_t)width height:(uint32_t)height format:(NSUInteger)format type:(NSUInteger)type plane:(uint32_t)plane invert:(BOOL)invert NS_AVAILABLE_IOS(4_0); // confirmed in iOS5.1
@end

#   endif // COREVIDEO_SUPPORTS_IOSURFACE
#else
#   include <IOSurface/IOSurface.h>
#endif // Q_OS_IOS

#include "VideoFrame.h"
#include "OpenGLHelper.h"
#include "digikam_debug.h"

namespace QtAV
{

namespace cv
{

VideoFormat::PixelFormat format_from_cv(int cv);

// https://www.opengl.org/registry/specs/APPLE/rgb_422.txt
// https://www.opengl.org/registry/specs/APPLE/ycbcr_422.txt
// uyvy: UNSIGNED_SHORT_8_8_REV_APPLE, yuy2: GL_UNSIGNED_SHORT_8_8_APPLE
// check extension GL_APPLE_rgb_422 and rectangle?

class InteropResourceIOSurface Q_DECL_FINAL : public InteropResource
{
public:

    bool stridesForWidth(int cvfmt, int width, int* strides, VideoFormat::PixelFormat* outFmt) override;

    bool mapToTexture2D() const                                                                override
    {
        return false;
    }

    bool map(CVPixelBufferRef buf, GLuint* tex, int w, int h, int plane)                       override;

    GLuint createTexture(CVPixelBufferRef, const VideoFormat& fmt,
                         int plane, int planeWidth,
                         int planeHeight)                                                      override
    {
        Q_UNUSED(fmt);
        Q_UNUSED(plane);
        Q_UNUSED(planeWidth);
        Q_UNUSED(planeHeight);
        GLuint tex = 0;
        DYGL(glGenTextures(1, &tex));

        return tex;
    }
};

InteropResource* CreateInteropIOSurface()
{
    return new InteropResourceIOSurface();
}

bool InteropResourceIOSurface::stridesForWidth(int cvfmt, int width, int* strides, VideoFormat::PixelFormat* outFmt)
{
    switch (cvfmt)
    {
        case '2vuy':
        case 'yuvs':
        {
            *outFmt = VideoFormat::Format_VYU;

            if (strides[0] <= 0)
                strides[0] = 4*width; //RGB layout: BRGX
            else
                strides[0] *= 2;

            break;
        }

        default:
        {
            return InteropResource::stridesForWidth(cvfmt, width, strides, outFmt);
        }
    }

    return true;
}

bool InteropResourceIOSurface::map(CVPixelBufferRef buf, GLuint* tex, int w, int h, int plane)
{
    Q_UNUSED(w);
    Q_UNUSED(h);

#if COREVIDEO_SUPPORTS_IOSURFACE // IOSurface header is not included otherwise

    const OSType pixfmt = CVPixelBufferGetPixelFormatType(buf);
    GLint iformat;
    GLenum format, dtype;
    getParametersGL(pixfmt, &iformat, &format, &dtype, plane);

    switch (pixfmt)
    {
        case '2vuy':
        case 'yuvs':
        {
            // ES2 requires internal format and format are the same. desktop can use internal format GL_RGB or sized GL_RGB8

#ifdef Q_OS_IOS

            iformat = GL_RGB_422_APPLE;

#else

            iformat = GL_RGB8;

#endif

            format = GL_RGB_422_APPLE;
            dtype  = (pixfmt == '2vuy' ? GL_UNSIGNED_SHORT_8_8_APPLE : GL_UNSIGNED_SHORT_8_8_REV_APPLE);

            break;
        }

        // macOS: GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV
        // GL_YCBCR_422_APPLE: convert to rgb texture internally (bt601). only supports OSX
        // GL_RGB_422_APPLE: raw yuv422 texture

        case 'BGRA':
        {

#ifdef Q_OS_IOS

            iformat = GL_RGBA;
            format  = GL_RGBA;

#else

            iformat = GL_RGBA8;
            format  = GL_BGRA;
            dtype   = GL_UNSIGNED_INT_8_8_8_8_REV;

#endif

            break;
        }

        default:
        {
            break;
        }
    }

    const GLenum target = GL_TEXTURE_RECTANGLE;
    DYGL(glBindTexture(target, *tex));
    const int planeW    = CVPixelBufferGetWidthOfPlane(buf, plane);
    const int planeH    = CVPixelBufferGetHeightOfPlane(buf, plane);
/*
    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("map plane%d. %dx%d, gl %d %d %d",
            plane, planeW, planeH, iformat, format, dtype);
*/
    const IOSurfaceRef surface  = CVPixelBufferGetIOSurface(buf);             // available in ios11 sdk, ios4 runtime

#ifdef Q_OS_IOS

    BOOL ok = false;

#   ifdef __IPHONE_11_0
#       if __has_builtin(__builtin_available) // xcode9: runtime check @available is required, or -Wno-unguarded-availability-new

    if (@available(iOS 11, *)) // symbol is available in iOS5+, but crashes at runtime

#  else

    if (false)

#  endif // __has_builtin(__builtin_available)

        ok = [[EAGLContext currentContext] texImageIOSurface:surface target:target internalFormat:iformat width:planeW height:planeH format:format type:dtype plane:plane];

    else // fallback to old private api if runtime version < 11

# endif // __IPHONE_11_0

    {

#if IOS_USE_PRIVATE

        ok = [[EAGLContext currentContext] texImageIOSurface:surface target:target internalFormat:iformat width:planeW height:planeH format:format type:dtype plane:plane invert:NO];

#endif // IOS_USE_PRIVATE

    }

    if (!ok)
    {
         qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("error creating IOSurface texture at plane %d", plane);
    }

#else

    CGLError err = CGLTexImageIOSurface2D(CGLGetCurrentContext(), target, iformat, planeW, planeH, format, dtype, surface, plane);

    if (err != kCGLNoError)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("error creating IOSurface texture at plane %d: %s",
                plane, CGLErrorString(err));
    }

#endif // Q_OS_IOS

    DYGL(glBindTexture(target, 0));

#endif // COREVIDEO_SUPPORTS_IOSURFACE

    return true;
}

} // namespace cv

} // namespace QtAV
