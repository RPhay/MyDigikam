/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * https://www.digikam.org
 *
 * Date        : 2022-08-26
 * Description : file list view and items
 *
 * Copyright (C) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2022      by Quoc Hung Tran <quochungtran1999 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "textconverterlist.h"

// Qt includes

#include <QFileInfo>
#include <QDir>

// KDE includes

#include <klocalizedstring.h>

// LibKDcraw includes

#include "drawdecoder.h"

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

// -------------------------------------------------------------------------------------
// -------------------------------- Text Converter List --------------------------------
// -------------------------------------------------------------------------------------

TextConverterList::TextConverterList(QWidget* const parent)
    : DItemsList(parent)
{
    setControlButtonsPlacement(DItemsList::ControlButtonsBelow);

    listView()->setColumn(static_cast<DItemsListView::ColumnType>(RECOGNIZEDWORDS), i18n("Recognized Words"),  true);
    listView()->setColumn(static_cast<DItemsListView::ColumnType>(TARGETFILENAME),  i18n("Target File"),       true);
    listView()->setColumn(static_cast<DItemsListView::ColumnType>(IDENTIFICATION),  i18n("Camera"),            true);
    listView()->setColumn(static_cast<DItemsListView::ColumnType>(STATUS),          i18n("Status"),            true);
}

TextConverterList::~TextConverterList()
{
}


void TextConverterList::slotAddImages(const QList<QUrl>& list)
{
    if (list.count() == 0)
    {
        return;
    }

    QList<QUrl> urls;
    bool raw = false;

    for (auto const& imageUrl: list)
    {

        // Check if the new item already exist in the list.

        bool found    = false;

        for (int i = 0 ; i < listView()->topLevelItemCount() ; ++i)
        {
            TextConverterListViewItem* const currItem = dynamic_cast<TextConverterListViewItem*>(listView()->topLevelItem(i));

            if (currItem && (currItem->url() == imageUrl))
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            if (DRawDecoder::isRawFile(imageUrl))
            {
                raw = true;
                continue;
            }

            new TextConverterListViewItem(listView(), imageUrl);
            urls.append(imageUrl);
        }
    }

    Q_EMIT signalAddItems(urls);
    Q_EMIT signalFoundRAWImages(raw);
    Q_EMIT signalImageListChanged();
}


void TextConverterList::slotRemoveItems()
{
    bool find = false;

    do
    {
        find = false;
        QTreeWidgetItemIterator it(listView());

        while (*it)
        {
            TextConverterListViewItem* const item = dynamic_cast<TextConverterListViewItem*>(*it);

            if (item && item->isSelected())
            {
                delete item;
                find = true;
                break;
            }

            ++it;
        }
    }
    while(find);
}


// --------------------------------------------------------------------------------------
// ---------------------------- Text Converter List View Item ---------------------------
// --------------------------------------------------------------------------------------

class TextConverterListViewItem::Private
{
public:

    Private()
    {
    }

    QString destFileName;
    QString recognizedWords;
    QString identity;
    QString status;
};

TextConverterListViewItem::TextConverterListViewItem(DItemsListView* const view, const QUrl& url)
    : DItemsListViewItem(view, url),
      d                 (new Private)
{
}

TextConverterListViewItem::~TextConverterListViewItem()
{
    delete d;
}


void TextConverterListViewItem::setDestFileName(const QString& str)
{
    d->destFileName = str;
    setText(TextConverterList::TARGETFILENAME, d->destFileName);
}

QString TextConverterListViewItem::destFileName() const
{
    return d->destFileName;
}

void TextConverterListViewItem::setRecognizedWords(const QString& str)
{
    d->recognizedWords = str;
    setText(TextConverterList::RECOGNIZEDWORDS, d->recognizedWords);
}

QString TextConverterListViewItem::recognizedWords() const
{
    return d->recognizedWords;
}


void TextConverterListViewItem::setIdentity(const QString& str)
{
    d->identity = str;
    setText(TextConverterList::IDENTIFICATION, d->identity);
}

QString TextConverterListViewItem::identity() const
{
    return d->identity;
}

void TextConverterListViewItem::setStatus(const QString& str)
{
    d->status = str;
    setText(TextConverterList::STATUS, d->status);
}

QString TextConverterListViewItem::destPath() const
{
    return (QDir::fromNativeSeparators(QFileInfo(url().toLocalFile()).path() + QLatin1String("/") + destFileName()));
}

} // namespace DigikamGenericTextConverterPlugin
