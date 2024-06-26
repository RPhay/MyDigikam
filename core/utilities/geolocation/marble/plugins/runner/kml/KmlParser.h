// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#pragma once

#include "GeoParser.h"

namespace Marble
{

class KmlParser : public GeoParser
{
public:

    KmlParser();
    ~KmlParser() override;

private:

    bool isValidElement(const QString& tagName) const override;
    bool isValidRootElement() override;

    GeoDocument* createDocument() const override;
};

}
