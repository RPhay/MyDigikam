#ifndef DIGIKAM_OCR_TEXT_CONVERTER_PLUGIN_H
#define DIGIKAM_OCR_TEXT_CONVERTER_PLUGIN_H

// Local includes

#include "dplugingeneric.h"

#define DPLUGIN_IID "org.kde.digikam.plugin.generic.TextConverter"

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

class TextConverterPlugin : public DPluginGeneric
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DPLUGIN_IID)
    Q_INTERFACES(Digikam::DPluginGeneric)

public:

    explicit TextConverterPlugin(QObject* const parent = nullptr);
    ~TextConverterPlugin()                override;

    QString name()                 const override;
    QString iid()                  const override;
    QIcon   icon()                 const override;
    QString details()              const override;
    QString description()          const override;
    QList<DPluginAuthor> authors() const override;

    void setup(QObject* const)           override;

private Q_SLOTS:

    void slotTextConverter();
};

} // namespace DigikamGenericTextConverterPlugin

#endif // DIGIKAM_OCR_TEXT_CONVERTER_PLUGIN_H
