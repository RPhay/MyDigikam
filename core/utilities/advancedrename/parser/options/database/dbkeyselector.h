/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-05-22
 * Description : database key selector.
 *
 * SPDX-FileCopyrightText: 2010-2012 by Andi Clemens <andi dot clemens at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DB_KEY_SELECTOR_H
#define DIGIKAM_DB_KEY_SELECTOR_H

// Qt includes

#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QStringList>

// Local includes

#include "searchtextbar.h"
#include "databaseoption.h"

namespace Digikam
{
class DbHeaderListItem;

class DbKeySelectorItem : public QTreeWidgetItem
{

public:

    explicit DbKeySelectorItem(DbHeaderListItem* const parent, const QString& title, const QString& desc);
    ~DbKeySelectorItem() override;

    QString key()         const;
    QString description() const;

private:

    QString m_key;
    QString m_description;

private:

    // Disable
    DbKeySelectorItem(const DbKeySelectorItem&)            = delete;
    DbKeySelectorItem& operator=(const DbKeySelectorItem&) = delete;
};

// ------------------------------------------------------------------------------------

class DbKeySelector : public QTreeWidget
{
    Q_OBJECT

public:

    explicit DbKeySelector(QWidget* const parent);
    ~DbKeySelector() override;

    void setKeysMap(const DbOptionKeysMap& map);
    QStringList checkedKeysList();

private:

    // Disable
    DbKeySelector(const DbKeySelector&)            = delete;
    DbKeySelector& operator=(const DbKeySelector&) = delete;
};

// ------------------------------------------------------------------------------------

class DbKeySelectorView : public QWidget
{
    Q_OBJECT

public:

    explicit DbKeySelectorView(QWidget* const parent);
    ~DbKeySelectorView() override;

    void setKeysMap(const DbOptionKeysMap& map);
    QStringList checkedKeysList() const;

private Q_SLOTS:

    void slotSearchTextChanged(const SearchTextSettings&);

private:

    void removeChildlessHeaders();

private:

    // Disable
    DbKeySelectorView(const DbKeySelectorView&)            = delete;
    DbKeySelectorView& operator=(const DbKeySelectorView&) = delete;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DB_KEY_SELECTOR_H
