/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-03-13
 * Description : image files selector dialog.
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "imagedialog.h"

// Qt includes

#include <QLabel>
#include <QPointer>
#include <QVBoxLayout>
#include <QApplication>
#include <QMimeDatabase>
#include <QStyle>
#include <QLocale>
#include <QPixmap>
#include <QPainter>
#include <QScopedPointer>
#include <QToolTip>
#include <QDir>
#include <QTimer>
#include <QWheelEvent>
#include <QKeyEvent>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "drawdecoder.h"
#include "digikam_debug.h"
#include "digikam_globals.h"
#include "thumbnailloadthread.h"
#include "ditemtooltip.h"
#include "dmetadata.h"
#include "loadingdescription.h"
#include "thumbnailsize.h"
#include "dfiledialog.h"

namespace Digikam
{

class Q_DECL_HIDDEN ImageDialogPreview::Private
{
public:

    explicit Private()
      : imageLabel     (nullptr),
        infoLabel      (nullptr),
        thumbLoadThread(nullptr)
    {
    }

    QLabel*              imageLabel;
    QLabel*              infoLabel;

    QUrl                 currentURL;

    ThumbnailLoadThread* thumbLoadThread;
};

ImageDialogPreview::ImageDialogPreview(QWidget* const parent)
    : QScrollArea(parent),
      d          (new Private)
{
    d->thumbLoadThread = ThumbnailLoadThread::defaultThread();

    QVBoxLayout* const vlay  = new QVBoxLayout(this);
    d->imageLabel            = new QLabel(this);
    d->imageLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    d->imageLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    d->infoLabel = new QLabel(this);
    d->infoLabel->setAlignment(Qt::AlignCenter);

    vlay->setContentsMargins(QMargins());
    vlay->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    vlay->addWidget(d->imageLabel);
    vlay->addWidget(d->infoLabel);
    vlay->addStretch();

    connect(d->thumbLoadThread, SIGNAL(signalThumbnailLoaded(LoadingDescription,QPixmap)),
            this, SLOT(slotThumbnail(LoadingDescription,QPixmap)));
}

ImageDialogPreview::~ImageDialogPreview()
{
    delete d;
}

QSize ImageDialogPreview::sizeHint() const
{
    return QSize(256, 256);
}

void ImageDialogPreview::resizeEvent(QResizeEvent*)
{
    QMetaObject::invokeMethod(this, "showPreview", Qt::QueuedConnection);
}

void ImageDialogPreview::showPreview()
{
    QUrl url(d->currentURL);
    slotClearPreview();
    slotShowPreview(url);
}

void ImageDialogPreview::slotShowPreview(const QUrl& url)
{
    if (!url.isValid())
    {
        slotClearPreview();
        return;
    }

    if (url != d->currentURL)
    {
        slotClearPreview();
        d->currentURL    = url;
        d->thumbLoadThread->find(ThumbnailIdentifier(d->currentURL.toLocalFile()));

        QString identify = identifyItem(d->currentURL);

        if (!identify.isEmpty())
        {
            d->infoLabel->setText(identify);
        }
        else
        {
            d->infoLabel->clear();
        }
    }
}

QString ImageDialogPreview::identifyItem(const QUrl& url, const QImage& preview)
{
    QString identify;
    QScopedPointer<DMetadata> meta(new DMetadata);

    // NOTE: even if metadata loading faild, continue to fill information with empty properties.

    (void)meta->load(url.toLocalFile());

    PhotoInfoContainer info      = meta->getPhotographInformation();
    VideoInfoContainer videoInfo = meta->getVideoInformation();

    DToolTipStyleSheet cnt;
    identify = QLatin1String("<qt><center>");
    QString make, model, dateTime, aperture, focalLength, exposureTime, sensitivity;
    QString aspectRatio, audioBitRate, audioChannelType, audioCodec, duration, frameRate, videoCodec;

    if (info.make.isEmpty())
    {
        make = cnt.unavailable;
    }
    else
    {
        make = info.make;
    }

    if (info.model.isEmpty())
    {
        model = cnt.unavailable;
    }
    else
    {
        model = info.model;
    }

    if (!info.dateTime.isValid())
    {
        dateTime = cnt.unavailable;
    }
    else
    {
        dateTime = QLocale().toString(info.dateTime, QLocale::ShortFormat);
    }

    if (info.aperture.isEmpty())
    {
        aperture = cnt.unavailable;
    }
    else
    {
        aperture = info.aperture;
    }

    if (info.focalLength.isEmpty())
    {
        focalLength = cnt.unavailable;
    }
    else
    {
        focalLength = info.focalLength;
    }

    if (info.exposureTime.isEmpty())
    {
        exposureTime = cnt.unavailable;
    }
    else
    {
        exposureTime = info.exposureTime;
    }

    if (info.sensitivity.isEmpty())
    {
        sensitivity = cnt.unavailable;
    }
    else
    {
        sensitivity = i18n("%1 ISO", info.sensitivity);
    }

    if (videoInfo.aspectRatio.isEmpty())
    {
        aspectRatio = cnt.unavailable;
    }
    else
    {
        aspectRatio = videoInfo.aspectRatio;
    }

    if (videoInfo.audioBitRate.isEmpty())
    {
        audioBitRate = cnt.unavailable;
    }
    else
    {
        audioBitRate = videoInfo.audioBitRate;
    }

    if (videoInfo.audioChannelType.isEmpty())
    {
        audioChannelType = cnt.unavailable;
    }
    else
    {
        audioChannelType = videoInfo.audioChannelType;
    }

    if (videoInfo.audioCodec.isEmpty())
    {
        audioCodec = cnt.unavailable;
    }
    else
    {
        audioCodec = videoInfo.audioCodec;
    }

    if (videoInfo.duration.isEmpty())
    {
        duration = cnt.unavailable;
    }
    else
    {
        duration = videoInfo.duration;
    }

    if (videoInfo.frameRate.isEmpty())
    {
        frameRate = cnt.unavailable;
    }
    else
    {
        frameRate = videoInfo.frameRate;
    }

    if (videoInfo.videoCodec.isEmpty())
    {
        videoCodec = cnt.unavailable;
    }
    else
    {
        videoCodec = videoInfo.videoCodec;
    }

    identify += QLatin1String("<table cellspacing=0 cellpadding=0>");

    if (!preview.isNull())
    {
        QImage img = preview.scaled(QSize(64, 64), Qt::KeepAspectRatio);
        QByteArray byteArray;
        QBuffer    buffer(&byteArray);
        img.save(&buffer, "PNG");
        identify += cnt.cellBeg +
                    i18n("<i>Preview:</i>") +
                    cnt.cellMid +
                    QString::fromLatin1("<img src=\"data:image/png;base64,%1\">").arg(QString::fromLatin1(byteArray.toBase64().data())) +
                    cnt.cellEnd;
    }

    identify += cnt.cellBeg + i18n("<i>Make:</i>")              + cnt.cellMid + make                + cnt.cellEnd;
    identify += cnt.cellBeg + i18n("<i>Model:</i>")             + cnt.cellMid + model               + cnt.cellEnd;
    identify += cnt.cellBeg + i18n("<i>Created:</i>")           + cnt.cellMid + dateTime            + cnt.cellEnd;
    identify += cnt.cellBeg + i18n("<i>Aperture:</i>")          + cnt.cellMid + aperture            + cnt.cellEnd;
    identify += cnt.cellBeg + i18n("<i>Focal:</i>")             + cnt.cellMid + focalLength         + cnt.cellEnd;
    identify += cnt.cellBeg + i18n("<i>Exposure:</i>")          + cnt.cellMid + exposureTime        + cnt.cellEnd;
    identify += cnt.cellBeg + i18n("<i>Sensitivity:</i>")       + cnt.cellMid + sensitivity         + cnt.cellEnd;

    if (QMimeDatabase().mimeTypeForFile(url.toLocalFile()).name().startsWith(QLatin1String("video/")) &&
        !videoInfo.isEmpty())
    {
        identify += cnt.cellBeg + i18n("<i>AspectRatio:</i>")       + cnt.cellMid + aspectRatio         + cnt.cellEnd;
        identify += cnt.cellBeg + i18n("<i>AudioBitRate:</i>")      + cnt.cellMid + audioBitRate        + cnt.cellEnd;
        identify += cnt.cellBeg + i18n("<i>AudioChannelType:</i>")  + cnt.cellMid + audioChannelType    + cnt.cellEnd;
        identify += cnt.cellBeg + i18n("<i>AudioCodec:</i>")        + cnt.cellMid + audioCodec          + cnt.cellEnd;
        identify += cnt.cellBeg + i18n("<i>Duration:</i>")          + cnt.cellMid + duration            + cnt.cellEnd;
        identify += cnt.cellBeg + i18n("<i>FrameRate:</i>")         + cnt.cellMid + frameRate           + cnt.cellEnd;
        identify += cnt.cellBeg + i18n("<i>VideoCodec:</i>")        + cnt.cellMid + videoCodec          + cnt.cellEnd;
    }

    identify += QLatin1String("</table></center></qt>");

    return identify;
}

void ImageDialogPreview::slotThumbnail(const LoadingDescription& desc, const QPixmap& pix)
{
    if (QUrl::fromLocalFile(desc.filePath) == d->currentURL)
    {
        QPixmap pixmap;
        QSize   s = d->imageLabel->contentsRect().size();

        if ((s.width() < pix.width()) || (s.height() < pix.height()))
        {
            pixmap = pix.scaled(s, Qt::KeepAspectRatio);
        }
        else
        {
            pixmap = pix;
        }

        d->imageLabel->setPixmap(pixmap);
    }
}

void ImageDialogPreview::slotClearPreview()
{
    d->imageLabel->clear();
    d->infoLabel->clear();
    d->currentURL = QUrl();
}

// ------------------------------------------------------------------------

class Q_DECL_HIDDEN ImageDialogToolTip::Private
{
public:

