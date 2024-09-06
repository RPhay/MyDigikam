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
#include <QLabel>
#include <QTimer>
#include <QPointer>
#include <QCheckBox>
#include <QByteArray>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QApplication>
#include <QStandardPaths>
#include <QNetworkRequest>
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
#include "itempropertiestab.h"

namespace Digikam
{

class Q_DECL_HIDDEN FilesDownloader::Private
{
public:

    Private() = default;

    QList<DownloadInfo>    files;
    DownloadInfo           currentInfo;

    int                    redirects        = 0;
    int                    total            = 0;
    int                    count            = 0;

    QDialogButtonBox*      buttons          = nullptr;
    QProgressBar*          progress         = nullptr;
    QCheckBox*             facesEngineCheck = nullptr;
    QCheckBox*             aestheticCheck   = nullptr;
    QCheckBox*             autoTagsCheck    = nullptr;
    QLabel*                nameLabel        = nullptr;
    QLabel*                infoLabel        = nullptr;
    QLabel*                sizeLabel        = nullptr;

    QNetworkReply*         reply            = nullptr;
    QNetworkAccessManager* netMngr          = nullptr;

    SystemSettings         system           = SystemSettings(qApp->applicationName());

    QString                error;
    const QString          downloadUrl      = QLatin1String("https://files.kde.org/digikam/");
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
    setWindowTitle(i18nc("@title:window", "Download Required Files"));
    setMinimumHeight(250);
    setMinimumWidth(600);

    QWidget* const mainWidget = new QWidget(this);
    QVBoxLayout* const vBox   = new QVBoxLayout(mainWidget);

    d->buttons                = new QDialogButtonBox(QDialogButtonBox::Help |
                                                     QDialogButtonBox::Ok |
                                                     QDialogButtonBox::Close,
                                                     mainWidget);
    d->buttons->button(QDialogButtonBox::Ok)->setDefault(true);
    d->buttons->button(QDialogButtonBox::Ok)->setText(i18n("Download"));
    d->buttons->button(QDialogButtonBox::Ok)->setIcon(QIcon::fromTheme(QLatin1String("edit-download")));

    d->infoLabel         = new QLabel(mainWidget);
    d->sizeLabel         = new QLabel(mainWidget);

    d->infoLabel->setWordWrap(true);

    d->facesEngineCheck  = new QCheckBox(i18n("Use Face Detection feature"), mainWidget);
    d->aestheticCheck    = new QCheckBox(i18n("Use Aesthetic Detection feature"), mainWidget);
    d->autoTagsCheck     = new QCheckBox(i18n("Use Auto Tags Assignment feature"), mainWidget);

    d->progress          = new QProgressBar(mainWidget);
    d->progress->setMinimum(0);
    d->progress->setMaximum(1);
    d->progress->setValue(0);

    d->nameLabel         = new QLabel(mainWidget);

    vBox->addWidget(d->infoLabel);
    vBox->addWidget(d->sizeLabel);
    vBox->addStretch(1);
    vBox->addWidget(d->facesEngineCheck);
    vBox->addWidget(d->aestheticCheck);
    vBox->addWidget(d->autoTagsCheck);
    vBox->addStretch(1);
    vBox->addWidget(d->nameLabel);
    vBox->addWidget(d->progress);
    vBox->addWidget(d->buttons);

    setLayout(vBox);

    connect(d->buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(slotDownload()));

    connect(d->buttons->button(QDialogButtonBox::Help), SIGNAL(clicked()),
            this, SLOT(slotHelp()));

    connect(d->buttons->button(QDialogButtonBox::Close), SIGNAL(clicked()),
            this, SLOT(reject()));

    d->facesEngineCheck->setChecked(d->system.enableFaceEngine);
    d->aestheticCheck->setChecked(d->system.enableAesthetic);
    d->autoTagsCheck->setChecked(d->system.enableAutoTags);

    connect(d->facesEngineCheck, SIGNAL(toggled(bool)),
            this, SLOT(slotUpdateDownloadInfo()));

    connect(d->aestheticCheck, SIGNAL(toggled(bool)),
            this, SLOT(slotUpdateDownloadInfo()));

    connect(d->autoTagsCheck, SIGNAL(toggled(bool)),
            this, SLOT(slotUpdateDownloadInfo()));

    slotUpdateDownloadInfo();

    (void)exec();
}

void FilesDownloader::slotDownload()
{
    d->buttons->button(QDialogButtonBox::Ok)->setEnabled(false);
    d->facesEngineCheck->setEnabled(false);
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
                                 i18n("All files were downloaded successfully."));

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

