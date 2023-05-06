/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-11-14
 * Description : Files downloader
 *
 * SPDX-FileCopyrightText: 2020-2021 by Maik Qualmann <metzpinguin at gmail dot com>
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

    explicit Private()
      : downloadUrls( { QLatin1String("https://files.kde.org/digikam/"),
                        QLatin1String("https://mirror.faigner.de/kde/files/digikam/"),
                        QLatin1String("https://kde-applicationdata.mirrors.omnilance.com/digikam/"),
                        QLatin1String("https://mirrors.ocf.berkeley.edu/kde-applicationdata/digikam/") } ),
        mirrorIndex (0),
        redirects   (0),
        buttons     (nullptr),
        progress    (nullptr),
        faceCheck   (nullptr),
        nameLabel   (nullptr),
        reply       (nullptr),
        netMngr     (nullptr)
    {
    }

    const QStringList      downloadUrls;

    QString                error;

    QList<DownloadInfo>    files;
    DownloadInfo           currentInfo;

    int                    mirrorIndex;
    int                    redirects;

    QDialogButtonBox*      buttons;
    QProgressBar*          progress;
    QCheckBox*             faceCheck;
    QLabel*                nameLabel;

    QNetworkReply*         reply;
    QNetworkAccessManager* netMngr;
};

FilesDownloader::FilesDownloader(QWidget* const parent)
    : QDialog(parent),
      d      (new Private)
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

    d->files << DownloadInfo(QLatin1String("aestheticdetector/"),
                             QLatin1String("weights_inceptionv3_299.pb"),
                             QLatin1String("8923e3daff71c07533b9023ef32c69d8c058d1e0931d76d8b81241a201138538"),
                             88007527
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

FilesDownloader::~FilesDownloader()
{
    if (d->reply)
    {
        d->reply->abort();
        d->reply = nullptr;
    }

    if (d->faceCheck)
    {
        SystemSettings system(qApp->applicationName());

        system.disableFaceEngine = d->faceCheck->isChecked();
        system.saveSettings();
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

    Q_FOREACH (const DownloadInfo& info, d->files)
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
                                                     QDialogButtonBox::Cancel,
                                                     mainWidget);
    d->buttons->button(QDialogButtonBox::Ok)->setDefault(true);
    d->buttons->button(QDialogButtonBox::Ok)->setText(i18n("Download"));
    d->buttons->button(QDialogButtonBox::Ok)->setIcon(QIcon::fromTheme(QLatin1String("edit-download")));

    QString path = QDir::toNativeSeparators(getFacesEnginePath());
    qint64 size  = 0;

    Q_FOREACH (const DownloadInfo& info, d->files)
    {
        if (!downloadExists(info))
        {
            // cppcheck-suppress useStlAlgorithm
            size += info.size;
        }
    }

    QString total             = ItemPropertiesTab::humanReadableBytesCount(size);

    QLabel* const infoLabel   = new QLabel(i18nc("%1: folder path, %2: disk size with unit",
                                                 "<p>For the face engine and red eye removal tool, digiKam "
                                                 "needs some large binary files. Some of these files were "
                                                 "not found. Click 'Download' to begin downloading the "
                                                 "needed files. You can cancel this process, the next time "
                                                 "digiKam is started this dialog will appear again. Face "
                                                 "detection will not work without these files.</p>"
                                                 "<p>The files will be downloaded to %1. Make sure there are "
                                                 "around %2 available. <b>After the successful download you "
                                                 "have to restart digiKam.</b></p>", path, total), mainWidget);

    infoLabel->setWordWrap(true);

    d->faceCheck              = new QCheckBox(i18n("Disable face engine feature and don't ask again"), mainWidget);

    d->progress               = new QProgressBar(mainWidget);
    d->progress->setMinimum(0);
    d->progress->setMaximum(1);
    d->progress->setValue(0);

    d->nameLabel              = new QLabel(mainWidget);

    vBox->addWidget(infoLabel);
    vBox->addStretch(5);
    vBox->addWidget(d->faceCheck);
    vBox->addStretch(1);
    vBox->addWidget(d->nameLabel);
    vBox->addWidget(d->progress);
    vBox->addWidget(d->buttons);

    setLayout(vBox);

    connect(d->buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(slotDownload()));

    connect(d->buttons->button(QDialogButtonBox::Help), SIGNAL(clicked()),
            this, SLOT(slotHelp()));

    connect(d->buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
            this, SLOT(reject()));

    (void)exec();
}

void FilesDownloader::slotDownload()
{
    d->buttons->button(QDialogButtonBox::Ok)->setEnabled(false);

    if (d->error.isEmpty())
    {
        while (!d->files.isEmpty())
        {
            d->currentInfo = d->files.takeFirst();

            if (!downloadExists(d->currentInfo))
            {
                download();

                return;
            }
        }

        QMessageBox::information(this, qApp->applicationName(),
                                 i18n("All files were downloaded successfully."));

        d->faceCheck->setChecked(false);

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
            d->mirrorIndex++;

            if (d->mirrorIndex >= d->downloadUrls.size())
            {
                d->mirrorIndex = 0;
            }

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

    QUrl request(d->downloadUrls.at(d->mirrorIndex) +
                 d->currentInfo.path + d->currentInfo.name);

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
    QString text = QString::fromUtf8("%1 (%2://%3)")
                   .arg(d->currentInfo.name)
                   .arg(url.scheme())
                   .arg(url.host());

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

//-----------------------------------------------------------------------------

DownloadInfo::DownloadInfo()
    : size(0)
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