    explicit Private()
      : view        (nullptr),
        catcher     (nullptr),
        thread      (nullptr)
    {
    }

    QAbstractItemView*     view;
    QModelIndex            index;
    QUrl                   url;
    QImage                 preview;
    ThumbnailImageCatcher* catcher;
    ThumbnailLoadThread*   thread;
};

ImageDialogToolTip::ImageDialogToolTip()
    : DItemToolTip(),
      d           (new Private)
{
    d->thread  = new ThumbnailLoadThread;
    d->thread->setThumbnailSize(64);
    d->thread->setPixmapRequested(false);
    d->catcher = new ThumbnailImageCatcher(d->thread);
}

ImageDialogToolTip::~ImageDialogToolTip()
{
    d->catcher->thread()->stopAllTasks();
    d->catcher->cancel();

    delete d->catcher->thread();
    delete d->catcher;
    delete d;
}

void ImageDialogToolTip::setData(QAbstractItemView* const view,
                                 const QModelIndex& index,
                                 const QUrl& url)
{
    d->view    = view;
    d->index   = index;
    d->url     = url;

    d->catcher->setActive(true);
    d->catcher->thread()->find(ThumbnailIdentifier(d->url.toLocalFile()));
    d->catcher->enqueue();
    QList<QImage> images = d->catcher->waitForThumbnails();

    if (!images.isEmpty())
    {
        d->preview = images.first();
    }
    else
    {
        d->preview = QImage();
    }

    d->catcher->setActive(false);

    if (!d->index.isValid())
    {
        hide();
    }
    else
    {
        updateToolTip();
        reposition();

        if (isHidden() && !toolTipIsEmpty())
        {
            show();
        }
    }
}

QRect ImageDialogToolTip::repositionRect()
{
    if (!d->index.isValid())
    {
        return QRect();
    }

    QRect rect = d->view->visualRect(d->index);
    rect.moveTopLeft(d->view->viewport()->mapToGlobal(rect.topLeft()));

    return rect;
}

QString ImageDialogToolTip::tipContents()
{
    if (!d->index.isValid())
    {
        return QString();
    }

    QString identify = ImageDialogPreview::identifyItem(d->url, d->preview);

    return identify;
}

// ------------------------------------------------------------------------

class Q_DECL_HIDDEN DFileIconProvider::Private
{

public:

