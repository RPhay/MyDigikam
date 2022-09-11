/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-05-22
 * Description : common information keys
 *
 * SPDX-FileCopyrightText: 2009-2012 by Andi Clemens <andi dot clemens at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_COMMON_KEYS_H
#define DIGIKAM_COMMON_KEYS_H

// Local includes

#include "dbkeyscollection.h"
#include "parsesettings.h"

namespace Digikam
{

class CommonKeys : public DbKeysCollection
{
public:

    explicit CommonKeys();
    ~CommonKeys() override {};

protected:

    QString getDbValue(const QString& key, ParseSettings& settings) override;

private:

    // Disable
    CommonKeys(const CommonKeys&)            = delete;
    CommonKeys& operator=(const CommonKeys&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_COMMON_KEYS_H
