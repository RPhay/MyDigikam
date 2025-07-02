/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-11-14
 * Description : Files downloader
 *
 * SPDX-FileCopyrightText: 2020-2024 by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "filesdownloader.h"

// Qt includes

#include <QDir>
#include <QUrl>
#include <QLabel>
#include <QTimer>
#include <QThread>
#include <QPointer>
#include <QCheckBox>
#include <QByteArray>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QApplication>
#include <QStandardPaths>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QCryptographicHash>
#include <QNetworkAccessManager>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "dxmlguiwindow.h"
#include "systemsettings.h"
#include "dnnmodelmanager.h"
#include "itempropertiestab.h"

namespace Digikam
{

class Q_DECL_HIDDEN FilesDownloader::Private
{
public:

    Private() = default;

public:

    QList<DownloadInfo>    files;
    DownloadInfo           currentInfo;

    int                    redirects        = 0;
    int                    total            = 0;
    int                    count            = 0;

    bool                   fallback         = false;

    QDialogButtonBox*      buttons          = nullptr;
    QProgressBar*          progress         = nullptr;
    QCheckBox*             aiAutoToolsCheck = nullptr;
    QCheckBox*             faceEngineCheck  = nullptr;
    QCheckBox*             aestheticCheck   = nullptr;
    QCheckBox*             autoTagsCheck    = nullptr;
    QLabel*                nameLabel        = nullptr;
    QLabel*                infoLabel        = nullptr;
    QLabel*                loadLabel        = nullptr;
    QLabel*                sizeLabel        = nullptr;

    QNetworkReply*         reply            = nullptr;
    QNetworkAccessManager* netMngr          = nullptr;

    SystemSettings         system           = SystemSettings(qApp->applicationName());

    QString                error;
    const QString          downloadUrl      = QLatin1String("https://files.kde.org/digikam/");
    const QString          fallbackUrl      = QLatin1String("https://cdn.files.kde.org/digikam/");
    const QString          userAgent        = QLatin1String("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 "
                                                            "(KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36");
};

FilesDownloader::FilesDownloader(QWidget* const parent)
    : QDialog(parent),
      d      (new Private)
{
    createDownloadInfo();
}

FilesDownloader::~FilesDownloader()
{
    if (d->reply)
    {
        d->reply->abort();
        d->reply = nullptr;
    }

    delete d;
}

bool FilesDownloader::checkDownloadFiles() const
{
    QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                          QLatin1String("digikam/facesengine"),
                                          QStandardPaths::LocateDirectory);

    if (path.isEmpty())
    {
        return false;
    }

    for (const DownloadInfo& info : std::as_const(d->files))
    {
        QFileInfo fileInfo(path + QLatin1Char('/') + info.name);

        if (!fileInfo.exists() || (fileInfo.size() != info.size))
        {
            return false;
        }
    }

    return true;
}