    QUrl request(d->downloadUrl      +
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

    d->redirects++;
    d->reply = d->netMngr->get(QNetworkRequest(url));

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

    if ((reply->error() != QNetworkReply::NoError)             &&
        (reply->error() != QNetworkReply::InsecureRedirectError))
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

QString FilesDownloader::getFacesEnginePath() const
{
    QString appPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    appPath        += QLatin1String("/digikam/facesengine");

    return appPath;
}

void FilesDownloader::slotHelp()
{
    openOnlineDocumentation(QLatin1String("getting_started"), QLatin1String("quick_start"), QLatin1String("firstrun-downloads"));
}

void FilesDownloader::createDownloadInfo()
{
    d->files.clear();

    if (d->system.enableFaceEngine)
    {
        d->files << DownloadInfo(QLatin1String("facesengine/shape-predictor/"),
                                 QLatin1String("shapepredictor.dat"),
                                 QLatin1String("6f3d2a59dc30c7c9166983224dcf5732b25de734fff1e36ff1f3047ef90ed82b"),
                                 67740572
                                );

        d->files << DownloadInfo(QLatin1String("facesengine/dnnface/"),
                                 QLatin1String("yolov3-face.cfg"),
                                 QLatin1String("f6563bd6923fd6500d2c2d6025f32ebdba916a85e5c9798351d916909f62aaf5"),
                                 8334
                                );

        d->files << DownloadInfo(QLatin1String("facesengine/dnnface/"),
                                QLatin1String("yolov3-wider_16000.weights"),
                                QLatin1String("a88f3b3882e3cce1e553a81d42beef6202cb9afc3db88e7944f9ffbcc369e7df"),
                                246305388
                               );

        if (qApp->applicationName() == QLatin1String("digikam"))
        {
            d->files << DownloadInfo(QLatin1String("facesengine/dnnface/"),
                                     QLatin1String("openface_nn4.small2.v1.t7"),
                                     QLatin1String("9b72d54aeb24a64a8135dca8e792f7cc675c99a884a6940350a6cedcf7b7ba08"),
                                     31510785
                                    );

            d->files << DownloadInfo(QLatin1String("facesengine/dnnface/"),
                                     QLatin1String("deploy.prototxt"),
                                     QLatin1String("f62621cac923d6f37bd669298c428bb7ee72233b5f8c3389bb893e35ebbcf795"),
                                     28092
                                    );

            d->files << DownloadInfo(QLatin1String("facesengine/dnnface/"),
                                     QLatin1String("res10_300x300_ssd_iter_140000_fp16.caffemodel"),
                                     QLatin1String("510ffd2471bd81e3fcc88a5beb4eae4fb445ccf8333ebc54e7302b83f4158a76"),
                                     5351047
                                    );
        }
    }

    if (d->system.enableAesthetic)
    {
        d->files << DownloadInfo(QLatin1String("aestheticdetector/"),
                                 QLatin1String("weights_inceptionv3_299.pb"),
                                 QLatin1String("8923e3daff71c07533b9023ef32c69d8c058d1e0931d76d8b81241a201138538"),
                                 88007527
                                );
    }

    if (d->system.enableAutoTags)
    {
        d->files << DownloadInfo(QLatin1String("autotags/"),
                                 QLatin1String("yolov5n_batch_16_s320.onnx"),
                                 QLatin1String("43cf201144e6918354892a2fda7ccb07b3df1abc136ddac3b5c968dffc847009"),
                                 7616008
                                );

        d->files << DownloadInfo(QLatin1String("autotags/"),
                                 QLatin1String("yolov5x_batch_16_s320.onnx"),
                                 QLatin1String("e56b5024e6d29f8a35dbfbdd67bbdb69593fb67bfb275915081f9a463fcd355b"),
                                 347010839
                                );

        d->files << DownloadInfo(QLatin1String("autotags/"),
                                 QLatin1String("resnet50.onnx"),
                                 QLatin1String("490e2761519cbf8a4433f880c2aa16b457730085cf9a8aab1e43d82bcadba4f1"),
                                 102146365
                                );

        d->files << DownloadInfo(QLatin1String("autotags/"),
                                 QLatin1String("coco.names"),
                                 QLatin1String("634a1132eb33f8091d60f2c346ababe8b905ae08387037aed883953b7329af84"),
                                 625
                                );

        d->files << DownloadInfo(QLatin1String("autotags/"),
                                 QLatin1String("classification_classes_ILSVRC2012.txt"),
                                 QLatin1String("4eb3da435cf544e4a6f390f62c84cb9c9bb68cf8b14e97f8a063452382e5efd2"),
                                 21675
                                );
    }

}

void FilesDownloader::slotUpdateDownloadInfo()
{
    QString path = QDir::toNativeSeparators(getFacesEnginePath());

    d->system.enableFaceEngine = d->facesEngineCheck->isChecked();
    d->system.enableAesthetic  = d->aestheticCheck->isChecked();
    d->system.enableAutoTags   = d->autoTagsCheck->isChecked();
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

    d->infoLabel->setText(i18nc("%1: folder path",
                                "<p>For face detection, the red-eye removal tool, the classification "
                                "of images according to aesthetics or the automatic assignment "
                                "of tags to images, digiKam requires some large binary files. You "
                                "can choose which feature you want to use.</p>"
                                "<p>Some of these files were not found. Click “Download” to begin "
                                "downloading the files you need. You can close this dialog, you "
                                "will be asked again the next time you start digiKam. Without "
                                "these files the selected features will not work.</p>"
                                "<p>The files will be downloaded to %1.</p>"
                                "<p><b>After the successful download you have to restart digiKam.</b></p>", path));
    if (size > 0)
    {
        d->sizeLabel->setText(i18nc("%1: file counter, %2: disk size with unit",
                                    "The download requires %1 files with a size of %2.",
                                    d->total, sizeString));

        d->buttons->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
    else
    {
        d->sizeLabel->setText(i18n("All files of the selected features were found."));

        d->buttons->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}

//-----------------------------------------------------------------------------

DownloadInfo::DownloadInfo()
{
}

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

DownloadInfo::~DownloadInfo()
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
