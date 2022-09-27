/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-02-20
 * Description : a widget to display Standard Exif metadata
 *
 * SPDX-FileCopyrightText: 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_EXIF_WIDGET_H
#define DIGIKAM_EXIF_WIDGET_H

// Qt includes

#include <QString>

// Local includes

#include "metadatawidget.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT ExifWidget : public MetadataWidget
{
    Q_OBJECT

public:

    explicit ExifWidget(QWidget* const parent, const QString& name = QString());
    ~ExifWidget()                                   override;

    bool loadFromURL(const QUrl& url)               override;

    QString getTagDescription(const QString& key)   override;
    QString getTagTitle(const QString& key)         override;

    QString getMetadataTitle() const                override;

protected Q_SLOTS:

    void slotSaveMetadataToFile()                   override;

private:

    bool decodeMetadata()                           override;
    void buildView()                                override;

private:

    QStringList m_keysFilter;
};

} // namespace Digikam

#endif // DIGIKAM_EXIF_WIDGET_H
