/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *               Add function calls before/after main()
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef QTAV_PREPOST_H
#define QTAV_PREPOST_H

/*
 * Avoid a compiler warning when the arguments is empty,
 * e.g. PRE_FUNC_ADD(foo). The right one is PRE_FUNC_ADD(f,)
 */

/*
 * TODO:
 *  boost::call_once
 *  http://stackoverflow.com/questions/4173384/how-to-make-sure-a-function-is-only-called-once                                  // krazy:exclude=insecurenet
 *  http://publib.boulder.ibm.com/infocenter/lnxpcomp/v8v101/index.jsp?topic=%2Fcom.ibm.xlcpp8l.doc%2Flanguage%2Fref%2Fco.htm   // krazy:exclude=insecurenet
 */

#ifdef __cplusplus

/*
 * for C++, we use non-local static object to call the functions automatically before main().
 * anonymous namespace: avoid name confliction('static' keyword is not necessary)
 */

#define PRE_FUNC_ADD(f, .../*args*/)                \
    namespace {                                     \
        static const struct initializer_for_##f {   \
            inline initializer_for_##f() {          \
                f(__VA_ARGS__);                     \
            }                                       \
        }  __sInit_##f;                             \
    }

#define POST_FUNC_ADD(f, .../*args*/)                           \
    namespace {                                                 \
        static const struct deinitializer_for_##f {             \
            inline deinitializer_for_##f() {}                   \
            inline ~deinitializer_for_##f() { f(__VA_ARGS__); } \
        } __sDeinit_##f;                                        \
    }

#else /*for C. ! defined __cplusplus*/

/*
 *http://buliedian.iteye.com/blog/1069072                                                       // krazy:exclude=insecurenet
 *http://research.microsoft.com/en-us/um/redmond/projects/invisible/src/crt/md/ppc/_crt.c.htm   // krazy:exclude=insecurenet
 */

#if defined(_MSC_VER)                           // krazy:exclude=cpp
#   pragma section(".CRT$XIU", long, read)
#   pragma section(".CRT$XPU", long, read)
#   define _CRTALLOC(x) __declspec(allocate(x))

/* TODO: Auto unique naming */

typedef int (__cdecl *_PF)();   /* why not void? */

/* static to avoid multiple defination */

#   define PRE_FUNC_ADD(f, .../*args*/)                 \
    static int init_##f() { f(__VA_ARGS__); return 0;}  \
    _CRTALLOC(".CRT$XIU") static _PF pinit_##f [] = { init_##f }; /*static void (*pinit_##f)() = init_##f //__cdecl */

#   define POST_FUNC_ADD(f, .../*args*/)                    \
    static int deinit_##f() { f(__VA_ARGS__); return 0;}    \
    _CRTALLOC(".CRT$XPU") static _PF pdeinit_##f [] = { deinit_##f };

#elif defined(__GNUC__)
#   define PRE_FUNC_ADD(f, ...) \
    __attribute__((constructor)) static void init_##f() { f(__VA_ARGS__); }

#   define POST_FUNC_ADD(f, ...) \
    __attribute__((destructor)) static void deinit_##f() { f(__VA_ARGS__); }

#else
#   ifndef __cplusplus
#       error Not supported for C: PRE_FUNC_ADD, POST_FUNC_ADD
#   endif
#   include <stdlib.h>

/* static var init, atexit */

#   define PRE_FUNC_ADD(f, ...)                         \
    static int init_##f() { f(__VA_ARGS__); return 0; } \
    static int v_init_##f = init_##f();

/*
 * Works for C++. For C, gcc will throw an error:
 * initializer element is not constant
 */

// atexit do not support arguments

#   define POST_FUNC_ADD(f, ...)            \
    static void atexit_##f() { atexit(f); } \
    PRE_FUNC_ADD(atexit_##f)
#endif

#endif //__cplusplus

#endif // QTAV_PREPOST_H
