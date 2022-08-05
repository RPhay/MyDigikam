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
        oem(int(OcrOptions::EngineModes::DEFAULT))


    {
    }

    int language;
    int psm; 
    int oem;

    QUrl                 url;

    OcrTesseracrEngine   ocrEngine;
};

TextConverterTask::TextConverterTask(QObject* const parent, const QUrl& fileUrl)
    : ActionJob(parent),
      d        (new Private)
{
    d->url    = fileUrl;
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
    d->ocrEngine.setInputFile(d->url.toLocalFile());
    d->ocrEngine.setLanguagesMode(d->language);
    d->ocrEngine.setPSMMode(d->psm);
    d->ocrEngine.setOEMMode(d->oem);
    int ret = d->ocrEngine.runOcrProcess();

    Q_EMIT signalFinished(d->ocrEngine.outputText());
    
    Q_EMIT signalDone();

    // TODO 
}

} // namespace DigikamGenericTextConverterPlugin
