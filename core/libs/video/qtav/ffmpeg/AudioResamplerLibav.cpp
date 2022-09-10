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

#ifdef QTAV_HAVE_SWR_AVR_MAP
#   undef QTAV_HAVE_SWR_AVR_MAP
#   define QTAV_HAVE_SWR_AVR_MAP 1
#endif

#define BUILD_AVR

#include "AudioResamplerTemplate.cpp"
