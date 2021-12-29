/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-05-16
 * Description : dialog to set time stamp of picture files.
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2003-2005 by Jesper Pedersen <blackie at kde dot org>
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (c) 2018-2021 by Maik Qualmann <metzpinguin at gmail dot com>
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

#include "timeadjustdialog.h"

// Qt includes

#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QCursor>
#include <QFile>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMap>
#include <QTimer>
#include <QPointer>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QDateTime>
#include <QTimeEdit>
#include <QToolButton>
#include <QVBoxLayout>
#include <QApplication>
#include <QMessageBox>
#include <QWindow>
#include <QScopedPointer>

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <klocalizedstring.h>

// Local includes

#include "dmetadata.h"
#include "dprogresswdg.h"
#include "digikam_debug.h"
#include "dxmlguiwindow.h"
#include "timeadjustlist.h"
#include "digikam_version.h"
#include "timeadjustthread.h"
#include "timeadjustsettings.h"

namespace DigikamGenericTimeAdjustPlugin
{

class Q_DECL_HIDDEN TimeAdjustDialog::Private
{

public:

    explicit Private()
      : settingsView(nullptr),
        previewTimer(nullptr),
        updateTimer (nullptr),
        progressBar (nullptr),
        listView    (nullptr),
        thread      (nullptr),
        iface       (nullptr)
    {
    }

    TimeAdjustSettings*   settingsView;

    QMap<QUrl, int>       itemsUsedMap;           // Map of item urls and index.

    QTimer*               previewTimer;
    QTimer*               updateTimer;

    DProgressWdg*         progressBar;
    TimeAdjustList*       listView;

    TimeAdjustThread*     thread;

    DInfoInterface*       iface;
};

TimeAdjustDialog::TimeAdjustDialog(QWidget* const parent, DInfoInterface* const iface)
    : DPluginDialog(parent, QLatin1String("Time Adjust Dialog")),
      d            (new Private)
{
    setWindowTitle(i18nc("@title", "Adjust Time & Date"));
    setMinimumSize(900, 500);
    setModal(true);

    d->iface = iface;

    m_buttons->addButton(QDialogButtonBox::Close);
    m_buttons->addButton(QDialogButtonBox::Ok);
    m_buttons->button(QDialogButtonBox::Ok)->setText(i18nc("@action:button", "&Apply"));
    m_buttons->button(QDialogButtonBox::Ok)->setToolTip(i18nc("@info:tooltip", "Write the corrected date and time for each image"));
    m_buttons->button(QDialogButtonBox::Ok)->setIcon(QIcon::fromTheme(QLatin1String("dialog-ok-apply")));

    QWidget* const mainWidget = new QWidget(this);
    QVBoxLayout* const vbx    = new QVBoxLayout(this);
    vbx->addWidget(mainWidget);
    vbx->addWidget(m_buttons);
    setLayout(vbx);

    QGridLayout* const mainLayout = new QGridLayout(mainWidget);
    d->listView                   = new TimeAdjustList(mainWidget);
    d->settingsView               = new TimeAdjustSettings(mainWidget, true);
    d->progressBar                = new DProgressWdg(mainWidget);
    d->progressBar->reset();
    d->progressBar->hide();

    // ----------------------------------------------------------------------------

    mainLayout->addWidget(d->listView,     0, 0, 2, 1);
    mainLayout->addWidget(d->settingsView, 0, 1, 1, 1);
    mainLayout->addWidget(d->progressBar,  1, 1, 1, 1);
    mainLayout->setColumnStretch(0, 10);
    mainLayout->setRowStretch(0, 10);
    mainLayout->setContentsMargins(QMargins());

    // ----------------------------------------------------------------------------

    d->previewTimer = new QTimer(this);
    d->previewTimer->setSingleShot(true);
    d->previewTimer->setInterval(500);

    d->updateTimer = new QTimer(this);
    d->updateTimer->setSingleShot(true);
    d->updateTimer->setInterval(250);

    connect(d->previewTimer, SIGNAL(timeout()),
            this, SLOT(slotPreviewTimer()));

    connect(d->updateTimer, SIGNAL(timeout()),
            this, SLOT(slotUpdateTimer()));

    // -- Thread Slots/Signals ----------------------------------------------

    d->thread = new TimeAdjustThread(this, d->iface);

    connect(d->thread, SIGNAL(signalProcessEnded(QUrl,QDateTime,QDateTime,int)),
            this, SLOT(slotProcessEnded(QUrl,QDateTime,QDateTime,int)));

    connect(d->thread, SIGNAL(signalPreviewReady(QUrl,QDateTime,QDateTime)),
            this, SLOT(slotPreviewReady(QUrl,QDateTime,QDateTime)));

    connect(d->thread, SIGNAL(signalDateTimeForUrl(QUrl,QDateTime,bool)),
            d->iface, SLOT(slotDateTimeForUrl(QUrl,QDateTime,bool)));

    connect(d->progressBar, SIGNAL(signalProgressCanceled()),
            this, SLOT(slotCancelThread()));

    connect(d->thread, SIGNAL(signalProcessStarted(QUrl)),
            this, SLOT(slotProcessStarted(QUrl)));

    connect(d->thread, SIGNAL(finished()),
            this, SLOT(slotThreadFinished()));

    // -- Dialog Slots/Signals -----------------------------------------------

    connect(m_buttons->button(QDialogButtonBox::Close), SIGNAL(clicked()),
            this, SLOT(slotCancelThread()));

    connect(m_buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(slotUpdateTimestamps()));

    connect(d->settingsView, SIGNAL(signalSettingsChangedTool()),
            this, SLOT(slotPreviewTimestamps()));

    connect(d->settingsView, SIGNAL(signalSettingsChanged()),
            this, SLOT(slotPreviewTimestamps()));

    connect(this, SIGNAL(finished(int)),
            this, SLOT(slotDialogFinished()));

    connect (d->listView->listView(), &DItemsListView::itemSelectionChanged,
             [this]()
             {
                 d->settingsView->setCurrentItemUrl(d->listView->getCurrentUrl());
             });

    // -----------------------------------------------------------------------

    setBusy(false);
    readSettings();

    int index = 0;

    d->listView->setIface(d->iface);
    d->listView->loadImagesFromCurrentSelection();

    foreach (const QUrl& url, d->listView->imageUrls())
    {
        d->itemsUsedMap.insert(url, index);
        ++index;
    }

    slotPreviewTimestamps();
}

TimeAdjustDialog::~TimeAdjustDialog()
{
    if (d->thread->isRunning())
    {
        d->thread->cancel();
        d->thread->wait();
    }

    delete d;
}

void TimeAdjustDialog::closeEvent(QCloseEvent* e)
{
    if (d->thread->isRunning())
    {
        d->thread->cancel();
        d->thread->wait();

        e->ignore();

        return;
    }

    slotDialogFinished();

    e->accept();
}

void TimeAdjustDialog::slotDialogFinished()
{
    saveSettings();
}

void TimeAdjustDialog::readSettings()
{
    TimeAdjustContainer prm;

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("Time Adjust Settings"));

