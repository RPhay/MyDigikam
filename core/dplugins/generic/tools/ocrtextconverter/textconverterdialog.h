#ifndef DIGIKAM_TEXT_CONVERTER_DIALOG_H
#define DIGIKAM_TEXT_CONVERTER_DIALOG_H

// Qt includes

#include <QUrl>

// Local includes

#include "dplugindialog.h"
#include "dinfointerface.h"

using namespace Digikam;

class TextConverterDialog : public DPluginDialog
{

    Q_OBJECT

public:

    explicit TextConverterDialog(QWidget* const parent, DInfoInterface* const iface);
    ~TextConverterDialog() override;

    void addItems(const QList<QUrl>& itemList);

protected:

    void closeEvent(QCloseEvent* e);

public Q_SLOTS:

    void slotSettingsChanged();

private: 

    void saveSettings();
    void readSettings();

    QMap<QString, QString> getValidValues(const QString& opt);
    void setupTesseractOptionsView();

private:

    class Private;
    Private* const d;
};


#endif // DIGIKAM_TEXT_CONVERTER_DIALOG_H
