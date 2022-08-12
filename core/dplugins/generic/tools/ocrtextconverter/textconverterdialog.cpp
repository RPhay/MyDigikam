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
#include "dtextedit.h"
#include "textconverterthread.h"
#include "textconverteraction.h"


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
        ocrSettings      (nullptr),
        currentSelectedItem (nullptr)
    {
    }

    bool                              busy;
        
    QList<QUrl>                       fileList;
        
    QMap<QUrl, QString>               textEditList;  
        
    DProgressWdg*                     progressBar;

    TextConverterActionThread*        thread; 

    DInfoInterface*                   iface;
        
    TextConverterList*                listView;
        
    TextConverterSettings*            ocrSettings;  
        
    DTextEdit*                        textedit;
        
    QPushButton*                      saveTextButton;

    TextConverterListViewItem*        currentSelectedItem;
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
    
    d->textedit                       = new DTextEdit(mainWidget);
    d->textedit->setMaximumHeight(300);
    d->textedit->setPlaceholderText(QLatin1String("OCR result is displayed here"));
    
    d->saveTextButton = new QPushButton(mainWidget);
    d->saveTextButton->setText(i18nc("@action: button", "Save"));
    d->saveTextButton->setEnabled(false);

    //-------------------------------------------------------------------------------------------

    mainLayout->addWidget(d->listView,                       0, 0, 5, 1);
    mainLayout->addWidget(d->ocrSettings,                    0, 1, 1, 1);
    mainLayout->addWidget(d->textedit,                       1, 1, 3, 1);
    mainLayout->addWidget(d->saveTextButton,                 3, 1, 1, 1);
    mainLayout->addWidget(d->progressBar,                    5, 1, 1, 1);
    mainLayout->setColumnStretch(0, 10);
    mainLayout->setRowStretch(4, 10);
    mainLayout->setContentsMargins(QMargins());

    // ---------------------------------------------------------------
    
    // TODO Thread operation

    d->thread = new TextConverterActionThread(this);

     connect(d->thread, SIGNAL(signalStarting(DigikamGenericTextConverterPlugin::TextConverterActionData)),
            this, SLOT(slotTextConverterAction(DigikamGenericTextConverterPlugin::TextConverterActionData)));

    connect(d->thread, SIGNAL(signalFinished(DigikamGenericTextConverterPlugin::TextConverterActionData)),
            this, SLOT(slotTextConverterAction(DigikamGenericTextConverterPlugin::TextConverterActionData)));

    connect(d->thread, SIGNAL(finished()),
            this, SLOT(slotThreadFinished()));

    // ---------------------------------------------------------------

    
    // TODO connect

    connect(m_buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(slotStartStop()));

    connect(m_buttons->button(QDialogButtonBox::Close), SIGNAL(clicked()),
            this, SLOT(slotClose()));   

    connect(d->ocrSettings, SIGNAL(signalSettingsChanged()),
            this, SLOT(slotStartStop()));

    connect(d->progressBar, SIGNAL(signalProgressCanceled()),
            this, SLOT(slotStartStop()));
    
    connect(d->listView->listView(), &DItemsListView::itemDoubleClicked,
            this, &TextConverterDialog::slotDoubleClick);

    connect(d->listView->listView(), &DItemsListView::itemSelectionChanged,
            this, &TextConverterDialog::slotSetDisable);
                    
    connect(d->saveTextButton, SIGNAL(clicked()),
            this, SLOT(slotUpdateText()));

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

void TextConverterDialog::slotDoubleClick(QTreeWidgetItem* element, int i)
{
    TextConverterListViewItem* const item = dynamic_cast<TextConverterListViewItem*>(element);
    d->currentSelectedItem = item;

    if (d->textEditList.contains(item->url()))
    {
        d->textedit->setText(d->textEditList[item->url()]);
        d->saveTextButton->setEnabled(true);
    }
    else
    {
        d->textedit->clear();
    }
}

void TextConverterDialog::slotUpdateText()
{
    QString newText = d->textedit->text();

    if (!d->textedit->text().isEmpty() &&
        !d->currentSelectedItem->url().isEmpty() &&
        !d->currentSelectedItem->destFileName().isEmpty())
    {
        d->textEditList[d->currentSelectedItem->url()] = newText;
        
        QFile file(d->currentSelectedItem->destFileName());
        
        if(file.open(QIODevice::ReadWrite | QIODevice::Truncate))
        {
            QTextStream stream(&file);
            stream << newText;
            file.close();
        }
        
    }
}

void TextConverterDialog::slotSetDisable()
{
    d->saveTextButton->setEnabled(false);
}

void TextConverterDialog::slotTextConverterAction(const DigikamGenericTextConverterPlugin::TextConverterActionData& ad)
{
    if (ad.starting)
    {
        switch (ad.action)
        {
            case(PROCESS):
            {
                busy(true);
                d->listView->processing(ad.fileUrl);
                d->progressBar->progressStatusChanged(i18n("Processing %1", ad.fileUrl.fileName()));
                break;
            }

            default:
            {
                qWarning() << "DigikamGenericTextConverterPlugin: Unknown action";
                break;
            }
        }
    }
    else
    {
        if (ad.result != OcrTesseracrEngine::PROCESS_COMPLETE)
        {
            switch (ad.action)
            {
                case(PROCESS):
                {
                    processingFailed(ad.fileUrl, ad.result);
                    break;
                }

                default:
                {
                    qWarning() << "DigikamGenericTextConverterPlugin: Unknown action";
                    break;
                }
            }
        }
        else                    // Something is done...
        {
            switch (ad.action)
            {
                case(PROCESS):
                {
                    d->textEditList[ad.fileUrl] = ad.outputText; 
                    processed(ad.fileUrl, ad.destPath);
                    break;
                }

                default:
                {
                    qWarning() << "DigikamGenericDNGConverterPlugin: Unknown action";
                    break;
                }
            }
        }
    }
}


