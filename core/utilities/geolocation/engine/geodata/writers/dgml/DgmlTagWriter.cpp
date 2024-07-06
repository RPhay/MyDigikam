/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "DgmlTagWriter.h"

#include "GeoWriter.h"
#include "DgmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerDgml( GeoTagWriter::QualifiedName( QString::fromUtf8(""), QString::fromUtf8(dgml::dgmlTag_nameSpace20) ), new DgmlTagWriter() );

bool DgmlTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    Q_UNUSED(node);
    writer.writeStartElement( QString::fromUtf8("dgml") );
    writer.writeAttribute( QString::fromUtf8("xmlns"), QString::fromUtf8(dgml::dgmlTag_nameSpace20) );

    // Do not write an end element for document handlers
    return true;
}

} // namespace Marble
