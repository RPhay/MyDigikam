

// Qt includes

#include <QCloseEvent>
#include <QFile>
#include <QFileInfo>
#include <QGridLayout>
#include <QPixmap>
#include <QTimer>
#include <QTreeWidgetItemIterator>
#include <QApplication>
#include <QMessageBox>
#include <QMenu>
#include <QPushButton>
#include <QCursor>
#include <QWidget>
#include <QProcess>

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <klocalizedstring.h>
#include <kprocess.h>

// Local includes

#include "filesaveconflictbox.h"
#include "dcombobox.h"
#include "dprogresswdg.h"
#include "dmetadata.h"
#include "digikam_debug.h"
#include "imagedialog.h"
#include "dexpanderbox.h"
#include "dfileoperations.h"
#include "exiftoolerrorview.h"
#include "exiftoolparser.h"
#include "dnuminput.h"

#include "textconverterlist.h"
#include "textconvertersettings.h"
#include "ocroptions.h"

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

class Q_DECL_HIDDEN TextConverterSettings::Private
{
public:

    explicit Private()
      : ocrTesseractLanguageMode   (nullptr),
        ocrTesseractPSMMode        (nullptr),
        ocrTesseractOEMMode        (nullptr),
        ocrTesseractDpi            (nullptr),
        errorView                  (nullptr)
    {
    }

    // Tesseract options 
    
    DComboBox*          ocrTesseractLanguageMode;
    
    DComboBox*          ocrTesseractPSMMode;

    DComboBox*          ocrTesseractOEMMode;

    DIntNumInput*       ocrTesseractDpi;

    ExifToolErrorView*  errorView;
};


TextConverterSettings::TextConverterSettings(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
     // ------------

    QLabel* const ocrTesseractLanguageLabel   = new QLabel(i18nc("@label", "Languages:"));
    d->ocrTesseractLanguageMode               = new DComboBox(this);
    
    QMap<OcrOptions::Languages, QString>             langMap  = OcrOptions::languagesNames();
    QMap<OcrOptions::Languages, QString>::const_iterator it   = langMap.constBegin();

    while (it != langMap.constEnd())
    {
        d->ocrTesseractLanguageMode->addItem(it.value(), (int)it.key());
        ++it;
    }
    
    d->ocrTesseractLanguageMode->setDefaultIndex(int(OcrOptions::Languages::DEFAULT));
    
    // ------------

    QLabel* const ocrTesseractPSMLabel  = new QLabel(i18nc("@label", "Segmentation mode:"));
    d->ocrTesseractPSMMode              = new DComboBox(this);

    QMap<OcrOptions::PageSegmentationModes, QString>                psmMap = OcrOptions::psmNames();
    QMap<OcrOptions::PageSegmentationModes, QString>::const_iterator it1   = psmMap.constBegin();

    while (it1 != psmMap.constEnd())
    {
        d->ocrTesseractPSMMode->addItem(it1.value(), (int)it1.key());
        ++it1;
    }

    d->ocrTesseractPSMMode->setDefaultIndex(int(OcrOptions::PageSegmentationModes::DEFAULT));

    // ------------
   
    QLabel* const ocrTesseractOEMLabel  = new QLabel(i18nc("@label", "Engine mode:"));
    d->ocrTesseractOEMMode              = new DComboBox(this);
    
    QMap<OcrOptions::EngineModes, QString>                oemMap  = OcrOptions::oemNames();
    QMap<OcrOptions::EngineModes, QString>::const_iterator it2    = oemMap.constBegin();

    while (it2 !=  oemMap.constEnd())
    {
        d->ocrTesseractOEMMode->addItem(it2.value(), (int)it2.key());
        ++it2;
    }

    d->ocrTesseractOEMMode->setDefaultIndex(int(OcrOptions::EngineModes::DEFAULT));

    // ------------

    QLabel* const ocrTesseractDpiLabel  = new QLabel(i18nc("@label", "Resolution Dpi:"));
    d->ocrTesseractDpi                  = new DIntNumInput(this);
    d->ocrTesseractDpi->setDefaultValue(300);
    d->ocrTesseractDpi->setRange(70, 2400, 1);
    d->ocrTesseractDpi->setWhatsThis(i18nc("@info", "Specify DPI for input image."));
    ocrTesseractDpiLabel->setBuddy(d->ocrTesseractDpi);

    // ------------

    QGridLayout* const settingsBoxLayout = new QGridLayout(this);
    settingsBoxLayout->addWidget(ocrTesseractLanguageLabel,        0, 0, 1, 1);
    settingsBoxLayout->addWidget(d->ocrTesseractLanguageMode,      0, 1, 1, 1);
    settingsBoxLayout->addWidget(ocrTesseractPSMLabel,             1, 0, 1, 1);
    settingsBoxLayout->addWidget(d->ocrTesseractPSMMode,           1, 1, 1, 1);
    settingsBoxLayout->addWidget(ocrTesseractOEMLabel,             2, 0, 1, 1);
    settingsBoxLayout->addWidget(d->ocrTesseractOEMMode,           2, 1, 1, 1);
    settingsBoxLayout->addWidget(ocrTesseractDpiLabel,             3, 0, 1, 1);
    settingsBoxLayout->addWidget(d->ocrTesseractDpi,               3, 1, 1, 1);
    settingsBoxLayout->setRowStretch(9, 10);
    settingsBoxLayout->setContentsMargins(QMargins());

    // ------------------------------------------------------------------------
    
    connect(d->ocrTesseractLanguageMode, SIGNAL(activated(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->ocrTesseractPSMMode, SIGNAL(activated(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->ocrTesseractOEMMode, SIGNAL(activated(int)),
            this, SIGNAL(signalSettingsChanged()));

//    connect(d->errorView, SIGNAL(signalSetupExifTool()),
//            this, SIGNAL(signalSetupExifTool()));

    // ------------------------------------------------------------------------

//    slotSetupChanged();

}   

TextConverterSettings::~TextConverterSettings()
{
    delete d;
}

void TextConverterSettings::setDefaultSettings()
{
    d->ocrTesseractLanguageMode->slotReset();
    d->ocrTesseractOEMMode->slotReset();
    d->ocrTesseractPSMMode->slotReset(); 
    d->ocrTesseractDpi->slotReset();
}

void TextConverterSettings::setLanguagesMode(int mode)
{
    d->ocrTesseractLanguageMode->setCurrentIndex(mode);
}

int TextConverterSettings::LanguagesMode() const
{
    return d->ocrTesseractLanguageMode->currentIndex();
}


void TextConverterSettings::setPSMMode(int mode)
{
    d->ocrTesseractPSMMode->setCurrentIndex(mode);
}

int TextConverterSettings::PSMMode() const
{
    return d->ocrTesseractPSMMode->currentIndex();
}

void TextConverterSettings::setOEMMode(int mode)
{
    d->ocrTesseractOEMMode->setCurrentIndex(mode);
}

int TextConverterSettings::OEMMode() const
{
    return d->ocrTesseractOEMMode->currentIndex();
}

void TextConverterSettings::setDpi(int value)
{
    d->ocrTesseractDpi->setValue(value);
}

int  TextConverterSettings::Dpi() const
{
    return d->ocrTesseractDpi->value();
}

void TextConverterSettings::slotSetupChanged()
{
    QScopedPointer<ExifToolParser> const parser(new ExifToolParser(this));

    d->errorView->setVisible(!parser->exifToolAvailable());
}

} // namespace DigikamGenericTextConverterPlugin
