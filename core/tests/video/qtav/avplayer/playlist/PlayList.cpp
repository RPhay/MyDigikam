/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "PlayList.h"

// Qt includes

#include <QFileDialog>
#include <QListView>
#include <QLayout>
#include <QToolButton>
#include <QFile>
#include <QDataStream>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "PlayListModel.h"
#include "PlayListDelegate.h"
#include "common.h"
#include "digikam_debug.h"

namespace AVPlayer
{

PlayList::PlayList(QWidget* const parent)
    : QWidget(parent)
{
    mFirstShow             = true;
    mMaxRows               = -1;
    mpModel                = new PlayListModel(this);
    mpDelegate             = new PlayListDelegate(this);
    mpListView             = new QListView;
/*
    mpListView->setResizeMode(QListView::Adjust);
*/
    mpListView->setModel(mpModel);
    mpListView->setItemDelegate(mpDelegate);
    mpListView->setSelectionMode(QAbstractItemView::ExtendedSelection); // ctrl,shift
    mpListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mpListView->setToolTip(i18nc("@info", "Use Ctrl/Shift + Click to select multiple items"));
    QVBoxLayout* const vbl = new QVBoxLayout;
    setLayout(vbl);
    vbl->addWidget(mpListView);
    QHBoxLayout* const hbl = new QHBoxLayout;

    mpClear                = new QToolButton(nullptr);
    mpClear->setText(i18nc("@action: clear playlist", "Clear"));
    mpRemove               = new QToolButton(nullptr);
    mpRemove->setText(QString::fromLatin1("-"));
    mpRemove->setToolTip(i18nc("@info", "Remove selected items"));
    mpAdd                  = new QToolButton(nullptr);
    mpAdd->setText(QString::fromLatin1("+"));

    hbl->addWidget(mpClear);
    hbl->addSpacing(width());
    hbl->addWidget(mpRemove);
    hbl->addWidget(mpAdd);
    vbl->addLayout(hbl);

    connect(mpClear, SIGNAL(clicked()),
            this, SLOT(clearItems()));

    connect(mpRemove, SIGNAL(clicked()),
            this, SLOT(removeSelectedItems()));

    connect(mpAdd, SIGNAL(clicked()),
            this, SLOT(addItems()));

    connect(mpListView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(onAboutToPlay(QModelIndex)));

    // enter to highight
/*
    connect(mpListView, SIGNAL(entered(QModelIndex)),
            this, SLOT(highlight(QModelIndex)));
*/
}

PlayList::~PlayList()
{
    save();
}

void PlayList::setSaveFile(const QString& file)
{
    mFile = file;
}

QString PlayList::saveFile() const
{
    return mFile;
}

void PlayList::load()
{
    QFile f(mFile);

    if (!f.exists())
        return;

    if (!f.open(QIODevice::ReadOnly))
        return;

    QDataStream ds(&f);
    QList<PlayListItem> list;
    ds >> list;

    for (int i = 0 ; i < list.size() ; ++i)
    {
        insertItemAt(list.at(i), i);
    }
}

void PlayList::save()
{
    QFile f(mFile);

    if (!f.open(QIODevice::WriteOnly))
    {
        qCWarning(DIGIKAM_AVPLAYER_LOG).noquote()
            << QString::asprintf("File open error: %s",
                qPrintable(f.errorString()));

        return;
    }

    QDataStream ds(&f);
    ds << mpModel->items();
}

PlayListItem PlayList::itemAt(int row)
{
    if (mpModel->rowCount() < 0)
    {
        qCWarning(DIGIKAM_AVPLAYER_LOG).noquote()
            << QString::asprintf("Invalid rowCount");

        return PlayListItem();
    }

    return (mpModel->data(mpModel->index(row), Qt::DisplayRole).value<PlayListItem>());
}

void PlayList::insertItemAt(const PlayListItem& item, int row)
{
    if ((mMaxRows > 0) && (mpModel->rowCount() >= mMaxRows))
    {
        // +1 because new row is to be inserted

        mpModel->removeRows(mMaxRows, mpModel->rowCount() - mMaxRows + 1);
    }

    int i = mpModel->items().indexOf(item, row+1);

    if (i > 0)
    {
        mpModel->removeRow(i);
    }

    if (!mpModel->insertRow(row))
        return;

    if (row > 0)
    {
        i = mpModel->items().lastIndexOf(item, row - 1);

        if (i >= 0)
            mpModel->removeRow(i);
    }

    setItemAt(item, row);
}

void PlayList::setItemAt(const PlayListItem& item, int row)
{
    mpModel->setData(mpModel->index(row), QVariant::fromValue(item), Qt::DisplayRole);
}

void PlayList::insert(const QString& url, int row)
{
    PlayListItem item;
    item.setUrl(url);
    item.setDuration(0);
    item.setLastTime(0);
    QString title = url;

    if (!url.contains(QLatin1String("://")) || url.startsWith(QLatin1String("file://")))
    {
        title = QFileInfo(url).fileName();
    }

    item.setTitle(title);
    insertItemAt(item, row);
}

void PlayList::remove(const QString& url)
{
    for (int i = (mpModel->rowCount() - 1) ; i >= 0 ; --i)
    {
        PlayListItem item = mpModel->data(mpModel->index(i), Qt::DisplayRole).value<PlayListItem>();

        if (item.url() == url)
        {
            mpModel->removeRow(i);
        }
    }
}

void PlayList::setMaxRows(int r)
{
    mMaxRows = r;
}

int PlayList::maxRows() const
{
    return mMaxRows;
}

void PlayList::removeSelectedItems()
{
    QItemSelectionModel* const selection = mpListView->selectionModel();

    if (!selection->hasSelection())
        return;

    QModelIndexList s = selection->selectedIndexes();

    for (int i = (s.size() - 1) ; i >= 0 ; --i)
    {
        mpModel->removeRow(s.at(i).row());
    }
}

void PlayList::clearItems()
{
    mpModel->removeRows(0, mpModel->rowCount());
}

void PlayList::addItems()
{
    // TODO: add url

    QStringList files = QFileDialog::getOpenFileNames(nullptr, i18nc("@title:window", "Select Files"));

    if (files.isEmpty())
        return;

    // TODO: check playlist file: m3u, pls... In another thread

    for (int i = 0 ; i < files.size() ; ++i)
    {
        QString file = files.at(i);

        if (!QFileInfo(file).isFile())
            continue;

        insert(file, i);
    }
}

void PlayList::onAboutToPlay(const QModelIndex& index)
{
    Q_EMIT aboutToPlay(index.data(Qt::DisplayRole).value<PlayListItem>().url());

    save();
}

} // namespace AVPlayer