void FilesDownloader::startDownload()
{
    setWindowTitle(i18nc("@title:window", "Download Required Model Files"));
    setMinimumHeight(250);
    setMinimumWidth(600);

    QWidget* const mainWidget = new QWidget(this);
    QVBoxLayout* const vBox   = new QVBoxLayout(mainWidget);

    d->buttons                = new QDialogButtonBox(QDialogButtonBox::Help |
                                                     QDialogButtonBox::Ok   |
                                                     QDialogButtonBox::Open |
                                                     QDialogButtonBox::Close,
                                                     mainWidget);
    d->buttons->button(QDialogButtonBox::Ok)->setDefault(true);
    d->buttons->button(QDialogButtonBox::Ok)->setText(i18n("Download"));
    d->buttons->button(QDialogButtonBox::Ok)->setIcon(QIcon::fromTheme(QLatin1String("edit-download")));

    d->buttons->button(QDialogButtonBox::Open)->setToolTip(i18n("Open local directory used for the data files storage."));

    d->infoLabel         = new QLabel(mainWidget);
    d->loadLabel         = new QLabel(mainWidget);
    d->sizeLabel         = new QLabel(mainWidget);

    d->infoLabel->setWordWrap(true);
    d->loadLabel->setWordWrap(true);

    d->aiAutoToolsCheck = new QCheckBox(i18n("Use AI Auto-Tools features"),       mainWidget);
    d->faceEngineCheck  = new QCheckBox(i18n("Use Face Management feature"),      mainWidget);
    d->aestheticCheck   = new QCheckBox(i18n("Use Aesthetic Detection feature"),  mainWidget);
    d->autoTagsCheck    = new QCheckBox(i18n("Use AutoTags Assignment feature"),  mainWidget);

    d->progress          = new QProgressBar(mainWidget);
    d->progress->setFormat(i18nc("%p is the percent value, % is the percent sign", "%p%"));
    d->progress->setMinimum(0);
    d->progress->setMaximum(1);
    d->progress->setValue(0);

    d->nameLabel         = new QLabel(mainWidget);

    vBox->addWidget(d->infoLabel);
    vBox->addWidget(d->loadLabel);
    vBox->addWidget(d->sizeLabel);
    vBox->addStretch(1);
    vBox->addWidget(d->aiAutoToolsCheck);
    vBox->addWidget(d->faceEngineCheck);
    vBox->addWidget(d->aestheticCheck);
    vBox->addWidget(d->autoTagsCheck);
    vBox->addStretch(1);
    vBox->addWidget(d->nameLabel);
    vBox->addWidget(d->progress);
    vBox->addWidget(d->buttons);

    setLayout(vBox);

    connect(d->buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(slotDownload()));

    connect(d->buttons->button(QDialogButtonBox::Open), SIGNAL(clicked()),
            this, SLOT(slotOpenLocalRepo()));

    connect(d->buttons->button(QDialogButtonBox::Help), SIGNAL(clicked()),
            this, SLOT(slotHelp()));

    connect(d->buttons->button(QDialogButtonBox::Close), SIGNAL(clicked()),
            this, SLOT(reject()));

    d->aiAutoToolsCheck->setChecked(d->system.enableAIAutoTools);
    d->faceEngineCheck->setChecked(d->system.enableFaceEngine);
    d->aestheticCheck->setChecked(d->system.enableAesthetic);
    d->autoTagsCheck->setChecked(d->system.enableAutoTags);

    connect(d->aiAutoToolsCheck, SIGNAL(toggled(bool)),
            this, SLOT(slotUpdateDownloadInfo()));

    connect(d->faceEngineCheck, SIGNAL(toggled(bool)),
            this, SLOT(slotUpdateDownloadInfo()));

    connect(d->aestheticCheck, SIGNAL(toggled(bool)),
            this, SLOT(slotUpdateDownloadInfo()));

    connect(d->autoTagsCheck, SIGNAL(toggled(bool)),
            this, SLOT(slotUpdateDownloadInfo()));

    slotUpdateDownloadInfo();

    QEventLoop loop;

    connect(this, &QDialog::finished,
            &loop, &QEventLoop::quit);

    show();

    loop.exec();
}

void FilesDownloader::slotDownload()
{
    d->buttons->button(QDialogButtonBox::Ok)->setEnabled(false);
    d->aiAutoToolsCheck->setEnabled(false);
    d->faceEngineCheck->setEnabled(false);
    d->aestheticCheck->setEnabled(false);
    d->autoTagsCheck->setEnabled(false);

    if (d->error.isEmpty())
    {
        while (!d->files.isEmpty())
        {
            d->currentInfo = d->files.takeFirst();

            if (!downloadExists(d->currentInfo))
            {
                d->count++;
                download();

                return;
            }
        }

        QMessageBox::information(this, qApp->applicationName(),
                                 i18n("All model files were downloaded successfully."));

        deleteUnusedFiles();

        close();
    }
    else
    {
        QPointer<QMessageBox> msgBox = new QMessageBox(QMessageBox::Critical,
                 i18nc("@title:window", "Download Error"),
                 i18n("An error occurred during the download.\n\n"
                      "File: %1\n\n%2\n\n"
                      "You can try again or continue the "
                      "download at the next start.",
                      d->currentInfo.name, d->error),
                 QMessageBox::Yes | QMessageBox::Cancel,
                 qApp->activeWindow());

        msgBox->button(QMessageBox::Yes)->setText(i18nc("@action:button", "Try Again"));
        msgBox->button(QMessageBox::Yes)->setIcon(QIcon::fromTheme(QLatin1String("edit-download")));

        int result = msgBox->exec();
        delete msgBox;

        if (result == QMessageBox::Yes)
        {
            d->fallback = !d->fallback;
            d->error.clear();
            download();

            return;
        }

        close();
    }
}

void FilesDownloader::download()
{
    if (!d->netMngr)
    {
        d->netMngr = new QNetworkAccessManager(this);
        d->netMngr->setRedirectPolicy(QNetworkRequest::ManualRedirectPolicy);

        connect(d->netMngr, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(slotDownloaded(QNetworkReply*)));
    }

    QString serverUrl(d->fallback ? d->fallbackUrl
                                  : d->downloadUrl);

    QUrl request(serverUrl           +
                 d->currentInfo.path +
                 d->currentInfo.name);

    d->redirects = 0;
    createRequest(request);
}

void FilesDownloader::nextDownload()
{
    QTimer::singleShot(100, this, SLOT(slotDownload()));
}