    explicit Private()
      : catcher     (nullptr),
        thread      (nullptr)
    {
    }

    ThumbnailImageCatcher* catcher;           ///< Thumbnail thread catcher from main process.
    ThumbnailLoadThread*   thread;            ///< The separated thread to render thumbnail images.
};

DFileIconProvider::DFileIconProvider()
    : QFileIconProvider(),
      d                (new Private)
{
    d->thread  = new ThumbnailLoadThread;
    d->thread->setThumbnailSize(256);
    d->thread->setPixmapRequested(false);
    d->catcher = new ThumbnailImageCatcher(d->thread);
}

DFileIconProvider::~DFileIconProvider()
{
    d->catcher->thread()->stopAllTasks();
    d->catcher->cancel();

    delete d->catcher->thread();
    delete d->catcher;
    delete d;
}

QIcon DFileIconProvider::icon(const QFileInfo& info) const
{
    // We will only process image files

    if (info.isFile() && !info.isSymLink() && !info.isDir() && !info.isRoot())
    {
        QString path    = info.absoluteFilePath();
        qCDebug(DIGIKAM_GENERAL_LOG) << "request thumb icon for " << path;

        QMimeType mtype = QMimeDatabase().mimeTypeForFile(path);
        QString suffix  = info.suffix().toUpper();

        if (mtype.name().startsWith(QLatin1String("image/")) ||
            (suffix == QLatin1String("PGF"))                 ||
            (suffix == QLatin1String("KRA"))                 ||
            (suffix == QLatin1String("CR3"))                 ||
            (suffix == QLatin1String("HEIC"))                ||
            (suffix == QLatin1String("HEIF")))
        {
            // --- Critical section.

            // NOTE: this part run in separated thread.
            //       Do not use QPixmap here, as it's not re-entrant with X11 under Linux.

            d->catcher->setActive(true);    // ---

                d->catcher->thread()->find(ThumbnailIdentifier(path));
                d->catcher->enqueue();
                QList<QImage> images = d->catcher->waitForThumbnails();

                if (!images.isEmpty())
                {
                    // resize and center pixmap on target icon.

                    QPixmap pix = QPixmap::fromImage(images.first());
                    pix         = pix.scaled(QSize(256, 256), Qt::KeepAspectRatio, Qt::FastTransformation);

                    QPixmap icon(QSize(256, 256));
                    icon.fill(Qt::transparent);
                    QPainter p(&icon);
                    p.drawPixmap((icon.width()  - pix.width() )  / 2,
                                 (icon.height() - pix.height())  / 2,
                                 pix);

                    return icon;
                }

            // --- End of critical section.

            d->catcher->setActive(false);   // ---
        }
    }

    // For non-iages, we will use default provider implementation.

    return QFileIconProvider::icon(info);
}

// ------------------------------------------------------------------------

class Q_DECL_HIDDEN ImageDialog::Private
{

public:

