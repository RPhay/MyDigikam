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

#include "gl_api.h"

// Local includes

#include "OpenGLHelper.h"
#include "digikam_debug.h"

namespace QtAV
{

typedef void* (*GetProcAddress_t)(const char*);

static GetProcAddress_t sGetProcAddress;

void* GetProcAddress_Qt(const char* name)
{
    if (!QOpenGLContext::currentContext())
        return nullptr;

    void* p = (void*)QOpenGLContext::currentContext()->getProcAddress(QByteArray((const char*)name));

    if (!p)
    {

#if defined(Q_OS_WIN) && defined(QT_OPENGL_DYNAMIC)

        HMODULE handle = (HMODULE)QOpenGLContext::openGLModuleHandle();

        if (handle)
            p = (void*)GetProcAddress(handle, name);

#endif

    }

    // fallback to QOpenGLFunctions_1_0?

    return p;
}

static void* GetProcAddressWithExt(GetProcAddress_t get, const char* name)
{
    void* fp = get(name);

    if (fp)
        return fp;

    static const char* ext[] =
    {
        "ARB",
        "OES",
        "EXT",
        "ANGLE",
        "NV" // TODO: MESA, INTEL?

#ifdef __APPLE__         // krazy:exclude=cpp

      , "APPLE"

#endif

      , nullptr
    };

    char f[512]   = { 0 };
    memcpy(f, name, strlen(name));
    char* const p = f + strlen(name);

    for (int i = 0 ; ext[i] ; ++i)
    {
        memcpy(p, ext[i], sizeof(ext[i]) + 1); // copy trail '\0'
        fp = get(f);

        if (fp)
        {
            qCDebug(DIGIKAM_QTAV_LOG) << "extension resolved:" << f;

            return fp;
        }
    }

    return nullptr;
}

static void* GetProcAddressDefault(const char* name)
{
    return GetProcAddressWithExt(GetProcAddress_Qt, name);
}

#ifdef QT_OPENGL_DYNAMIC
#   define GETPROCADDRESS_RESOLVE

// GL_RESOLVE_ES_X_X will link to lib or use getProcAddress

#   define GL_RESOLVE_ES_2_0(name) GL_RESOLVE(name)
#   define GL_RESOLVE_ES_3_0(name) GL_RESOLVE(name)
#   define GL_RESOLVE_ES_3_1(name) GL_RESOLVE(name)
#else
#   if defined GL_ES_VERSION_2_0
#       define GL_RESOLVE_ES_2_0(name) GL_RESOLVE(name)
#       define GL_RESOLVE_ES_3_0(name) GL_RESOLVE_EXT(name)
#       define GL_RESOLVE_ES_3_1(name) GL_RESOLVE_EXT(name)
#   elif defined GL_ES_VERSION_3_0
#       define GL_RESOLVE_ES_2_0(name) GL_RESOLVE(name)
#       define GL_RESOLVE_ES_3_0(name) GL_RESOLVE(name)
#       define GL_RESOLVE_ES_3_1(name) GL_RESOLVE_NONE(name) // gl3ext is empty
#   elif defined GL_ES_VERSION_3_1
#       define GL_RESOLVE_ES_2_0(name) GL_RESOLVE(name)
#       define GL_RESOLVE_ES_3_0(name) GL_RESOLVE(name)
#       define GL_RESOLVE_ES_3_1(name) GL_RESOLVE(name)
#   else
#       define GETPROCADDRESS_RESOLVE

// GL_RESOLVE_ES_X_X will link to lib or use getProcAddress

#       define GL_RESOLVE_ES_2_0(name) GL_RESOLVE(name)
#       define GL_RESOLVE_ES_3_0(name) GL_RESOLVE(name)
#       define GL_RESOLVE_ES_3_1(name) GL_RESOLVE(name)
#   endif
#endif

#define GL_RESOLVE_NONE(name) do { name = nullptr;}while(0)
#define GL_RESOLVE_EXT(name) do {                   \
    void** fp = (void**)(&name);                    \
    *fp       = GetProcAddressDefault("gl" # name); \
} while(0)

#ifdef GETPROCADDRESS_RESOLVE
#   define GL_RESOLVE(name) GL_RESOLVE_EXT(name)
#else
#   define GL_RESOLVE(name)  do {   \
    name = ::gl##name;              \
} while(0)
#endif

#define WGL_RESOLVE(name) do {                  \
    void** fp = (void**)(&name);                \
    *fp       = sGetProcAddress("wgl" # name);  \
} while(0)

void api::resolve()
{
/*
    memset(g, 0, sizeof(g));
*/
    sGetProcAddress = GetProcAddressDefault;
    GL_RESOLVE(GetString);
    GL_RESOLVE(GetError);
    GL_RESOLVE(ActiveTexture);
    GL_RESOLVE(BindFramebuffer);
    GL_RESOLVE(GetUniformLocation);
    GL_RESOLVE(Uniform1f);
    GL_RESOLVE(Uniform2f);
    GL_RESOLVE(Uniform3f);
    GL_RESOLVE(Uniform4f);
    GL_RESOLVE(Uniform1fv);
    GL_RESOLVE(Uniform2fv);
    GL_RESOLVE(Uniform3fv);
    GL_RESOLVE(Uniform4fv);
    GL_RESOLVE(Uniform1iv);
    GL_RESOLVE(Uniform2iv);
    GL_RESOLVE(Uniform3iv);
    GL_RESOLVE(Uniform4iv);
    GL_RESOLVE(UniformMatrix2fv);
    GL_RESOLVE(UniformMatrix3fv);
    GL_RESOLVE(UniformMatrix4fv);
    GL_RESOLVE(BlendFuncSeparate);

    GL_RESOLVE_ES_3_1(GetTexLevelParameteriv);

#ifdef Q_OS_WIN32

    if (!OpenGLHelper::isOpenGLES())
    {
        static const char* ext[] =
        {
            "WGL_NV_DX_interop2",
            "WGL_NV_DX_interop",
            nullptr
        };

        if (OpenGLHelper::hasExtension(ext))
        {
            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("resolving WGL_NV_DX_interop...");

            WGL_RESOLVE(DXSetResourceShareHandleNV);
            WGL_RESOLVE(DXOpenDeviceNV);
            WGL_RESOLVE(DXCloseDeviceNV);
            WGL_RESOLVE(DXRegisterObjectNV);
            WGL_RESOLVE(DXUnregisterObjectNV);
            WGL_RESOLVE(DXObjectAccessNV);
            WGL_RESOLVE(DXLockObjectsNV);
            WGL_RESOLVE(DXUnlockObjectsNV);
        }
    }

#endif // Q_OS_WIN32

}

api& gl()
{
    static api g;

    if (!sGetProcAddress)
    {
        g.resolve();
    }

    return g;
}

} // namespace QtAV