void FilesDownloader::createRequest(const QUrl& url)
{
    d->progress->setMaximum(d->currentInfo.size);
    d->progress->setValue(0);
    printDownloadInfo(url);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, d->userAgent);

    d->redirects++;
    d->reply = d->netMngr->get(request);

    connect(d->reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(slotDownloadProgress(qint64,qint64)));

    connect(d->reply, SIGNAL(sslErrors(QList<QSslError>)),
            d->reply, SLOT(ignoreSslErrors()));
}

void FilesDownloader::printDownloadInfo(const QUrl& url)
{
    QString text = QString::fromUtf8("%1 (%2://%3) (%4/%5)")
                   .arg(d->currentInfo.name)
                   .arg(url.scheme())
                   .arg(url.host())
                   .arg(d->count)
                   .arg(d->total);

    d->nameLabel->setText(text);
}

bool FilesDownloader::downloadExists(const DownloadInfo& info) const
{
    QString path = getFacesEnginePath() +
                   QString::fromLatin1("/%1").arg(info.name);

    return (!path.isEmpty() && (QFileInfo(path).size() == info.size));
}

void FilesDownloader::reject()
{
    if (d->reply)
    {
        d->reply->abort();
        d->reply = nullptr;
    }

    QDialog::reject();
}

void FilesDownloader::slotDownloaded(QNetworkReply* reply)
{
    if (reply != d->reply)
    {
        return;
    }

    d->reply = nullptr;

    if (
        (reply->error() != QNetworkReply::NoError)             &&
        (reply->error() != QNetworkReply::InsecureRedirectError)
       )
    {
        d->error = reply->errorString();

        reply->deleteLater();

        nextDownload();

        return;
    }

    QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

    if (redirectUrl.isValid() && (reply->url() != redirectUrl) && (d->redirects < 10))
    {
        createRequest(redirectUrl);

        reply->deleteLater();

        return;
    }

    QCryptographicHash sha256(QCryptographicHash::Sha256);

    QByteArray data = reply->readAll();

    sha256.addData(data);

    if (d->currentInfo.hash != QString::fromLatin1(sha256.result().toHex()))
    {
        d->error = i18n("Checksum is incorrect.");

        reply->deleteLater();

        nextDownload();

        return;
    }

    QString path = getFacesEnginePath();

    if (!QFileInfo::exists(path))
    {
        QDir().mkpath(path);
    }

    QFile file(path + QLatin1Char('/') + d->currentInfo.name);

    if (file.open(QIODevice::WriteOnly))
    {
        qint64 written = file.write(data);

        if (written != d->currentInfo.size)
        {
            d->error = i18n("File write error.");
        }

        file.close();
    }
    else
    {
        d->error = i18n("File open error.");
    }

    reply->deleteLater();

    nextDownload();
}

void FilesDownloader::slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (d->reply && (bytesReceived > d->currentInfo.size))
    {
        d->reply->abort();
        d->reply = nullptr;

        d->error = i18n("File on the server is too large.");

        nextDownload();

        return;
    }

    d->progress->setMaximum(bytesTotal);
    d->progress->setValue(bytesReceived);
}

void FilesDownloader::slotOpenLocalRepo()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(getFacesEnginePath()));
}

QString FilesDownloader::getFacesEnginePath()
{
    QString appPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    appPath        += QLatin1String("/digikam/facesengine");

    return appPath;
}

void FilesDownloader::slotHelp()
{
    openOnlineDocumentation(QLatin1String("getting_started"),
                            QLatin1String("quick_start"),
                            QLatin1String("firstrun-downloads"));
}

void FilesDownloader::createDownloadInfo()
{
    d->files.clear();

    if (d->system.enableFaceEngine)
    {
        d->files << DNNModelManager::instance()->getDownloadInformation(DNNModelUsage::DNNUsageFaceDetection);
        d->files << DNNModelManager::instance()->getDownloadInformation(DNNModelUsage::DNNUsageFaceRecognition);
    }

    if (d->system.enableAesthetic)
    {
        d->files << DNNModelManager::instance()->getDownloadInformation(DNNModelUsage::DNNUsageAesthetics);
    }

    if (d->system.enableAIAutoTools)
    {
        d->files << DNNModelManager::instance()->getDownloadInformation(DNNModelUsage::DNNUsageAutoRotate);
    }

    if (d->system.enableAutoTags)
    {
        d->files << DNNModelManager::instance()->getDownloadInformation(DNNModelUsage::DNNUsageObjectDetection);
        d->files << DNNModelManager::instance()->getDownloadInformation(DNNModelUsage::DNNUsageImageClassification);
    }
}

