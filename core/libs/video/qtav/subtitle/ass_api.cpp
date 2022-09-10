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

#define ASS_CAPI_BUILD

/*
#define CAPI_IS_LAZY_RESOLVE 0
*/

#ifndef CAPI_LINK_ASS
#   include "QtAV_capi.h"
#endif

#include "ass_api.h" // include last to avoid covering types later

namespace ass
{

#ifdef CAPI_LINK_ASS

api::api()               { dll = nullptr; }
api::~api()              {                }
bool api::loaded() const { return true;   }

#else

static const char* names[] =
{
    "ass",

#ifdef CAPI_TARGET_OS_WIN

    "libass",

#endif

    nullptr
};

typedef ::capi::dso user_dso;

#if 1

static const int versions[] =
{
    ::capi::NoVersion,

    // the following line will be replaced by the content of config/ass/version if exists

    5,
    4,
    ::capi::EndVersion
};

CAPI_BEGIN_DLL_VER(names, versions, user_dso)

# else

CAPI_BEGIN_DLL(names, user_dso)

# endif

// CAPI_DEFINE_RESOLVER(argc, return_type, name, argv_no_name)
/*
CAPI_DEFINE_ENTRY(int, ass_library_version, CAPI_ARG0())                                                              // cppcheck-suppress thisSubtraction
*/
CAPI_DEFINE_ENTRY(ASS_Library*, ass_library_init, CAPI_ARG0())                                                        // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(void, ass_library_done, CAPI_ARG1(ASS_Library*))                                                    // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(void, ass_set_fonts_dir, CAPI_ARG2(ASS_Library*, const char*))                                      // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(void, ass_set_extract_fonts, CAPI_ARG2(ASS_Library*, int))                                          // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(void, ass_set_style_overrides, CAPI_ARG2(ASS_Library*, char**))                                     // cppcheck-suppress thisSubtraction
/*
CAPI_DEFINE_ENTRY(void, ass_process_force_style, CAPI_ARG1(ASS_Track*))                                               // cppcheck-suppress thisSubtraction
*/
CAPI_DEFINE_ENTRY(void, ass_set_message_cb, CAPI_ARG3(ASS_Library*, void (*msg_cb)                                    // cppcheck-suppress thisSubtraction
                        (int level, const char* fmt, va_list args, void* data), void*))                               // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(ASS_Renderer*, ass_renderer_init, CAPI_ARG1(ASS_Library*))                                          // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(void, ass_renderer_done, CAPI_ARG1(ASS_Renderer*))                                                  // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(void, ass_set_frame_size, CAPI_ARG3(ASS_Renderer*, int, int))                                       // cppcheck-suppress thisSubtraction
/*
CAPI_DEFINE_ENTRY(void, ass_set_storage_size, CAPI_ARG3(ASS_Renderer*, int, int))                                     // cppcheck-suppress thisSubtraction
*/
CAPI_DEFINE_ENTRY(void, ass_set_shaper, CAPI_ARG2(ASS_Renderer*, ASS_ShapingLevel))                                   // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(void, ass_set_margins, CAPI_ARG5(ASS_Renderer*, int, int, int, int))                                // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(void, ass_set_use_margins, CAPI_ARG2(ASS_Renderer*, int))                                           // cppcheck-suppress thisSubtraction
/*
CAPI_DEFINE_ENTRY(void, ass_set_pixel_aspect, CAPI_ARG2(ASS_Renderer*, double))                                       // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(void, ass_set_aspect_ratio, CAPI_ARG3(ASS_Renderer*, double, double))                               // cppcheck-suppress thisSubtraction
*/
CAPI_DEFINE_ENTRY(void, ass_set_font_scale, CAPI_ARG2(ASS_Renderer*, double))                                         // cppcheck-suppress thisSubtraction
/*
CAPI_DEFINE_ENTRY(void, ass_set_hinting, CAPI_ARG2(ASS_Renderer*, ASS_Hinting))                                       // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(void, ass_set_line_spacing, CAPI_ARG2(ASS_Renderer*, double))                                       // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(void, ass_set_line_position, CAPI_ARG2(ASS_Renderer*, double))                                      // cppcheck-suppress thisSubtraction
*/
CAPI_DEFINE_ENTRY(void, ass_set_fonts, CAPI_ARG6(ASS_Renderer*, const char*, const char*, int, const char*, int))     // cppcheck-suppress thisSubtraction
/*
CAPI_DEFINE_ENTRY(void, ass_set_selective_style_override_enabled, CAPI_ARG2(ASS_Renderer*, int))                      // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(void, ass_set_selective_style_override, CAPI_ARG2(ASS_Renderer*, ASS_Style *))                      // cppcheck-suppress thisSubtraction
*/
CAPI_DEFINE_ENTRY(int, ass_fonts_update, CAPI_ARG1(ASS_Renderer*))                                                    // cppcheck-suppress thisSubtraction
/*
CAPI_DEFINE_ENTRY(void, ass_set_cache_limits, CAPI_ARG3(ASS_Renderer*, int, int))                                     // cppcheck-suppress thisSubtraction
*/
CAPI_DEFINE_ENTRY(ASS_Image*, ass_render_frame, CAPI_ARG4(ASS_Renderer*, ASS_Track*, long long, int*))                // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(ASS_Track*, ass_new_track, CAPI_ARG1(ASS_Library*))                                                 // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(void, ass_free_track, CAPI_ARG1(ASS_Track*))                                                        // cppcheck-suppress thisSubtraction
/*
CAPI_DEFINE_ENTRY(int, ass_alloc_style, CAPI_ARG1(ASS_Track*))                                                        // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(int, ass_alloc_event, CAPI_ARG1(ASS_Track*))                                                        // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(void, ass_free_style, CAPI_ARG2(ASS_Track*, int))                                                   // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(void, ass_free_event, CAPI_ARG2(ASS_Track*, int))                                                   // cppcheck-suppress thisSubtraction
*/
CAPI_DEFINE_ENTRY(void, ass_process_data, CAPI_ARG3(ASS_Track*, char*, int))                                          // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(void, ass_process_codec_private, CAPI_ARG3(ASS_Track*, char*, int))                                 // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(void, ass_process_chunk, CAPI_ARG5(ASS_Track*, char*, int, long long, long long))                   // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(void, ass_set_check_readorder, CAPI_ARG2(ASS_Track*, int))                                          // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(void, ass_flush_events, CAPI_ARG1(ASS_Track*))                                                      // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(ASS_Track*, ass_read_file, CAPI_ARG3(ASS_Library*, char*, char*))                                   // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(ASS_Track*, ass_read_memory, CAPI_ARG4(ASS_Library*, char*, size_t, char*))                         // cppcheck-suppress thisSubtraction
/*
CAPI_DEFINE_ENTRY(int, ass_read_styles, CAPI_ARG3(ASS_Track*, char*, char*))                                          // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(void, ass_add_font, CAPI_ARG4(ASS_Library*, char*, char*, int))                                     // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(void, ass_clear_fonts, CAPI_ARG1(ASS_Library*))                                                     // cppcheck-suppress thisSubtraction
CAPI_DEFINE_ENTRY(long long, ass_step_sub, CAPI_ARG3(ASS_Track*, long long, int))                                     // cppcheck-suppress thisSubtraction
*/
CAPI_END_DLL()
CAPI_DEFINE_DLL                                                                                                       // cppcheck-suppress[noOperatorEq,noCopyConstructor]

// CAPI_DEFINE(argc, return_type, name, argv_no_name)

typedef void (*ass_set_message_cb_msg_cb1)
              (int level, const char* fmt, va_list args, void* data);
/*
CAPI_DEFINE(int, ass_library_version, CAPI_ARG0())
*/
CAPI_DEFINE(ASS_Library*, ass_library_init, CAPI_ARG0())
CAPI_DEFINE(void, ass_library_done, CAPI_ARG1(ASS_Library*))
CAPI_DEFINE(void, ass_set_fonts_dir, CAPI_ARG2(ASS_Library*, const char*))
CAPI_DEFINE(void, ass_set_extract_fonts, CAPI_ARG2(ASS_Library*, int))
CAPI_DEFINE(void, ass_set_style_overrides, CAPI_ARG2(ASS_Library*, char**))
/*
CAPI_DEFINE(void, ass_process_force_style, CAPI_ARG1(ASS_Track*))
*/
CAPI_DEFINE(void, ass_set_message_cb, CAPI_ARG3(ASS_Library*, ass_set_message_cb_msg_cb1, void*))
CAPI_DEFINE(ASS_Renderer*, ass_renderer_init, CAPI_ARG1(ASS_Library*))
CAPI_DEFINE(void, ass_renderer_done, CAPI_ARG1(ASS_Renderer*))
CAPI_DEFINE(void, ass_set_frame_size, CAPI_ARG3(ASS_Renderer*, int, int))
/*
CAPI_DEFINE(void, ass_set_storage_size, CAPI_ARG3(ASS_Renderer*, int, int))
*/
CAPI_DEFINE(void, ass_set_shaper, CAPI_ARG2(ASS_Renderer*, ASS_ShapingLevel))
CAPI_DEFINE(void, ass_set_margins, CAPI_ARG5(ASS_Renderer*, int, int, int, int))
CAPI_DEFINE(void, ass_set_use_margins, CAPI_ARG2(ASS_Renderer*, int))
/*
CAPI_DEFINE(void, ass_set_pixel_aspect, CAPI_ARG2(ASS_Renderer*, double))
CAPI_DEFINE(void, ass_set_aspect_ratio, CAPI_ARG3(ASS_Renderer*, double, double))
*/
CAPI_DEFINE(void, ass_set_font_scale, CAPI_ARG2(ASS_Renderer*, double))
/*
CAPI_DEFINE(void, ass_set_hinting, CAPI_ARG2(ASS_Renderer*, ASS_Hinting))
CAPI_DEFINE(void, ass_set_line_spacing, CAPI_ARG2(ASS_Renderer*, double))
CAPI_DEFINE(void, ass_set_line_position, CAPI_ARG2(ASS_Renderer*, double))
*/
CAPI_DEFINE(void, ass_set_fonts, CAPI_ARG6(ASS_Renderer*, const char*, const char*, int, const char*, int))
/*
CAPI_DEFINE(void, ass_set_selective_style_override_enabled, CAPI_ARG2(ASS_Renderer*, int))
CAPI_DEFINE(void, ass_set_selective_style_override, CAPI_ARG2(ASS_Renderer*, ASS_Style*))
*/
CAPI_DEFINE(int, ass_fonts_update, CAPI_ARG1(ASS_Renderer*))
/*
CAPI_DEFINE(void, ass_set_cache_limits, CAPI_ARG3(ASS_Renderer*, int, int))
*/
CAPI_DEFINE(ASS_Image*, ass_render_frame, CAPI_ARG4(ASS_Renderer*, ASS_Track*, long long, int*))
CAPI_DEFINE(ASS_Track*, ass_new_track, CAPI_ARG1(ASS_Library*))
CAPI_DEFINE(void, ass_free_track, CAPI_ARG1(ASS_Track*))
/*
CAPI_DEFINE(int, ass_alloc_style, CAPI_ARG1(ASS_Track*))
CAPI_DEFINE(int, ass_alloc_event, CAPI_ARG1(ASS_Track*))
CAPI_DEFINE(void, ass_free_style, CAPI_ARG2(ASS_Track*, int))
CAPI_DEFINE(void, ass_free_event, CAPI_ARG2(ASS_Track*, int))
*/
CAPI_DEFINE(void, ass_process_data, CAPI_ARG3(ASS_Track*, char*, int))
CAPI_DEFINE(void, ass_process_codec_private, CAPI_ARG3(ASS_Track*, char*, int))
CAPI_DEFINE(void, ass_process_chunk, CAPI_ARG5(ASS_Track*, char*, int, long long, long long))
CAPI_DEFINE(void, ass_set_check_readorder, CAPI_ARG2(ASS_Track*, int))
CAPI_DEFINE(void, ass_flush_events, CAPI_ARG1(ASS_Track*))
CAPI_DEFINE(ASS_Track *, ass_read_file, CAPI_ARG3(ASS_Library*, char*, char*))
CAPI_DEFINE(ASS_Track *, ass_read_memory, CAPI_ARG4(ASS_Library*, char*, size_t, char*))
/*
CAPI_DEFINE(int, ass_read_styles, CAPI_ARG3(ASS_Track*, char*, char*))
CAPI_DEFINE(void, ass_add_font, CAPI_ARG4(ASS_Library*, char*, char*, int))
CAPI_DEFINE(void, ass_clear_fonts, CAPI_ARG1(ASS_Library*))
CAPI_DEFINE(long long, ass_step_sub, CAPI_ARG3(ASS_Track*, long long, int))
*/
#endif

} // namespace ass