    prm.customDate     = group.readEntry(QLatin1String("Custom Date"),                   QDateTime());
    prm.customTime     = group.readEntry(QLatin1String("Custom Time"),                   QDateTime());

    prm.adjustmentType = group.readEntry(QLatin1String("Adjustment Type"),               0);
    prm.adjustmentDays = group.readEntry(QLatin1String("Adjustment Days"),               0);
    prm.adjustmentTime = group.readEntry(QLatin1String("Adjustment Time"),               QDateTime());

    prm.updIfAvailable = group.readEntry(QLatin1String("Update Only If Available Time"), true);
    prm.updFileModDate = group.readEntry(QLatin1String("Update File Modification Time"), true);
    prm.updEXIFModDate = group.readEntry(QLatin1String("Update EXIF Modification Time"), true);
    prm.updEXIFOriDate = group.readEntry(QLatin1String("Update EXIF Original Time"),     true);
    prm.updEXIFDigDate = group.readEntry(QLatin1String("Update EXIF Digitization Time"), true);
    prm.updEXIFThmDate = group.readEntry(QLatin1String("Update EXIF Thumbnail Time"),    true);
    prm.updIPTCDate    = group.readEntry(QLatin1String("Update IPTC Time"),              true);
    prm.updXMPVideo    = group.readEntry(QLatin1String("Update XMP Video Time"),         true);
    prm.updXMPDate     = group.readEntry(QLatin1String("Update XMP Creation Time"),      true);

    prm.dateSource     = group.readEntry(QLatin1String("Use Timestamp Type"),            0);
    prm.metadataSource = group.readEntry(QLatin1String("Meta Timestamp Type"),           0);
    prm.fileDateSource = group.readEntry(QLatin1String("File Timestamp Type"),           0);

    d->settingsView->setSettings(prm);
}

