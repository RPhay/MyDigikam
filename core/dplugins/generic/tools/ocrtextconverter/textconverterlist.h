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

#ifndef DIGIKAM_TEXT_CONVERTER_LIST_H
#define DIGIKAM_TEXT_CONVERTER_LIST_H

// Qt includes

#include <QUrl>

// Local includes

#include "ditemslist.h"

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

class TextConverterListViewItem;

class TextConverterList : public DItemsList
{
    Q_OBJECT

public:

    /**
     * List the different columns in a list.
     */
    enum FieldType
    {
        RECOGNIZEDWORDS      = DItemsListView::User1,
        TARGETFILENAME       = DItemsListView::User2,
        STATUS               = DItemsListView::User3,
    };

public:

    explicit TextConverterList(QWidget* const parent = nullptr);
    ~TextConverterList();


public Q_SLOTS:

    void slotAddImages(const QList<QUrl>& list)    override;

protected Q_SLOTS:

    void slotRemoveItems()                         override;
};

// -------------------------------------------------------------------------

class TextConverterListViewItem : public DItemsListViewItem
{

public:

    explicit TextConverterListViewItem(DItemsListView* const view, const QUrl& url);
    ~TextConverterListViewItem();

    void setDestFileName(const QString& str);
    QString destFileName()  const;

    void setRecognizedWords(const QString& str);
    QString recognizedWords()      const;

    void setStatus(const QString& str);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericTextConverterPlugin


#endif // DIGIKAM_TEXT_CONVERTER_LIST_H
