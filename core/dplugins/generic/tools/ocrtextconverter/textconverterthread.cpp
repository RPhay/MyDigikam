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
        oem(int(OcrOptions::EngineModes::DEFAULT))
    {
    }

    int language;
    int psm; 
    int oem;
};

TextConverterActionThread::TextConverterActionThread(QObject* const parent)
    : ActionThreadBase(parent),
      d               (new Private)
{
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
        TextConverterTask* const t = new TextConverterTask(this, *it);
        t->setLanguagesMode(d->language);
        t->setPSMMode(d->psm);
        t->setOEMMode(d->oem);

        connect(t, SIGNAL(TextConverterTask::signalFinished(const QString)),
                this, SIGNAL(signalFinished(const QString)));
        
        collection.insert(t, 0);
    }

    appendJobs(collection);
}

void TextConverterActionThread::cancel()
{
    ActionThreadBase::cancel();
}

}  // namespace DigikamGenericTextConverterPlugin
