/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-03-20
 * Description : Simple virtual interface for ItemLister
 *
 * Copyright (C) 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2007-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2015      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * Copyright (C) 2007-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_ITEM_LISTER_RECEIVER_H
#define DIGIKAM_ITEM_LISTER_RECEIVER_H

// Qt includes

#include <QString>
#include <QList>

// Local includes

#include "digikam_export.h"
#include "itemlisterrecord.h"
#include "dbjob.h"

namespace Digikam
{

class DIGIKAM_DATABASE_EXPORT ItemListerReceiver
{
public:

    ItemListerReceiver()                                 = default;
    virtual ~ItemListerReceiver()                        = default;

    virtual void receive(const ItemListerRecord& record) = 0;
    virtual void error(const QString& /*errMsg*/) {};

private:

    Q_DISABLE_COPY(ItemListerReceiver)
};

// ------------------------------------------------------------------------------------------------

class DIGIKAM_DATABASE_EXPORT ItemListerValueListReceiver
    : public ItemListerReceiver
{
public:

    explicit ItemListerValueListReceiver();

    void receive(const ItemListerRecord& record) override;
    void error(const QString& errMsg) override;

public:

    QList<ItemListerRecord> records;
    bool                    hasError;

private:

    Q_DISABLE_COPY(ItemListerValueListReceiver)
};

// ------------------------------------------------------------------------------------------------

class DIGIKAM_DATABASE_EXPORT ItemListerJobReceiver
    : public ItemListerValueListReceiver
{
public:

    explicit ItemListerJobReceiver(DBJob* const job);

    void error(const QString& errMsg) override;
    void sendData();

protected:

    DBJob* const m_job;

private:

    Q_DISABLE_COPY(ItemListerJobReceiver)
};

// ------------------------------------------------------------------------------------------------

class DIGIKAM_DATABASE_EXPORT ItemListerJobPartsSendingReceiver
    : public ItemListerJobReceiver
{
public:

    explicit ItemListerJobPartsSendingReceiver(DBJob* const job, int limit);

    void receive(const ItemListerRecord &record) override;

protected:

    int m_limit;
    int m_count;

private:

    Q_DISABLE_COPY(ItemListerJobPartsSendingReceiver)
};

// ------------------------------------------------------------------------------------------------

class DIGIKAM_DATABASE_EXPORT ItemListerJobGrowingPartsSendingReceiver
    : public ItemListerJobPartsSendingReceiver
{
public:

    explicit ItemListerJobGrowingPartsSendingReceiver(DBJob* const job,
                                                      int start,
                                                      int end,
                                                      int increment);

    void receive(const ItemListerRecord& record) override;

protected:

    int m_maxLimit;
    int m_increment;

private:

    Q_DISABLE_COPY(ItemListerJobGrowingPartsSendingReceiver)
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_LISTER_RECEIVER_H
