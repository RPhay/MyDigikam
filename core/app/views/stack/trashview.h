/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-08-07
 * Description : Trash view
 *
 * SPDX-FileCopyrightText: 2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QWidget>
#include <QStyledItemDelegate>

namespace Digikam
{

class DTrashItemModel;
class ThumbnailSize;

class TrashView : public QWidget
{
    Q_OBJECT

public:

    explicit TrashView(QWidget* const parent = nullptr);
    ~TrashView()                              override;

    /**
     * @return model used for the view
     */
    DTrashItemModel* model()            const;

    /**
     * @return current thumbnail size
     */
    ThumbnailSize getThumbnailSize()    const;

    /**
     * @brief set thumbnail size to give to model
     * @param thumbSize: size to set
     */
    void setThumbnailSize(const ThumbnailSize& thumbSize);

    /**
     * @return QUrl to the last selected item in view
     */
    QUrl lastSelectedItemUrl()          const;

    /**
     * @brief Highlights the last selected item when the view gets focus
     */
    void selectLastSelected();

    /**
     * @return text for the main status bar
     */
    QString statusBarText()             const;

private Q_SLOTS:

    void slotSelectionChanged();
    void slotUndoLastDeletedItems();
    void slotRestoreSelectedItems();
    void slotDeleteSelectedItems();
    void slotRemoveItemsFromModel();
    void slotRemoveAllItemsFromModel();
    void slotDeleteAllItems();
    void slotDataChanged();
    void slotLoadingStarted();
    void slotLoadingFinished();
    void slotChangeLastSelectedItem(const QModelIndex& curr, const QModelIndex& prev);
    void slotContextMenuEmptyTrash(const QPoint& pos);

Q_SIGNALS:

    void selectionChanged();
    void signalEmptytrash();

private:

    class Private;
    Private* const d = nullptr;
};

// --------------------------------------------------

class ThumbnailAligningDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:

    explicit ThumbnailAligningDelegate(QObject* const parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

} // namespace Digikam
