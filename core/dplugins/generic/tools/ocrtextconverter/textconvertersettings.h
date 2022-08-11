#ifndef DIGIKAM_TEXT_CONVERTER_SETTINGS_H
#define DIGIKAM_TEXT_CONVERTER_SETTINGS_H

// Qt includes

#include <QWidget>
#include <QString>

// KDE includes

#include "digikam_export.h"
#include "kconfiggroup.h"

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

class TextConverterSettings  : public QWidget
{
    Q_OBJECT

public:

    explicit TextConverterSettings(QWidget* const parent = nullptr);
    ~TextConverterSettings() override;

    void setLanguagesMode(int mode);
    int  LanguagesMode()        const;

    void setPSMMode(int mode);
    int  PSMMode()              const;

    void setOEMMode(int mode);
    int  OEMMode()              const;

    void setDpi(int value);
    int  Dpi()                  const;

    void setIsSaveTextFile(bool check);
    bool isSaveTextFile() const;

    void setDefaultSettings();

public Q_SLOTS:

    /// To handle changes from host application Setup dialog.
    void slotSetupChanged();

Q_SIGNALS:

    void signalSettingsChanged();
    void signalSetupExifTool();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericTextConverterPlugin

#endif // DIGIKAM_TEXT_CONVERTER_SETTINGS_H
