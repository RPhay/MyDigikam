#include "textconverterthread.h"
#include "ocroptions.h"

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "drawinfo.h"
#include "drawdecoder.h"
#include "textconvertertask.h"
#include "dmetadata.h"
#include "digikam_debug.h"

// Qt includes 

#include <QUrl>
#include <QList>
#include <QString>

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

class TextConverterActionThread::Private
{
public:

    Private()
      : language(int(OcrOptions::Languages::DEFAULT)),
        psm(int(OcrOptions::PageSegmentationModes::DEFAULT)),
        oem(int(OcrOptions::EngineModes::DEFAULT)),
        dpi(300),
        isSaveTextFile(true)
    {
    }

    int language;
    int psm; 
    int oem;
    int dpi;    
    bool isSaveTextFile; 
};

TextConverterActionThread::TextConverterActionThread(QObject* const parent)
    : ActionThreadBase(parent),
      d               (new Private)
{
    qRegisterMetaType<TextConverterActionData>();
}

TextConverterActionThread::~TextConverterActionThread()
{
    // cancel the thread

    cancel();

    // wait for the thread to finish

    wait();

    delete d;
}


void TextConverterActionThread::setLanguagesMode(int mode)
{
    d->language = mode;
}


void TextConverterActionThread::setPSMMode(int mode)
{
    d->psm = mode;
}

void TextConverterActionThread::setOEMMode(int mode)
{
    d->oem = mode;
}

void TextConverterActionThread::setDpi(int value)
{
    d->dpi = value;
}

void TextConverterActionThread::setIsSaveTextFile(bool check)
{
    d->isSaveTextFile = check;
}

void TextConverterActionThread::ocrProcessFile(const QUrl& url)
{
    QList<QUrl> oneFile;
    oneFile.append(url);
    ocrProcessFiles(oneFile);
}


void TextConverterActionThread::ocrProcessFiles(const QList<QUrl>& urlList)
{
    ActionJobCollection collection;

    for (QList<QUrl>::const_iterator it = urlList.constBegin() ; it != urlList.constEnd() ; ++it)
    {
        TextConverterTask* const t = new TextConverterTask(this, *it, PROCESS);
        t->setLanguagesMode(d->language);
        t->setPSMMode(d->psm);
        t->setOEMMode(d->oem);
        t->setDpi(d->dpi);
        t->setIsSaveTextFile(d->isSaveTextFile);

        connect(t, SIGNAL(signalStarting(DigikamGenericTextConverterPlugin::TextConverterActionData)),
                this, SIGNAL(signalStarting(DigikamGenericTextConverterPlugin::TextConverterActionData)));

        connect(t, SIGNAL(signalFinished(DigikamGenericTextConverterPlugin::TextConverterActionData)),
                this, SIGNAL(signalFinished(DigikamGenericTextConverterPlugin::TextConverterActionData)));

        connect(this, SIGNAL(signalCancelTextConverterTask()),
                t, SLOT(slotCancel()), Qt::QueuedConnection);

        
        collection.insert(t, 0);
    }

    appendJobs(collection);
}

void TextConverterActionThread::cancel()
{
    if (isRunning())
    {
        Q_EMIT signalCancelTextConverterTask();
    }

    ActionThreadBase::cancel();
}

}  // namespace DigikamGenericTextConverterPlugin