    explicit Private()
        : dlg         (nullptr),
          provider    (nullptr),
          showToolTips(true),
          toolTipTimer(nullptr),
          toolTip     (nullptr),
          toolTipView (nullptr)
    {
    }

    QStringList         fileFormats;
    QList<QUrl>         urls;
    DFileDialog*        dlg;
    DFileIconProvider*  provider;
    bool                showToolTips;
    QTimer*             toolTipTimer;
    ImageDialogToolTip* toolTip;
    QAbstractItemView*  toolTipView;
    QModelIndex         toolTipIndex;
    QUrl                toolTipUrl;
};

ImageDialog::ImageDialog(QWidget* const parent, const QUrl& url, bool singleSelect, const QString& caption)
    : QObject(parent),
      d      (new Private)
{
    QString all;
    d->fileFormats = supportedImageMimeTypes(QIODevice::ReadOnly, all);
    qCDebug(DIGIKAM_GENERAL_LOG) << "file formats=" << d->fileFormats;

    d->toolTip       = new ImageDialogToolTip();
    d->toolTipTimer  = new QTimer(this);

    connect(d->toolTipTimer, SIGNAL(timeout()),
            this, SLOT(slotToolTip()));

    d->provider    = new DFileIconProvider();
    d->dlg         = new DFileDialog(parent);
    d->dlg->setWindowTitle(caption);
    d->dlg->setDirectoryUrl(url);
    d->dlg->setIconProvider(d->provider);
    d->dlg->setNameFilters(d->fileFormats);
    d->dlg->selectNameFilter(d->fileFormats.last());
    d->dlg->setAcceptMode(QFileDialog::AcceptOpen);
    d->dlg->setFileMode(singleSelect ? QFileDialog::ExistingFile : QFileDialog::ExistingFiles);

    for (auto* item : d->dlg->findChildren<QAbstractItemView*>())
    {
        item->installEventFilter(this);
        item->setMouseTracking(true);
    }

    if (d->dlg->exec() == QDialog::Accepted)
    {
        d->urls = d->dlg->selectedUrls();
    }
}

ImageDialog::~ImageDialog()
{
    delete d->toolTip;
    delete d->dlg;
    delete d->provider;
    delete d;
}

void ImageDialog::setEnableToolTips(bool val)
{
    d->showToolTips = val;

    if (!val)
    {
        hideToolTip();
    }
}

void ImageDialog::hideToolTip()
{
    d->toolTipIndex = QModelIndex();
    d->toolTipTimer->stop();
    slotToolTip();
}

void ImageDialog::slotToolTip()
{
    d->toolTip->setData(d->toolTipView, d->toolTipIndex, d->toolTipUrl);
}

bool ImageDialog::acceptToolTip(const QUrl& url) const
{
    if (url.isValid())
    {
        QFileInfo info(url.toLocalFile());

        if (info.isFile() && !info.isSymLink() && !info.isDir() && !info.isRoot())
        {
            return true;
        }
    }

    return false;
}

bool ImageDialog::eventFilter(QObject* obj, QEvent* ev)
{
    if (d->dlg)
    {
        QAbstractItemView* const view = dynamic_cast<QAbstractItemView*>(obj);

        if (view)
        {
            if      ((ev->type() == QEvent::HoverMove) && (qApp->mouseButtons() == Qt::NoButton))
            {
                QHoverEvent* const hev = dynamic_cast<QHoverEvent*>(ev);

                if (hev)
                {
                    QModelIndex index = view->indexAt(view->viewport()->mapFromGlobal(QCursor::pos()));

                    if (index.isValid())
                    {
                        QString name = index.data(Qt::DisplayRole).toString();

                        if (!name.isEmpty())
                        {
                            QUrl url = QUrl::fromLocalFile(QDir::fromNativeSeparators(d->dlg->directoryUrl().toLocalFile() +
                                                                                      QLatin1Char('/') + name));

                            if (d->showToolTips)
                            {
                                if (!d->dlg->isActiveWindow())
                                {
                                    hideToolTip();
                                    return false;
                                }

                                if (index != d->toolTipIndex)
                                {
                                    hideToolTip();

                                    if (acceptToolTip(url))
                                    {
                                        d->toolTipView  = view;
                                        d->toolTipIndex = index;
                                        d->toolTipUrl   = url;
                                        d->toolTipTimer->setSingleShot(true);
                                        d->toolTipTimer->start(500);
                                    }
                                }

                                if ((index == d->toolTipIndex) && !acceptToolTip(url))
                                {
                                    hideToolTip();
                                }
                            }

                            return false;
                        }
                    }
                    else
                    {
                         hideToolTip();
                         return false;
                    }
                }
            }
            else if ((ev->type() == QEvent::HoverLeave) ||
                     (ev->type() == QEvent::FocusOut)   ||
                     (ev->type() == QEvent::Wheel)      ||
                     (ev->type() == QEvent::KeyPress)   ||
                     (ev->type() == QEvent::Paint))
            {
                hideToolTip();
                return false;
            }
        }
    }

    // pass the event on to the parent class

    return QObject::eventFilter(obj, ev);
}

QStringList ImageDialog::fileFormats() const
{
    return d->fileFormats;
}

QUrl ImageDialog::url() const
{
    if (d->urls.isEmpty())
    {
        return QUrl();
    }

    return d->urls.first();
}

QList<QUrl> ImageDialog::urls() const
{
    return d->urls;
}

QUrl ImageDialog::getImageURL(QWidget* const parent, const QUrl& url, const QString& caption)
{
    ImageDialog dlg(parent, url, true, caption.isEmpty() ? i18n("Select an Item") : caption);

    if (dlg.url() != QUrl())
    {
        return dlg.url();
    }
    else
    {
        return QUrl();
    }
}

QList<QUrl> ImageDialog::getImageURLs(QWidget* const parent, const QUrl& url, const QString& caption)
{
    ImageDialog dlg(parent, url, false, caption.isEmpty() ? i18n("Select Items") : caption);

    if (!dlg.urls().isEmpty())
    {
        return dlg.urls();
    }
    else
    {
        return QList<QUrl>();
    }
}

} // namespace Digikam
