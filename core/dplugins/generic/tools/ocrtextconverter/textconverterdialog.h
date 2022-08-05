#ifndef DIGIKAM_TEXT_CONVERTER_DIALOG_H
#define DIGIKAM_TEXT_CONVERTER_DIALOG_H

// Qt includes

#include <QUrl>
#include <>

// Local includes

#include "dplugindialog.h"
#include "dinfointerface.h"

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

class TextConverterActionData;

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
    void processingFailed(const QUrl& url, int result);  // TODO     
    void processed(const QUrl& url);                     // TODO  
                                 
/**

**/

private Q_SLOTS:

    void slotDefault(); 
    void slotClose();
    void slotStartStop();   
    void slotAborted();
    void slotThreadFinished(); 

    void slotTextConverterAction(const DigikamGenericTextConverterPlugin::TextConverterActionData&);
    void slotDoubleClick(QTreeWidgetItem* element, int i);

    /**
     * Set Identity and Target file.
     */ 

    void slotSetupExifTool();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericTextConverterPlugin

#endif // DIGIKAM_TEXT_CONVERTER_DIALOG_H
