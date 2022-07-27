#include "textconverterdialog.h"

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
#include "textconverterlist.h"
#include "textconvertersettings.h"

using namespace Digikam;


class TextConverterDialog::Private
{
public:

    Private()
      : busy                             (false),
        ocrTesseractLanguageMode         (nullptr),
        ocrTesseractPSMMode              (nullptr),
        ocrTesseractOEMMode              (nullptr),
        listView                         (nullptr),
        optionsTesseractSettingsView     (nullptr),
        settings                         (nullptr),
        iface                            (nullptr)
    {
    }

    bool                      busy;

    QStringList               fileList;

    DInfoInterface*           iface;

    TextConverterList*        listView;

    TextConverterSettings*    settings;

    // Tesseract options 
    
    QWidget*            optionsTesseractSettingsView;

    DComboBox*          ocrTesseractLanguageMode;
    
    DComboBox*          ocrTesseractPSMMode;

    DComboBox*          ocrTesseractOEMMode;
};

TextConverterDialog::TextConverterDialog(QWidget* const parent, DInfoInterface* const iface)
    : DPluginDialog(parent, QLatin1String("Text Converter Dialog")),
      d            (new Private)
{
    setWindowTitle(i18nc("@title", "Text Converter"));
    setMinimumSize(900, 500);
    setModal(true);

    d->iface                  = iface;

    m_buttons->addButton(QDialogButtonBox::Close);
    m_buttons->addButton(QDialogButtonBox::Ok);
    m_buttons->button(QDialogButtonBox::Ok)->setText(i18nc("@action:button", "&start OCR"));
    QWidget* const mainWidget = new QWidget(this);
    
    QVBoxLayout* const vbx    = new QVBoxLayout(this);
    vbx->addWidget(mainWidget);
    vbx->addWidget(m_buttons);
    setLayout(vbx);

    //-------------------------------------------------------------------------------------------

    QGridLayout* const mainLayout = new QGridLayout(mainWidget);
    d->listView                   = new TextConverterList(mainWidget);
    d->optionsTesseractSettingsView   = new QWidget(mainWidget);

    setupTesseractOptionsView();


    //-------------------------------------------------------------------------------------------

    mainLayout->addWidget(d->listView,                       0, 0, 5, 1);
    mainLayout->addWidget(d->optionsTesseractSettingsView,   0, 1, 1, 1);
    mainLayout->setColumnStretch(0, 10);
    mainLayout->setRowStretch(4, 10);
    mainLayout->setContentsMargins(QMargins());

    // ---------------------------------------------------------------


    d->listView->setIface(d->iface);
    d->listView->loadImagesFromCurrentSelection();

    readSettings();
}

