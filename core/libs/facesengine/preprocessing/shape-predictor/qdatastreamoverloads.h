/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 16/08/2016
 * Description : Serialization utilities to help making long
 *               serialization platform independent
 *
 * SPDX-FileCopyrightText:      2016 by Omar Amin <Omar dot moh dot amin at gmail dot com>
 * SPDX-FileCopyrightText: 2016-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QDataStream>

// Local includes

#include "digikam_export.h"

DIGIKAM_EXPORT QDataStream& operator >> (QDataStream& dataStream,
                                         unsigned long& in);

DIGIKAM_EXPORT QDataStream& operator << (QDataStream& dataStream,
                                         const unsigned long& in);
