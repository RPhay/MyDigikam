#include "itemselectionpropertiestab.h"

//Qt includes
#include <QApplication>
#include <QGridLayout>
#include <QStyle>
#include <QIcon>

//KDE includes
#include "klocalizedstring.h"

//Local includes
#include "itempropertiestab.h"
#include "itempropertiestxtlabel.h"

namespace Digikam
{
class Q_DECL_HIDDEN ItemSelectionPropertiesTab::Private
{
public:

    enum Section{
        SelectionProperties = 0
    };

public:
  
    explicit Private()
      : labelSelectionSize(nullptr)
    {
    }
    DTextLabelValue* labelSelectionSize;
};

ItemSelectionPropertiesTab::ItemSelectionPropertiesTab(QWidget* const parent)
    : DExpanderBox(parent),
      d(new Private)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setLineWidth(style()->pixelMetric(QStyle::PM_DefaultFrameWidth));

    // --------------------------------------------------

    const int spacing = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    QWidget* const w1 = new QWidget(this);
    QGridLayout* const glay1 = new QGridLayout(w1);

    DTextLabelName* const selectionSize = new DTextLabelName(i18n("Size: "), w1);

    d->labelSelectionSize = new DTextLabelValue(QString(), w1);

    glay1->addWidget(selectionSize, 0, 0, 1, 1);
    glay1->addWidget(d->labelSelectionSize, 0, 1, 1, 1);
    glay1->setContentsMargins(spacing, spacing, spacing, spacing);
    glay1->setColumnStretch(0, 10);
    glay1->setColumnStretch(1, 25);
    glay1->setSpacing(0);

    insertItem(ItemSelectionPropertiesTab::Private::SelectionProperties, w1,
               QIcon::fromTheme(QLatin1String("dialog-information")),
               i18n("Selection Properties"), QLatin1String("Selection Properties"), true);

    addStretch();
}

ItemSelectionPropertiesTab::~ItemSelectionPropertiesTab()
{
    delete d;
}

void ItemSelectionPropertiesTab::setDisabled()
{
        d->labelSelectionSize->setAdjustedText(QString());
        setEnabled(false);
}

void ItemSelectionPropertiesTab::setSelectionSize(const QString &str)
{
    d->labelSelectionSize->setAdjustedText(str);
    setEnabled(true);
}

} // namespace Digikam 