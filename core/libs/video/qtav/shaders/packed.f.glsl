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

uniform sampler2D u_Texture0;
varying vec2 v_TexCoords0;
uniform mat4 u_colorMatrix;
uniform float u_opacity;
uniform mat4 u_c;
/***User header code***%userHeader%***/
/***User sampling function here***%userSample%***/
#ifndef USER_SAMPLER
vec4 sample2d(sampler2D tex, vec2 pos, int plane)
{
    return texture(tex, pos);
}
#endif

void main() {
    vec4 c = sample2d(u_Texture0, v_TexCoords0, 0);
    c = u_c * c;
#ifndef HAS_ALPHA
    c.a = 1.0; // before color mat transform!
#endif //HAS_ALPHA
#ifdef XYZ_GAMMA
    c.rgb = pow(c.rgb, vec3(2.6));
#endif // XYZ_GAMMA
    c = u_colorMatrix * c;
#ifdef XYZ_GAMMA
    c.rgb = pow(c.rgb, vec3(1.0/2.2));
#endif //XYZ_GAMMA
    gl_FragColor = clamp(c, 0.0, 1.0) * u_opacity;
    /***User post processing here***%userPostProcess%***/
}
