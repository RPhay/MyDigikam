/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

// u_TextureN: yuv. use array?
uniform sampler2D u_Texture0;
uniform sampler2D u_Texture1;
uniform sampler2D u_Texture2;
#ifdef HAS_ALPHA
uniform sampler2D u_Texture3;
#endif //HAS_ALPHA
varying vec2 v_TexCoords0;
#ifdef MULTI_COORD
varying vec2 v_TexCoords1;
varying vec2 v_TexCoords2;
#ifdef HAS_ALPHA
varying vec2 v_TexCoords3;
#endif
#else
#define v_TexCoords1 v_TexCoords0
#define v_TexCoords2 v_TexCoords0
#define v_TexCoords3 v_TexCoords0
#endif //MULTI_COORD
uniform float u_opacity;
uniform mat4 u_colorMatrix;
#ifdef CHANNEL16_TO8
uniform vec2 u_to8;
#endif
/***User header code***%userHeader%***/
// matrixCompMult for convolution
/***User sampling function here***%userSample%***/
#ifndef USER_SAMPLER
vec4 sample2d(sampler2D tex, vec2 pos, int plane)
{
    return texture(tex, pos);
}
#endif

// 10, 16bit: http://msdn.microsoft.com/en-us/library/windows/desktop/bb970578%28v=vs.85%29.aspx
void main()
{
    gl_FragColor = clamp(u_colorMatrix
                         * vec4(
#ifdef CHANNEL16_TO8
#ifdef USE_RG
                             dot(sample2d(u_Texture0, v_TexCoords0, 0).rg, u_to8),
                             dot(sample2d(u_Texture1, v_TexCoords1, 1).rg, u_to8),
                             dot(sample2d(u_Texture2, v_TexCoords2, 2).rg, u_to8),
#else
                             dot(sample2d(u_Texture0, v_TexCoords0, 0).ra, u_to8),
                             dot(sample2d(u_Texture1, v_TexCoords1, 1).ra, u_to8),
                             dot(sample2d(u_Texture2, v_TexCoords2, 2).ra, u_to8),
#endif //USE_RG
#else
#ifdef USE_RG
                             sample2d(u_Texture0, v_TexCoords0, 0).r,
                             sample2d(u_Texture1, v_TexCoords1, 1).r,
#ifdef IS_BIPLANE
                             sample2d(u_Texture2, v_TexCoords2, 2).g,
#else
                             sample2d(u_Texture2, v_TexCoords2, 2).r,
#endif //IS_BIPLANE
#else
// use r, g, a to work for both yv12 and nv12. idea from xbmc
                             sample2d(u_Texture0, v_TexCoords0, 0).r,
                             sample2d(u_Texture1, v_TexCoords1, 1).g,
                             sample2d(u_Texture2, v_TexCoords2, 2).a,
#endif //USE_RG
#endif //CHANNEL16_TO8
                             1.0
                            )
                         , 0.0, 1.0) * u_opacity;
#ifdef HAS_ALPHA
    float a =
#ifdef CHANNEL16_TO8
#ifdef USE_RG
    dot(sample2d(u_Texture3, v_TexCoords3, 3).rg, u_to8);
#else
    dot(sample2d(u_Texture3, v_TexCoords3, 3).ra, u_to8);
#endif
#else
#ifdef USE_RG
    sample2d(u_Texture3, v_TexCoords3, 3).r;
#else
    sample2d(u_Texture3, v_TexCoords3, 3).a;
#endif
#endif
    gl_FragColor.rgb = gl_FragColor.rgb*a;
    gl_FragColor.a = a;
#endif //HAS_ALPHA
/***User post processing here***%userPostProcess%***/
}
