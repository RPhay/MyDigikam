/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-03-27
 * Description : a tool to export items to a local storage
 *
 * Copyright (C) 2006-2009 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019-2020 by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "fcexportwindow.h"

// Qt includes

#include <QCloseEvent>
#include <QMessageBox>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_version.h"
#include "fcexportwidget.h"
#include "ditemslist.h"
#include "fcthread.h"
#include "fctask.h"

namespace DigikamGenericFileCopyPlugin
{

class Q_DECL_HIDDEN FCExportWindow::Private
{
public:

    explicit Private()
      : exportWidget(nullptr),
        thread      (nullptr)
    {
    }

    const static QString TARGET_URL_PROPERTY;
    const static QString TARGET_OVERWRITE;
    const static QString TARGET_BEHAVIOR;
    const static QString CONFIG_GROUP;

    const static QString CHANGE_IMAGE_PROPERTIES;
    const static QString IMAGE_RESIZE;
    const static QString IMAGE_FORMAT;
    const static QString IMAGE_COMPRESSION;
    const static QString REMOVE_METADATA;

    FCExportWidget*      exportWidget;
    FCThread*            thread;
};

const QString FCExportWindow::Private::TARGET_URL_PROPERTY     = QLatin1String("targetUrl");
const QString FCExportWindow::Private::TARGET_OVERWRITE        = QLatin1String("overwrite");
const QString FCExportWindow::Private::TARGET_BEHAVIOR         = QLatin1String("targetBehavior");
const QString FCExportWindow::Private::CONFIG_GROUP            = QLatin1String("FileCopyExport");

const QString FCExportWindow::Private::CHANGE_IMAGE_PROPERTIES = QLatin1String("changeImageProperties");
const QString FCExportWindow::Private::IMAGE_RESIZE            = QLatin1String("imageResize");
const QString FCExportWindow::Private::IMAGE_FORMAT            = QLatin1String("imageFormat");
const QString FCExportWindow::Private::IMAGE_COMPRESSION       = QLatin1String("imageCompression");
const QString FCExportWindow::Private::REMOVE_METADATA         = QLatin1String("removeMetadata");

FCExportWindow::FCExportWindow(DInfoInterface* const iface, QWidget* const /*parent*/)
    : WSToolDialog(nullptr, QLatin1String("FileCopy Export Dialog")),
      d           (new Private)
{
    d->exportWidget = new FCExportWidget(iface, this);
    setMainWidget(d->exportWidget);

    // -- Window setup ------------------------------------------------------

    setWindowTitle(i18n("Export to Local Storage"));
    setModal(false);

    startButton()->setText(i18n("Start export"));
    startButton()->setToolTip(i18n("Start export to the specified target"));

    connect(startButton(), SIGNAL(clicked()),
            this, SLOT(slotCopy()));

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

FCExportWindow::~FCExportWindow()
{
    delete d;
}

void FCExportWindow::slotFinished()
{
    saveSettings();
    d->exportWidget->imagesList()->listView()->clear();
}

void FCExportWindow::closeEvent(QCloseEvent* e)
{
    if (!e)
    {
        return;
    }

    slotFinished();
    e->accept();
}

void FCExportWindow::reactivate()
{
    d->exportWidget->imagesList()->loadImagesFromCurrentSelection();
    show();
}

void FCExportWindow::restoreSettings()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group      = config->group(d->CONFIG_GROUP);

    FCContainer settings;

    settings.destUrl               = group.readEntry(d->TARGET_URL_PROPERTY,     QUrl());
    settings.behavior              = group.readEntry(d->TARGET_BEHAVIOR,         (int)FCContainer::CopyFile);
    settings.imageFormat           = group.readEntry(d->IMAGE_FORMAT,            (int)FCContainer::JPEG);
    settings.imageResize           = group.readEntry(d->IMAGE_RESIZE,            1024);
    settings.imageCompression      = group.readEntry(d->IMAGE_COMPRESSION,       75);
    settings.overwrite             = group.readEntry(d->TARGET_OVERWRITE,        false);
    settings.removeMetadata        = group.readEntry(d->REMOVE_METADATA,         false);
    settings.changeImageProperties = group.readEntry(d->CHANGE_IMAGE_PROPERTIES, false);

    d->exportWidget->setSettings(settings);
}

void FCExportWindow::saveSettings()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group      = config->group(d->CONFIG_GROUP);
    FCContainer settings    = d->exportWidget->getSettings();

    group.writeEntry(d->TARGET_URL_PROPERTY,     settings.destUrl);
    group.writeEntry(d->TARGET_BEHAVIOR,         settings.behavior);
    group.writeEntry(d->IMAGE_FORMAT,            settings.imageFormat);
    group.writeEntry(d->IMAGE_RESIZE,            settings.imageResize);
    group.writeEntry(d->IMAGE_COMPRESSION,       settings.imageCompression);
    group.writeEntry(d->TARGET_OVERWRITE,        settings.overwrite);
    group.writeEntry(d->REMOVE_METADATA,         settings.removeMetadata);
    group.writeEntry(d->CHANGE_IMAGE_PROPERTIES, settings.changeImageProperties);
}

void FCExportWindow::slotImageListChanged()
{
    updateUploadButton();
}

void FCExportWindow::slotTargetUrlChanged(const QUrl & target)
{
    Q_UNUSED(target);
    updateUploadButton();
}

void FCExportWindow::updateUploadButton()
{
    bool listNotEmpty = !d->exportWidget->imagesList()->imageUrls().isEmpty();
    startButton()->setEnabled(listNotEmpty && d->exportWidget->targetUrl().isValid());
/*
    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Updated upload button with listNotEmpty ="
                                     << listNotEmpty << ", targetUrl().isValid() ="
                                     << d->exportWidget->targetUrl().isValid();
*/
}

void FCExportWindow::slotCopyingDone(const QUrl& from, const QUrl& to)
{
    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Copied to:" << to.toLocalFile();

    d->exportWidget->imagesList()->removeItemByUrl(from);
}

void FCExportWindow::slotCopyingFinished()
{
    setEnabled(true);

    if (!d->exportWidget->imagesList()->imageUrls().isEmpty())
    {
        QMessageBox::information(this, i18n("Copy not completed"),
                                 i18n("Some of the items have not been copied "
                                      "and are still in the list. "
                                      "You can retry to copy these items now."));
    }
}

void FCExportWindow::slotCopy()
{
    saveSettings();

    // start copying and react on signals

    setEnabled(false);

    if (d->thread)
    {
        d->thread->cancel();
    }
    else
    {
        d->thread = new FCThread(this);

        connect(d->thread, SIGNAL(finished()),
                this, SLOT(slotCopyingFinished()));

        connect(d->thread, SIGNAL(signalUrlProcessed(QUrl,QUrl)),
                this, SLOT(slotCopyingDone(QUrl,QUrl)));
    }

    d->thread->createCopyJobs(d->exportWidget->imagesList()->imageUrls(),
                              d->exportWidget->getSettings());

    d->thread->start();
}

} // namespace DigikamGenericFileCopyPlugin
