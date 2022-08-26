#ifndef DIGIKAM_TEXT_CONVERTER_ACTIONS_H
#define DIGIKAM_TEXT_CONVERTER_ACTIONS_H

// Qt includes

#include <QString>
#include <QImage>
#include <QMetaType>
#include <QUrl>

// Local includes

#include "ocrtesseractengine.h"

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

enum TextConverterAction
{
    NONE = 0,
    PROCESS
};

class TextConverterActionData
{

public:

    TextConverterActionData()
      : starting(false),
        result  (OcrTesseracrEngine::PROCESS_COMPLETE),
        action  (NONE)
    {
    }

    bool                 starting;
    int                  result;

    QString              destPath;
    QString              message;

    QString              outputText;

    QImage               image;

    QUrl                 fileUrl;

    TextConverterAction  action;
};

}  // namespace DigikamGenericTextConverterPlugin

Q_DECLARE_METATYPE(DigikamGenericTextConverterPlugin::TextConverterActionData)

#endif /// DIGIKAM_TEXT_CONVERTER_ACTIONS_H
