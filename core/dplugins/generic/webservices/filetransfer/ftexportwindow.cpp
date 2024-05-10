/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-09-28
 * Description : a tool to export image to a KIO accessible
 *               location
 *
 * SPDX-FileCopyrightText: 2006-2009 by Johannes Wienke <languitar at semipol dot de>
 * SPDX-FileCopyrightText: 2011-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "ftexportwindow.h"

// Qt includes

#include <QCloseEvent>
#include <QMessageBox>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>
#include <kio/job.h>
#include <kio/copyjob.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_version.h"
#include "ditemslist.h"
#include "ftexportwidget.h"

namespace DigikamGenericFileTransferPlugin
{

class Q_DECL_HIDDEN FTExportWindow::Private
{
public:

    Private() = default;

    const QString TARGET_URL_PROPERTY   = QLatin1String("targetUrl");
    const QString HISTORY_URL_PROPERTY  = QLatin1String("historyUrls");
    const QString CONFIG_GROUP          = QLatin1String("KioExport");

    FTExportWidget* exportWidget        = nullptr;
};

FTExportWindow::FTExportWindow(DInfoInterface* const iface, QWidget* const /*parent*/)
    : WSToolDialog(nullptr, QLatin1String("Kio Export Dialog")),
      d           (new Private)
{
    d->exportWidget = new FTExportWidget(iface, this);
    setMainWidget(d->exportWidget);

    // -- Window setup ------------------------------------------------------

    setWindowTitle(i18nc("@title:window", "Export to Remote Storage"));
    setModal(false);

    startButton()->setText(i18nc("@action:button", "Start Export"));
    startButton()->setToolTip(i18nc("@info:tooltip, button", "Start export to the specified target"));

    connect(startButton(), SIGNAL(clicked()),
            this, SLOT(slotUpload()));

    connect(this, SIGNAL(finished(int)),
            this, SLOT(slotFinished()));

    connect(d->exportWidget->imagesList(), SIGNAL(signalImageListChanged()),
            this, SLOT(slotImageListChanged()));

    connect(d->exportWidget, SIGNAL(signalTargetUrlChanged(QUrl)),
            this, SLOT(slotTargetUrlChanged(QUrl)));

    // -- initial sync ------------------------------------------------------

    restoreSettings();
    updateUploadButton();
}

FTExportWindow::~FTExportWindow()
{
    delete d;
}

void FTExportWindow::slotFinished()
{
    saveSettings();
    d->exportWidget->imagesList()->listView()->clear();
}

void FTExportWindow::closeEvent(QCloseEvent* e)
{
    if (!e)
    {
        return;
    }

    slotFinished();
    e->accept();
}

void FTExportWindow::reactivate()
{
    d->exportWidget->imagesList()->loadImagesFromCurrentSelection();
    show();
}

void FTExportWindow::restoreSettings()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group      = config->group(d->CONFIG_GROUP);
    d->exportWidget->setHistory(group.readEntry(d->HISTORY_URL_PROPERTY, QList<QUrl>()));
    d->exportWidget->setTargetUrl(group.readEntry(d->TARGET_URL_PROPERTY, QUrl()));
}

void FTExportWindow::saveSettings()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group      = config->group(d->CONFIG_GROUP);
    group.writeEntry(d->HISTORY_URL_PROPERTY, d->exportWidget->history());
    group.writeEntry(d->TARGET_URL_PROPERTY,  d->exportWidget->targetUrl().url());
}

void FTExportWindow::slotImageListChanged()
{
    updateUploadButton();
}

void FTExportWindow::slotTargetUrlChanged(const QUrl& target)
{
    Q_UNUSED(target);

    updateUploadButton();
}

void FTExportWindow::updateUploadButton()
{
    bool listNotEmpty = !d->exportWidget->imagesList()->imageUrls().isEmpty();
    startButton()->setEnabled(listNotEmpty && d->exportWidget->targetUrl().isValid());

    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Updated upload button with listNotEmpty = "
                                     << listNotEmpty << ", targetUrl().isValid() = "
                                     << d->exportWidget->targetUrl().isValid();
}

void FTExportWindow::slotCopyingDone(KIO::Job* job,
                                     const QUrl& from,
                                     const QUrl& to,
                                     const QDateTime& mtime,
                                     bool directory,
                                     bool renamed)
{
    Q_UNUSED(job);
    Q_UNUSED(to);
    Q_UNUSED(mtime);
    Q_UNUSED(directory);
    Q_UNUSED(renamed);

    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "copied " << to.toDisplayString();

    d->exportWidget->imagesList()->removeItemByUrl(from);
}

void FTExportWindow::slotCopyingFinished(KJob* job)
{
    Q_UNUSED(job);

    setEnabled(true);

    if (!d->exportWidget->imagesList()->imageUrls().isEmpty())
    {
        QMessageBox::information(this, i18nc("@title:window", "Upload not Completed"),
                                 i18n("Some of the images have not been transferred "
                                      "and are still in the list. "
                                      "You can retry to export these images now."));
    }
}

void FTExportWindow::slotUpload()
{
    saveSettings();

    // start copying and react on signals

    setEnabled(false);
    KIO::CopyJob* const copyJob = KIO::copy(d->exportWidget->imagesList()->imageUrls(),
                                            d->exportWidget->targetUrl());

    connect(copyJob, SIGNAL(copyingDone(KIO::Job*,QUrl,QUrl,QDateTime,bool,bool)),
            this, SLOT(slotCopyingDone(KIO::Job*,QUrl,QUrl,QDateTime,bool,bool)));

    connect(copyJob, SIGNAL(result(KJob*)),
            this, SLOT(slotCopyingFinished(KJob*)));
}

} // namespace DigikamGenericFileTransferPlugin

#include "moc_ftexportwindow.cpp"
