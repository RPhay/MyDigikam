/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-06-23
 * Description : a tab to display item editing history
 *
 * SPDX-FileCopyrightText: 2010 by Martin Klapetek <martin dot klapetek at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QWidget>
#include <QAction>
#include <QModelIndex>
#include <QUrl>

// Local includes

#include "digikam_export.h"
#include "dmetadata.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT RemoveFilterAction : public QAction
{
    Q_OBJECT

public:

    explicit RemoveFilterAction(const QString& label,
                                const QModelIndex& index,
                                QObject* const parent = nullptr);
    ~RemoveFilterAction();

    void setIndex(const QModelIndex& index);

public Q_SLOTS:

    void triggerSlot();

Q_SIGNALS:

    void actionTriggered(QModelIndex index);

private:

    QModelIndex m_index;
};

// -------------------------------------------------------------------------------------

class DIGIKAM_GUI_EXPORT ItemPropertiesHistoryTab : public QWidget
{
    Q_OBJECT

public:

    explicit ItemPropertiesHistoryTab(QWidget* const parent);
    ~ItemPropertiesHistoryTab();

    void setCurrentURL(const QUrl& url = QUrl());

public Q_SLOTS:

    void showCustomContextMenu(const QPoint& position);
    void setModelData(const QList<DImageHistory::Entry>& entries);
    void disableEntry(bool disable);

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
