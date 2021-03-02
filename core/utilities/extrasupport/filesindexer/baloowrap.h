/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-08-05
 * Description : Plasma file indexer interface.
 *
 * Copyright (C) 2014 by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_BALOO_WRAP_H
#define DIGIKAM_BALOO_WRAP_H

// Qt includes

#include <QObject>
#include <QStringList>
#include <QPointer>

// Local includes

#include "digikam_export.h"

class QUrl;

namespace Digikam
{

class ItemInfo;

class BalooInfo
{
public:

    BalooInfo()
      : rating(-1)
    {
    }

    QStringList tags;
    QString     comment;
    int         rating;
};

/**
 * @brief The BalooWrap class is a singleton class which offer
 *        functionality for reading and writing image
 *        comment, tags and rating from Baloo to digiKam
 *        and from digiKam to Baloo
 *
 *        The singleton functionality is required because
 *        it also watches for changes in Baloo and notify
 *        digiKam, so it could trigger a scan
 */
class DIGIKAM_EXPORT BalooWrap : public QObject
{
    Q_OBJECT

public:

    BalooWrap();
    ~BalooWrap() override;

    /**
     * @brief internalPtr - singleton implementation
     */
    static QPointer<BalooWrap> internalPtr;
    static BalooWrap*          instance();
    static bool                isCreated();

    /**
     * @brief setSemanticInfo - generic method to set all data from digiKam to Baloo
     * @param url             - image url
     * @param bInfo           - container class for tags, comment, rating
     */
    void setSemanticInfo(const QUrl& url, const BalooInfo& bInfo);

    /**
     * @brief getSemanticInfo - used by ItemScanner to retrieve all information
     *                          tags, comment, rating
     * @param url             - image url
     * @return                - container class for tags, comment, rating
     */
    BalooInfo getSemanticInfo(const QUrl& url) const;

    void setSyncToBaloo(bool value);

    bool getSyncToBaloo()                      const;

    void setSyncToDigikam(bool value);

    bool getSyncToDigikam()                    const;

private:

    // Disable
    explicit BalooWrap(QObject*);

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_BALOO_WRAP_H
