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
#include "textconverterlist.h"
#include "textconvertersettings.h"
#include "ocroptions.h"

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

class TextConverterDialog::Private
{
public:

    Private()
      : busy             (false),
        progressBar      (nullptr),
        iface            (nullptr),
        listView         (nullptr),
        ocrSettings      (nullptr)
    {
    }

    bool                      busy;

    QStringList               fileList;

    DProgressWdg*             progressBar;

    // TODO Thread

    DInfoInterface*           iface;

    TextConverterList*        listView;

    TextConverterSettings*    ocrSettings;    
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

    QGridLayout* const mainLayout     = new QGridLayout(mainWidget);
    d->listView                       = new TextConverterList(mainWidget);
    d->ocrSettings = new TextConverterSettings(this);
    d->progressBar                    = new DProgressWdg(mainWidget);
    d->progressBar->reset();
    d->progressBar->hide();

    //-------------------------------------------------------------------------------------------

    mainLayout->addWidget(d->listView,                       0, 0, 5, 1);
    mainLayout->addWidget(d->ocrSettings,                    0, 1, 1, 1);
    mainLayout->addWidget(d->progressBar,                    2, 1, 1, 1);
    mainLayout->setColumnStretch(0, 10);
    mainLayout->setRowStretch(4, 10);
    mainLayout->setContentsMargins(QMargins());

    // ---------------------------------------------------------------
    
    // TODO Thread operation 

    // ---------------------------------------------------------------

    
    // TODO connect

    connect(m_buttons->button(QDialogButtonBox::Close), SIGNAL(clicked()),
            this, SLOT(slotClose()));

    connect(d->ocrSettings, SIGNAL(TextConverterSettings::signalSettingsChanged()),
            this, SLOT(slotIdentify()));

    // ---------------------------------------------------------------
    
    d->listView->setIface(d->iface);
    d->listView->loadImagesFromCurrentSelection();

    busy(false);
    readSettings();
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

    saveSettings();
    d->listView->listView()->clear();
    e->accept();
}


void TextConverterDialog::slotClose()
{
    // Stop current conversion if necessary
/**
     if (d->busy)
    {
        slotStartStop();
    }
**/

    saveSettings();
    d->listView->listView()->clear();
    d->fileList.clear();
    accept();
}


void TextConverterDialog::slotSetupExifTool()
{
    if (d->iface)
    {
        connect(d->iface, SIGNAL(signalSetupChanged()),
                d->ocrSettings, SLOT(slotSetupChanged()));

        d->iface->openSetupPage(DInfoInterface::ExifToolPage);
    }
}

void TextConverterDialog::slotDefault()
{
    d->ocrSettings->setDefaultSettings();
}

void TextConverterDialog::readSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("OCR Tesseract Settings"));

    d->ocrSettings->setLanguagesMode(group.readEntry("ocrLanguages",     int(OcrOptions::Languages::ENG)));
    d->ocrSettings->setPSMMode(group.readEntry("PageSegmentationModes",  int(OcrOptions::PageSegmentationModes::FULLY_AUTO_PAGE)));
    d->ocrSettings->setOEMMode(group.readEntry("EngineModes",            int(OcrOptions::EngineModes::DEFAULT)));
}

void TextConverterDialog::saveSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("OCR Tesseract Settings"));

    group.writeEntry("ocrLanguages",              (int)d->ocrSettings->LanguagesMode());
    group.writeEntry("PageSegmentationModes",     (int)d->ocrSettings->PSMMode());
    group.writeEntry("EngineModes",               (int)d->ocrSettings->OEMMode());
}

void TextConverterDialog::addItems(const QList<QUrl>& itemList)
{
    d->listView->slotAddImages(itemList);
}

void TextConverterDialog::busy(bool busy)  
{
    d->busy = busy;

    if (d->busy)
    {
        m_buttons->button(QDialogButtonBox::Ok)->setText(i18n("&Abort"));
        m_buttons->button(QDialogButtonBox::Ok)->setToolTip(i18n("Abort ocr Processing of Raw files."));
    }
    else
    {
        m_buttons->button(QDialogButtonBox::Ok)->setText(i18n("&Start OCR"));
        m_buttons->button(QDialogButtonBox::Ok)->setToolTip(i18n("Start OCR using the current settings."));
    }

    d->ocrSettings->setEnabled(!d->busy);
    d->listView->listView()->viewport()->setEnabled(!d->busy);
    d->busy ? setCursor(Qt::WaitCursor) : unsetCursor();
}

void TextConverterDialog::slotAborted()
{
    d->progressBar->setValue(0);
    d->progressBar->hide();
    d->progressBar->progressCompleted();
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
            QString desc        = rx.cap(2).simplified();
    
            if (desc.endsWith(QLatin1Char('.')) || desc.endsWith(QLatin1Char(','))) 
                desc.chop(1);

            result.insert(value, desc);
        }
    }

    qDebug() << "values for" << opt << "=" << result.keys();

    return (result);
}

} // namespace DigikamGenericTextConverterPlugin
