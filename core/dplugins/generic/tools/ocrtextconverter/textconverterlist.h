#ifndef DIGIKAM_TEXT_CONVERTER_LIST_H
#define DIGIKAM_TEXT_CONVERTER_LIST_H

// Qt includes

#include <QUrl>

// Local includes

#include "ditemslist.h"

using namespace Digikam;


class TextConverterList : public DItemsList
{
    Q_OBJECT

public:

    /**
     * List the different columns in a list.
     */
    enum FieldType
    {
        IDENTIFICATION = DItemsListView::User1,
        STATUS         = DItemsListView::User2
    };

public:

    explicit TextConverterList(QWidget* const parent = nullptr);
    ~TextConverterList();

protected Q_SLOTS:

    void slotRemoveItems()                      override;
};

// -------------------------------------------------------------------------

class TextConverterListViewItem : public DItemsListViewItem
{

public:

    TextConverterListViewItem(DItemsListView* const view, const QUrl& url);
    ~TextConverterListViewItem();

    void setIdentity(const QString& str);
    QString identity()      const;

    void setStatus(const QString& str);
    
private:

    class Private;
    Private* const d;
};


#endif // DIGIKAM_TEXT_CONVERTER_LIST_H