void TextConverterDialog::processingFailed(const QUrl& url, int result)
{
    d->listView->processed(url, false);
    d->progressBar->setValue(d->progressBar->value()+1);

    TextConverterListViewItem* const item = dynamic_cast<TextConverterListViewItem*>(d->listView->listView()->findItem(url));
    if (!item)
    {
        return;
    }

    QString status;

    switch (result)
    {
        case OcrTesseracrEngine::PROCESS_FAILED:
        {
            status = i18n("Process failed");
            break;
        }

        case OcrTesseracrEngine::PROCESS_CANCELED:
        {
            status = i18n("Process Canceled");
            break;
        }

        // TODO Tesseract ocr error

        default:
        {
            status = i18n("Internal error");
            break;
        }
    }

    item->setStatus(status);
}

void TextConverterDialog::processed(const QUrl& url, const QString& outputFile)
{
    TextConverterListViewItem* const item = dynamic_cast<TextConverterListViewItem*>(d->listView->listView()->findItem(url));

    if (!item)
    {
        return;
    }

    // TODO change fie name 

    if (!outputFile.isEmpty())
    {
        item->setDestFileName(outputFile);
    }

    d->listView->processed(url, true);
    item->setStatus(i18n("Success"));

    d->progressBar->setValue(d->progressBar->value()+1);
}

void TextConverterDialog::processAll()
{
    d->thread->setLanguagesMode(d->ocrSettings->LanguagesMode());
    d->thread->setPSMMode(d->ocrSettings->PSMMode());
    d->thread->setOEMMode(d->ocrSettings->OEMMode());
    d->thread->setDpi(d->ocrSettings->Dpi());
    d->thread->setIsSaveTextFile(d->ocrSettings->isSaveTextFile());
    d->thread->ocrProcessFiles(d->fileList);

    if (!d->thread->isRunning())
    {
        d->thread->start();
    }
}

void TextConverterDialog::slotStartStop()
{
    if (!d->busy)
    {
        d->fileList.clear();

        QList<QTreeWidgetItem*> selectedItemsList = d->listView->listView()->selectedItems();

        for (QList<QTreeWidgetItem*>::const_iterator it = selectedItemsList.constBegin() ;
             it != selectedItemsList.constEnd() ; ++it)
        {
            TextConverterListViewItem* const item = dynamic_cast<TextConverterListViewItem*>(*it);

            if (item)
            {
                qDebug() << item->url().path();
                d->fileList.append(item->url());
            }
        }

        if (d->listView->listView()->topLevelItemCount() == 0)
        {
            qDebug() << "Test";
            d->textedit->clear();
        }

        if (d->fileList.empty())
        {
            QMessageBox::information(this, i18n("Text Converter"), i18n("The list does not contain any digital files to process. You need to select them"));
            busy(false);
        //    slotAborted();
            return;
        }

        d->progressBar->setMaximum(d->fileList.count());
        d->progressBar->setValue(0);
        d->progressBar->show();
        d->progressBar->progressScheduled(i18n("Text Converter"), true, true);
        d->progressBar->progressThumbnailChanged(QIcon::fromTheme(QLatin1String("image-x-adobe-dng")).pixmap(22, 22));

        processAll();
    }
    else
    {
        d->fileList.clear();
        d->thread->cancel();
        busy(false);

        d->listView->cancelProcess();

        QTimer::singleShot(500, this, SLOT(slotAborted()));
    }
}

void TextConverterDialog::closeEvent(QCloseEvent* e)
{
    if (!e)
    {
        return;
    }

    // Stop current conversion if necessary

    if (d->busy)
    {
        slotStartStop();
    }

    saveSettings();
    d->listView->listView()->clear();
    e->accept();
}


void TextConverterDialog::slotClose()
{
    // Stop current conversion if necessary

    if (d->busy)
    {
        slotStartStop();
    }


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

    d->ocrSettings->setLanguagesMode(group.readEntry("ocrLanguages",                      int(OcrOptions::Languages::DEFAULT)));
    d->ocrSettings->setPSMMode(group.readEntry("PageSegmentationModes",                   int(OcrOptions::PageSegmentationModes::DEFAULT)));
    d->ocrSettings->setOEMMode(group.readEntry("EngineModes",                             int(OcrOptions::EngineModes::DEFAULT)));
    d->ocrSettings->setDpi(group.readEntry("Dpi",                                         300));
    d->ocrSettings->setIsSaveTextFile(group.readEntry("Check Save Test File",             true));
}

void TextConverterDialog::saveSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("OCR Tesseract Settings"));

    group.writeEntry("ocrLanguages",              (int)d->ocrSettings->LanguagesMode());
    group.writeEntry("PageSegmentationModes",     (int)d->ocrSettings->PSMMode());
    group.writeEntry("EngineModes",               (int)d->ocrSettings->OEMMode());
    group.writeEntry("Dpi",                       (int)d->ocrSettings->Dpi());
    group.writeEntry("Check Save Test File",      (bool)d->ocrSettings->isSaveTextFile());
}

void TextConverterDialog::addItems(const QList<QUrl>& itemList)
{
    d->listView->slotAddImages(itemList);
}

void TextConverterDialog::slotThreadFinished()
{
    busy(false);
    slotAborted();
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

} // namespace DigikamGenericTextConverterPlugin