void TimeAdjustDialog::saveSettings()
{
    TimeAdjustContainer prm   = d->settingsView->settings();

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(QLatin1String("Time Adjust Settings"));

    group.writeEntry(QLatin1String("Custom Date"),                   prm.customDate);
    group.writeEntry(QLatin1String("Custom Time"),                   prm.customTime);

    group.writeEntry(QLatin1String("Adjustment Type"),               prm.adjustmentType);
    group.writeEntry(QLatin1String("Adjustment Days"),               prm.adjustmentDays);
    group.writeEntry(QLatin1String("Adjustment Time"),               prm.adjustmentTime);

    group.writeEntry(QLatin1String("Update Only If Available Time"), prm.updIfAvailable);
    group.writeEntry(QLatin1String("Update File Modification Time"), prm.updFileModDate);
    group.writeEntry(QLatin1String("Update EXIF Modification Time"), prm.updEXIFModDate);
    group.writeEntry(QLatin1String("Update EXIF Original Time"),     prm.updEXIFOriDate);
    group.writeEntry(QLatin1String("Update EXIF Digitization Time"), prm.updEXIFDigDate);
    group.writeEntry(QLatin1String("Update EXIF Thumbnail Time"),    prm.updEXIFThmDate);
    group.writeEntry(QLatin1String("Update IPTC Time"),              prm.updIPTCDate);
    group.writeEntry(QLatin1String("Update XMP Video Time"),         prm.updXMPVideo);
    group.writeEntry(QLatin1String("Update XMP Creation Time"),      prm.updXMPDate);

    group.writeEntry(QLatin1String("Use Timestamp Type"),            prm.dateSource);
    group.writeEntry(QLatin1String("Meta Timestamp Type"),           prm.metadataSource);
    group.writeEntry(QLatin1String("File Timestamp Type"),           prm.fileDateSource);
}

void TimeAdjustDialog::slotPreviewTimestamps()
{
    if (d->thread->isRunning())
    {
        d->thread->cancel();
        d->thread->wait();
    }

    d->previewTimer->start();
}

void TimeAdjustDialog::slotUpdateTimestamps()
{
    if (d->thread->isRunning())
    {
        d->thread->cancel();
        d->thread->wait();
    }

    d->updateTimer->start();
}

void TimeAdjustDialog::slotPreviewTimer()
{
    d->listView->setWaitStatus();

    d->thread->setSettings(d->settingsView->settings());
    d->thread->setPreviewDates(d->itemsUsedMap);
    d->thread->start();
}

void TimeAdjustDialog::slotUpdateTimer()
{
    d->listView->setWaitStatus();

    d->progressBar->show();
    d->progressBar->setMaximum(d->itemsUsedMap.keys().size());
    d->progressBar->progressScheduled(i18nc("@info", "Adjust Time and Date"), true, true);
    d->progressBar->progressThumbnailChanged(QIcon::fromTheme(QLatin1String("appointment-new")).pixmap(22, 22));

    d->thread->setSettings(d->settingsView->settings());
    d->thread->setUpdatedDates(d->itemsUsedMap);
    d->thread->start();

    setBusy(true);
}

void TimeAdjustDialog::slotCancelThread()
{
    if (d->thread->isRunning())
    {
        d->thread->cancel();
        d->thread->wait();
    }

    if (m_buttons->button(QDialogButtonBox::Ok)->isEnabled())
    {
        accept();
    }
}

void TimeAdjustDialog::setBusy(bool busy)
{
    if (busy)
    {
        m_buttons->button(QDialogButtonBox::Close)->setText(i18nc("@action", "Cancel"));
        m_buttons->button(QDialogButtonBox::Close)->setIcon(QIcon::fromTheme(QLatin1String("dialog-cancel")));
        m_buttons->button(QDialogButtonBox::Close)->setToolTip(i18nc("@info", "Cancel current operation"));
    }
    else
    {
        m_buttons->button(QDialogButtonBox::Close)->setText(i18nc("@action", "Close"));
        m_buttons->button(QDialogButtonBox::Close)->setIcon(QIcon::fromTheme(QLatin1String("window-close")));
        m_buttons->button(QDialogButtonBox::Close)->setToolTip(i18nc("@info", "Close window"));
    }

    m_buttons->button(QDialogButtonBox::Ok)->setEnabled(!busy);
    d->settingsView->setEnabled(!busy);
}

void TimeAdjustDialog::slotProcessStarted(const QUrl& url)
{
    d->listView->processing(url);
}

void TimeAdjustDialog::slotPreviewReady(const QUrl& url,
                                        const QDateTime& org,
                                        const QDateTime& adj)
{
    d->listView->setStatus(url, org, adj);
}

void TimeAdjustDialog::slotProcessEnded(const QUrl& url,
                                        const QDateTime& org,
                                        const QDateTime& adj, int status)
{
    d->listView->processed(url, (status == TimeAdjustList::NOPROCESS_ERROR));
    d->listView->setStatus(url, org, adj, status);

    d->progressBar->setValue(d->progressBar->value() + 1);
}

void TimeAdjustDialog::slotThreadFinished()
{
    setBusy(false);
    d->progressBar->hide();
    d->progressBar->progressCompleted();
}

} // namespace DigikamGenericTimeAdjustPlugin
