#ifndef DIGIKAM_TEXT_CONVERTER_TASK_H
#define DIGIKAM_TEXT_CONVERTER_TASK_H

// Qt includes

#include <QUrl>
#include <QString>

// Local includes

#include "actionthreadbase.h"
#include "textconverteraction.h"

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

class TextConverterTask : public ActionJob
{
    Q_OBJECT

public:

    explicit TextConverterTask(QObject* const parent,
                              const QUrl& url,
                              const TextConverterAction& action
                              );
    ~TextConverterTask();

    void setLanguagesMode(int mode);
    void setPSMMode(int mode);
    void setOEMMode(int mode);
    void setDpi(int value);
    void setIsSaveTextFile(bool check);

Q_SIGNALS:

    void signalStarting(const DigikamGenericTextConverterPlugin::TextConverterActionData& ad);
    void signalFinished(const DigikamGenericTextConverterPlugin::TextConverterActionData& ad);

public Q_SLOTS:

    void slotCancel();

protected:

    void run();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericTextConverterPlugin

#endif // DIGIKAM_Text_CONVERTER_TASK_H

