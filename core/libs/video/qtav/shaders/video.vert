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

attribute vec4 a_Position;
attribute vec2 a_TexCoords0;
uniform mat4 u_Matrix;
varying vec2 v_TexCoords0;
#ifdef MULTI_COORD
attribute vec2 a_TexCoords1;
attribute vec2 a_TexCoords2;
varying vec2 v_TexCoords1;
varying vec2 v_TexCoords2;
#ifdef HAS_ALPHA
attribute vec2 a_TexCoords3;
varying vec2 v_TexCoords3;
#endif
#endif //MULTI_COORD
/***User header code***%userHeader%***/

void main() {
    gl_Position = u_Matrix * a_Position;
    v_TexCoords0 = a_TexCoords0;
#ifdef MULTI_COORD
    v_TexCoords1 = a_TexCoords1;
    v_TexCoords2 = a_TexCoords2;
#ifdef HAS_ALPHA
    v_TexCoords3 = a_TexCoords3;
#endif
#endif //MULTI_COORD
}
