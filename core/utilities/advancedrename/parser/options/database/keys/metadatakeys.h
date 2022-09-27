/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-05-22
 * Description : metadata information keys
 *
 * SPDX-FileCopyrightText: 2009-2012 by Andi Clemens <andi dot clemens at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_METADATA_KEYS_H
#define DIGIKAM_METADATA_KEYS_H

// Local includes

#include "dbkeyscollection.h"
#include "parsesettings.h"

namespace Digikam
{

class MetadataKeys : public DbKeysCollection
{

public:

    explicit MetadataKeys();
    ~MetadataKeys() override {};

protected:

    QString getDbValue(const QString& key, ParseSettings& settings) override;

private:

    // Disable
    MetadataKeys(const MetadataKeys&)            = delete;
    MetadataKeys& operator=(const MetadataKeys&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_METADATA_KEYS_H