void FilesDownloader::slotUpdateDownloadInfo()
{
    QString path                = QDir::toNativeSeparators(getFacesEnginePath());
    d->system.enableAIAutoTools = d->aiAutoToolsCheck->isChecked();
    d->system.enableFaceEngine  = d->faceEngineCheck->isChecked();
    d->system.enableAesthetic   = d->aestheticCheck->isChecked();
    d->system.enableAutoTags    = d->autoTagsCheck->isChecked();
    d->system.saveSettings();

    createDownloadInfo();

    qint64 size = 0;
    d->total    = 0;

    for (const DownloadInfo& info : std::as_const(d->files))
    {
        if (!downloadExists(info))
        {
            // cppcheck-suppress useStlAlgorithm
            size += info.size;
            d->total++;
        }
    }

    QString sizeString = ItemPropertiesTab::humanReadableBytesCount(size);

    d->infoLabel->setText(i18n("<p>digiKam relies on large binary model files to power features like face management, "
                               "image classification based on aesthetics, AI AutoTools such as automatic image rotation, "
                               "and automatic tag assignment. You have the option to select the specific features you want to "
                               "enable.</p><p><b>Note:</b> The red-eye removal tool requires face management model files.</p>"));

    d->loadLabel->setText(i18nc("%1: folder path",
                                "<p>Some required files are missing. Click “Download” to start installing the necessary model files. "
                                "If you close this dialog you will be prompted again the next time you launch digiKam. The "
                                "selected features will not function without these files.</p>"
                                "<p>The files will be downloaded to \"<i>%1</i>\".</p>"
                                "<p><b>You must restart digiKam after successfully downloading the files.</b></p>", path));
    if (size > 0)
    {
        d->sizeLabel->setText(i18nc("%1: file counter, %2: disk size with unit",
                                    "The download requires %1 files with a size of %2.",
                                    d->total, sizeString));

        d->loadLabel->setEnabled(true);
        d->buttons->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
    else
    {
        d->sizeLabel->setText(i18n("All files for the selected features were found."));

        d->loadLabel->setEnabled(false);
        d->loadLabel->hide();
        d->buttons->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}

void FilesDownloader::deleteUnusedFiles() const
{
    QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                          QLatin1String("digikam/facesengine"),
                                          QStandardPaths::LocateDirectory);

    if (!path.isEmpty())
    {
        // Get a QDir object for the directory

        QDir dir(path);

        d->nameLabel->setText(i18n("Clean up unused AI/DNN model files..."));
        d->progress->setMaximum(dir.entryInfoList(QDir::Files).size());
        d->progress->setValue(0);

        // get a list of all model files regardless if the feature is enabled or not

        QList<DownloadInfo> fileList;
        fileList << DNNModelManager::instance()->getDownloadInformation(DNNModelUsage::DNNUsageFaceDetection)
                 << DNNModelManager::instance()->getDownloadInformation(DNNModelUsage::DNNUsageFaceRecognition)
                 << DNNModelManager::instance()->getDownloadInformation(DNNModelUsage::DNNUsageAesthetics)
                 << DNNModelManager::instance()->getDownloadInformation(DNNModelUsage::DNNUsageAutoRotate)
                 << DNNModelManager::instance()->getDownloadInformation(DNNModelUsage::DNNUsageObjectDetection)
                 << DNNModelManager::instance()->getDownloadInformation(DNNModelUsage::DNNUsageImageClassification);

        // iterate over all files in the directory and remove those that are not in the list

        const auto lst = dir.entryInfoList(QDir::Files);

        for (const QFileInfo& info : lst)
        {
            bool found = false;
            d->progress->setValue(d->progress->value() + 1);

            for (const DownloadInfo& dinfo : std::as_const(fileList))
            {
                if (info.fileName() == dinfo.name)
                {
                    found = true;
                    break;
                }
            }

            // delete the file if it was not found in the list

            if (!found)
            {
                QFile::remove(info.filePath());
            }

            QThread::msleep(250);
        }
    }
}

//-----------------------------------------------------------------------------

DownloadInfo::DownloadInfo(const QString& _path,
                           const QString& _name,
                           const QString& _hash,
                           const qint64&  _size)
    : path(_path),
      name(_name),
      hash(_hash),
      size(_size)
{
}

DownloadInfo::DownloadInfo(const DownloadInfo& other)
    : path(other.path),
      name(other.name),
      hash(other.hash),
      size(other.size)
{
}

DownloadInfo& DownloadInfo::operator=(const DownloadInfo& other)
{
    path = other.path;
    name = other.name;
    hash = other.hash;
    size = other.size;

    return *this;
}

} // namespace Digikam

#include "moc_filesdownloader.cpp"
