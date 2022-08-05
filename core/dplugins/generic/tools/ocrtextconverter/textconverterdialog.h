#ifndef DIGIKAM_TEXT_CONVERTER_DIALOG_H
#define DIGIKAM_TEXT_CONVERTER_DIALOG_H

// Qt includes

#include <QUrl>

// Local includes

#include "dplugindialog.h"
#include "dinfointerface.h"

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

// TODO TextConverterActionData

class TextConverterDialog : public DPluginDialog
{

    Q_OBJECT

public:

    explicit TextConverterDialog(QWidget* const parent, DInfoInterface* const iface);
    ~TextConverterDialog() override;

   void addItems(const QList<QUrl>& itemList);

protected:

    void closeEvent(QCloseEvent* e);  

private: 

    void readSettings();
    void saveSettings();

    void busy(bool busy);

    void processAll();                                        
/**
    void processed(const QUrl& url, const QString& tmpFile);  // TODO
    void processingFailed(const QUrl& url, int result);       // TODO 

**/

private Q_SLOTS:

    void slotDefault(); 
    void slotClose();
    void slotAborted();
    void slotTextConverterAction(const QString& result);

    /**
     * Set Identity and Target file.
     */ 

    void slotSetupExifTool();
    void slotStartStop();   

/**
    void slotThreadFinished();  // TODO
    void slotIdentify();        // TODO

**/

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericTextConverterPlugin

#endif // DIGIKAM_TEXT_CONVERTER_DIALOG_H
