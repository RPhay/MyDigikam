/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-10-09
 * Description : Libraw config header
 *
 * Copyright (C) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef LIBRAW_CONFIG_H
#define LIBRAW_CONFIG_H

/* Define to 1 if LibRaw have been compiled with DNG lossy codec support */
#cmakedefine LIBRAW_USE_DNGLOSSYCODEC 1

/* Define to 1 if LibRaw have been compiled with RedCine codec support */
#cmakedefine LIBRAW_USE_REDCINECODEC 1

/* To share OpenMp support detected by Libraw at compilation time */
#include "libraw_types.h"

#endif // LIBRAW_CONFIG_H
