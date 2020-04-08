#ifndef DIGIKAM_ITEM_SELECTION_PROPERTIES_TAB_H
#define DIGIKAM_ITEM_SELECTION_PROPERTIES_TAB_H

//Qt includes

//Local includes
#include "dexpanderbox.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT ItemSelectionPropertiesTab : public DExpanderBox
{
public:

    explicit ItemSelectionPropertiesTab(QWidget* const parent);
    ~ItemSelectionPropertiesTab();
    // void setSelectionFileSize();    //what will this need? See from status bar.

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif //DIGIKAM_ITEM_SELECTION_PROPERTIES_TAB_H