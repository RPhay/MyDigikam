#ifndef DIGIKAM_ITEM_SELECTION_PROPERTIES_TAB_H
#define DIGIKAM_ITEM_SELECTION_PROPERTIES_TAB_H

//Qt includes
#include <QUrl>

//Local includes
#include "dexpanderbox.h"
#include "digikam_export.h"

namespace Digikam
{

class ItemInfoList;

class DIGIKAM_EXPORT ItemSelectionPropertiesTab : public DExpanderBox
{
public:

    explicit ItemSelectionPropertiesTab(QWidget* const parent);
    ~ItemSelectionPropertiesTab();

    void setDisabled();
    void setSelectionSize(const QString &str);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif //DIGIKAM_ITEM_SELECTION_PROPERTIES_TAB_H