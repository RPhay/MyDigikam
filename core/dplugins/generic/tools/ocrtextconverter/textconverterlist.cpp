#include "textconverterlist.h"

// Qt includes

#include <QFileInfo>
#include <QDir>

// KDE includes

#include <klocalizedstring.h>

// LibKDcraw includes

#include "drawdecoder.h"

// -------------------------------------------------------------------------------------
// -------------------------------- Text Converter List --------------------------------
// -------------------------------------------------------------------------------------

TextConverterList::TextConverterList(QWidget* const parent)
    : DItemsList(parent)
{
    setControlButtonsPlacement(DItemsList::ControlButtonsBelow);
    listView()->setColumnLabel(DItemsListView::Filename, i18n("Raw File"));
    listView()->setColumn(static_cast<DItemsListView::ColumnType>(IDENTIFICATION), i18n("Camera"),      true);
    listView()->setColumn(static_cast<DItemsListView::ColumnType>(STATUS),         i18n("Status"),      true);
}

TextConverterList::~TextConverterList()
{
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