#include "textconvertertask.h"
#include "ocrtesseractengine.h"
#include "ocroptions.h"

// Qt includes  

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

class TextConverterTask::Private

{
public: 

    Private()
      : language(int(OcrOptions::Languages::DEFAULT)),
        psm(int(OcrOptions::PageSegmentationModes::DEFAULT)),
        oem(int(OcrOptions::EngineModes::DEFAULT)),
        cancel(false)
    {
    }

    int language;
    int psm; 
    int oem;

    bool                 cancel;         

    QUrl                 url;
    TextConverterAction  action; 

    OcrTesseracrEngine   ocrEngine;
};

TextConverterTask::TextConverterTask(QObject* const parent, 
                                     const QUrl& fileUrl,
                                     const TextConverterAction& action)
    : ActionJob(parent),
      d        (new Private)
{
    d->url    = fileUrl;
    d->action = action;
}

TextConverterTask::~TextConverterTask()
{
//    slotCancel();
    delete d;
}


void TextConverterTask::setLanguagesMode(int mode)
{
    d->language = mode;
}

void TextConverterTask::setPSMMode(int mode)
{
    d->psm = mode;
}

void TextConverterTask::setOEMMode(int mode)
{
    d->oem = mode;
}

void TextConverterTask::run()
{

    if (d->cancel)
    {
        return; 
    }

    switch (d->action)
    {
        case PROCESS: 
        {   
            TextConverterActionData ad1;
            ad1.action    = PROCESS;
            ad1.fileUrl   = d->url;
            ad1.starting  = true;
            Q_EMIT signalStarting(ad1);

            // TODO dest Path

            // TODO set Ouput File 

            d->ocrEngine.setInputFile(d->url.toLocalFile());
            d->ocrEngine.setLanguagesMode(d->language);
            d->ocrEngine.setPSMMode(d->psm);
            d->ocrEngine.setOEMMode(d->oem);
            int ret = d->ocrEngine.runOcrProcess();  

            TextConverterActionData ad2;
            ad2.action     = PROCESS;
            ad2.fileUrl    = d->url;
            // ad2.destPath = destPath;
            ad2.result     = ret;
            ad2.outputText = d->ocrEngine.outputText();

//            qDebug() << d->ocrEngine.outputText();
            Q_EMIT signalFinished(ad2);
            break;
        }
    
        default:
        {
            qCritical() << "Unknown action specified";
            break;
        }    
    }

    Q_EMIT signalDone();

}

void TextConverterTask::slotCancel()
{
    d->cancel = true;
}


} // namespace DigikamGenericTextConverterPlugin