void TextConverterDialog::setupTesseractOptionsView()
{
    // ------------

    QMap<QString, QString> optionValues = getValidValues(QLatin1String("list-langs"));
    QLabel* ocrTesseractLanguageLabel   = new QLabel(i18nc("@label", "Languages:"));
    d->ocrTesseractLanguageMode         = new DComboBox(d->optionsTesseractSettingsView);
    d->ocrTesseractLanguageMode->addItem(i18n("default"), QString());
   
    for (QMap<QString, QString>::const_iterator it =  optionValues.constBegin();
         it != optionValues.constEnd();
         it++)
    {
        if (!it.value().isEmpty())
        {
            d->ocrTesseractLanguageMode->addItem(it.value(), it.key());
        }
        else
        {
            d->ocrTesseractLanguageMode->addItem(it.key(), it.key());
        }
    }
    
    optionValues.clear();

    // ------------

    optionValues = getValidValues(QLatin1String("help-psm"));
    QLabel* ocrTesseractPSMLabel  = new QLabel(i18nc("@label", "Segmentation mode:"));
    d->ocrTesseractPSMMode        = new DComboBox(d->optionsTesseractSettingsView);
    d->ocrTesseractPSMMode->addItem(i18n("default"), QString());
   
    for (QMap<QString, QString>::const_iterator it =  optionValues.constBegin();
         it != optionValues.constEnd();
         it++)
    {
        if (!it.value().isEmpty())
        {
            d->ocrTesseractPSMMode->addItem(it.value(), it.key());
        } 
    }

    optionValues.clear();
 
    // ------------
   
    optionValues = getValidValues(QLatin1String("help-oem"));
    QLabel* ocrTesseractOEMLabel  = new QLabel(i18nc("@label", "Engine mode:"));
    d->ocrTesseractOEMMode        = new DComboBox(d->optionsTesseractSettingsView);
    d->ocrTesseractOEMMode->addItem(i18n("default"), QString());
   
    for (QMap<QString, QString>::const_iterator it =  optionValues.constBegin();
         it != optionValues.constEnd();
         it++)
    {
        if (!it.value().isEmpty())
        {
            d->ocrTesseractOEMMode->addItem(it.value(), it.key());
        } 
    }

    optionValues.clear();

    // ------------

    QGridLayout* const settingsBoxLayout = new QGridLayout(d->optionsTesseractSettingsView);
    settingsBoxLayout->addWidget(ocrTesseractLanguageLabel,        0, 0, 1, 1);
    settingsBoxLayout->addWidget(d->ocrTesseractLanguageMode,      0, 1, 1, 1);
    settingsBoxLayout->addWidget(ocrTesseractPSMLabel,             1, 0, 1, 1);
    settingsBoxLayout->addWidget(d->ocrTesseractPSMMode,           1, 1, 1, 1);
    settingsBoxLayout->addWidget(ocrTesseractOEMLabel,             2, 0, 1, 1);
    settingsBoxLayout->addWidget(d->ocrTesseractOEMMode,           2, 1, 1, 1);
    settingsBoxLayout->setRowStretch(0, 10);
    settingsBoxLayout->setContentsMargins(QMargins());
}

QMap<QString, QString>  TextConverterDialog::getValidValues(const QString& opt)
{
    QMap<QString, QString> result; 

    KProcess process; 
    process.setOutputChannelMode(KProcess::MergedChannels);
    process << QLatin1String("tesseract") << QLatin1String("--%1").arg(opt);

    process.execute(5000);

    const QByteArray output = process.readAllStandardOutput();
    const QList<QByteArray> lines = output.split('\n');
    
    QRegExp rx;
    
    for (const QByteArray& line : lines)
    {
        const QString lineStr = QString::fromLocal8Bit(line);

        if (opt == QLatin1String("list-langs")) 
        {
            rx.setPattern(QLatin1String("^\\s*(\\w+)()$"));
        }
        else 
        {
            rx.setPattern(QLatin1String("^\\s*(\\d+)\\s+(\\w.+)?$"));
        }
        
        if (rx.indexIn(lineStr)>-1)
        {
            const QString value = rx.cap(1);
            QString desc = rx.cap(2).simplified();
    
            if (desc.endsWith(QLatin1Char('.')) || desc.endsWith(QLatin1Char(','))) 
                desc.chop(1);

            result.insert(value, desc);
        }
    }

    qDebug() << "values for" << opt << "=" << result.keys();

    return (result);
}


void TextConverterDialog::readSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("OCR Settings"));

    d->settings->readSettings(group);
    d->ocrTesseractPSMMode->setCurrentIndex(d->settings->psm);
    slotSettingsChanged();
}

void TextConverterDialog::saveSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("OCR Settings"));
    d->settings->writeSettings(group);
    config->sync();
}

void TextConverterDialog::slotSettingsChanged()
{
    d->settings->psm                = (int)d->ocrTesseractPSMMode->currentIndex();
}


TextConverterDialog::~TextConverterDialog()
{
    delete d;
}

void TextConverterDialog::closeEvent(QCloseEvent* e)
{
    if (!e)
    {
        return;
    }

    // Stop current conversion if necessary

/**
    if (d->busy)
    {
        slotStartStop();
    }
**/

//    saveSettings();
//    d->listView->listView()->clear();
    e->accept();
}

