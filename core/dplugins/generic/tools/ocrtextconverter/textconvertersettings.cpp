#include "textconvertersettings.h"

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
#include <QCheckBox>

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
        saveTextFile               (nullptr),
        saveXMP                    (nullptr)
    {
    }

    // Tesseract options 
    
    DComboBox*          ocrTesseractLanguageMode;
    
    DComboBox*          ocrTesseractPSMMode;

    DComboBox*          ocrTesseractOEMMode;

    DIntNumInput*       ocrTesseractDpi;

    QCheckBox*          saveTextFile;

    QCheckBox*          saveXMP;
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
    d->ocrTesseractLanguageMode->setToolTip(i18nc("@info", "Specify language(s) used for OCR."
                                                           "In the mode default of Language settings, for digital text with multiple languages \n"
                                                           "Tesseract can automatically recognize languages using Latin Alphabets such as English or French,...\n"
                                                           "but not compatible languages using hieroglyphs such as Chinese, Japanese."));
    
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
    d->ocrTesseractPSMMode->setToolTip(i18nc("@info", "Specify page segmentation mode."));

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
    d->ocrTesseractOEMMode->setToolTip(i18nc("@info", "Specify OCR Engine mode."));

    // ------------

    QLabel* const ocrTesseractDpiLabel  = new QLabel(i18nc("@label", "Resolution Dpi:"));
    d->ocrTesseractDpi                  = new DIntNumInput(this);
    d->ocrTesseractDpi->setRange(70, 2400, 1);
    d->ocrTesseractDpi->setToolTip(i18nc("@info", "Specify DPI for input image."));
    d->ocrTesseractDpi->setDefaultValue(300);
    ocrTesseractDpiLabel->setBuddy(d->ocrTesseractDpi);

    // ------------

    QLabel* const saveOcrResultLabel = new QLabel(i18nc("@label", "Store result in : "));
    d->saveTextFile = new QCheckBox(i18nc("@option:check", "Text file"), this);
    d->saveTextFile->setToolTip(i18nc("@info", "Store Ocr result in separated text file"));
    d->saveTextFile->setChecked(true);
    
    d->saveXMP = new QCheckBox(i18nc("@option:check", "XMP"), this);
    d->saveXMP->setToolTip(i18nc("@info", "Store Ocr result in XMP metadata"));
    d->saveXMP->setChecked(true);
    
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
    settingsBoxLayout->addWidget(saveOcrResultLabel,               4, 0, 1, 1);
    settingsBoxLayout->addWidget(d->saveTextFile,                  5, 0, 1, 1);
    settingsBoxLayout->addWidget(d->saveXMP,                       5, 1, 1, 1);


    settingsBoxLayout->setRowStretch(9, 10);
    settingsBoxLayout->setContentsMargins(QMargins());

    // ------------------------------------------------------------------------
    
    connect(d->ocrTesseractLanguageMode, SIGNAL(activated(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->ocrTesseractPSMMode, SIGNAL(activated(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->ocrTesseractOEMMode, SIGNAL(activated(int)),
            this, SIGNAL(signalSettingsChanged()));


    // ------------------------------------------------------------------------
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
    d->saveTextFile->setChecked(true);
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

void TextConverterSettings::setIsSaveTextFile(bool check)
{
    d->saveTextFile->setChecked(check);
}

bool  TextConverterSettings::isSaveTextFile() const
{
    return d->saveTextFile->isChecked();
}

void TextConverterSettings::setIsSaveXMP(bool check)
{
    d->saveXMP->setChecked(check);
}

bool  TextConverterSettings::isSaveXMP() const
{
    return d->saveXMP->isChecked();
}


} // namespace DigikamGenericTextConverterPlugin
