/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-24
 * Description : MJPEG Stream configuration dialog
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "mjpegstreamdlg.h"

// Qt includes

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QApplication>
#include <QMessageBox>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QStyle>
#include <QIcon>
#include <QCheckBox>
#include <QGroupBox>
#include <QTabWidget>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "dinfointerface.h"
#include "ditemslist.h"
#include "dnuminput.h"
#include "dlayoutbox.h"
#include "dxmlguiwindow.h"
#include "workingwidget.h"
#include "mjpegservermngr.h"
#include "vidslidesettings.h"
#include "effectpreview.h"
#include "dcombobox.h"

namespace DigikamGenericMjpegStreamPlugin
{

class Q_DECL_HIDDEN MjpegStreamDlg::Private
{
public:

    explicit Private()
      : dirty(false),
        mngr            (MjpegServerMngr::instance()),
        srvButton       (nullptr),
        srvStatus       (nullptr),
        progress        (nullptr),
        aStats          (nullptr),
        separator       (nullptr),
        iStats          (nullptr),
        startOnStartup  (nullptr),
        albumSupport    (false),
        albumSelector   (nullptr),
        listView        (nullptr),
        iface           (nullptr),
        page            (nullptr),
        buttons         (nullptr),
        streamSettings  (nullptr),
        srvPort         (nullptr),
        delay           (nullptr),
        quality         (nullptr),
        streamLoop      (nullptr),
        typeVal         (nullptr),
        effVal          (nullptr),
        effPreview      (nullptr)
    {
    }

    bool                dirty;
    MjpegServerMngr*    mngr;
    QPushButton*        srvButton;
    QLabel*             srvStatus;
    WorkingWidget*      progress;
    QLabel*             aStats;
    QLabel*             separator;
    QLabel*             iStats;
    QCheckBox*          startOnStartup;
    bool                albumSupport;
    QWidget*            albumSelector;
    DItemsList*         listView;
    DInfoInterface*     iface;
    QWidget*            page;
    QDialogButtonBox*   buttons;
    QWidget*            streamSettings;
    DIntNumInput*       srvPort;
    DIntNumInput*       delay;
    DIntNumInput*       quality;
    QCheckBox*          streamLoop;
    DComboBox*          typeVal;
    DComboBox*          effVal;
    EffectPreview*      effPreview;
    MjpegStreamSettings settings;
};

MjpegStreamDlg::MjpegStreamDlg(QObject* const /*parent*/,
                               DInfoInterface* const iface)
    : DPluginDialog(nullptr, MjpegServerMngr::instance()->configGroupName()),
      d            (new Private)
{
    setWindowTitle(i18nc("@title:window", "Share Files with MJPEG Stream Server"));

    d->iface                 = iface;

    // NOTE: We overwrite the default albums chooser object name for load save check items state between sessions.
    // The goal is not mix these settings with other export tools.

    d->iface->setObjectName(QLatin1String("SetupMjpegStreamIface"));
    d->settings.iface        = iface;

    m_buttons->addButton(QDialogButtonBox::Cancel);
    m_buttons->addButton(QDialogButtonBox::Ok);
    m_buttons->button(QDialogButtonBox::Ok)->setDefault(true);
    d->page                  = new QWidget(this);
    QVBoxLayout* const vbx   = new QVBoxLayout(this);
    vbx->addWidget(d->page);
    vbx->addWidget(m_buttons);
    setLayout(vbx);
    setModal(false);

    // -------------------

    QGridLayout* const grid = new QGridLayout(d->page);
    d->albumSupport         = (d->iface && d->iface->supportAlbums());

    if (d->albumSupport)
    {
        d->albumSelector = d->iface->albumChooser(this);
        grid->addWidget(d->albumSelector, 0, 0, 1, 6);

        connect(d->iface, SIGNAL(signalAlbumChooserSelectionChanged()),
                this, SLOT(slotSelectionChanged()));
    }
    else
    {
        d->listView = new DItemsList(d->page);
        d->listView->setObjectName(QLatin1String("MjpegStream ImagesList"));
        d->listView->setControlButtonsPlacement(DItemsList::ControlButtonsRight);
        d->listView->setIface(d->iface);

        // Add all items currently loaded in application.

        d->listView->loadImagesFromCurrentSelection();

        // Replug the previous shared items list.

        d->listView->slotAddImages(d->mngr->itemsList());
        grid->addWidget(d->listView, 0, 0, 1, 6);

        connect(d->listView, SIGNAL(signalImageListChanged()),
                this, SLOT(slotSelectionChanged()));
    }

    // -------------------

    const int spacing             = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);
    QWidget* const serverSettings = new QWidget(this);
    QGridLayout* const grid3      = new QGridLayout(serverSettings);

    QLabel* const portLbl     = new QLabel(i18nc("@label", "Server Port:"), serverSettings);
    d->srvPort                = new DIntNumInput(serverSettings);
    d->srvPort->setDefaultValue(8080);
    d->srvPort->setRange(1025, 65535, 1);
    d->srvPort->setWhatsThis(i18n("The MJPEG server IP port."));
    portLbl->setBuddy(d->srvPort);

    d->startOnStartup         = new QCheckBox(i18nc("@option", "Start Server at Startup"));
    d->startOnStartup->setWhatsThis(i18nc("@info", "Set this option to turn-on the MJPEG server at application start-up automatically"));
    d->startOnStartup->setChecked(true);

    // ---

    QLabel* const explanation = new QLabel(serverSettings);
    explanation->setOpenExternalLinks(true);
    explanation->setWordWrap(true);
    explanation->setFrameStyle(QFrame::Box | QFrame::Plain);

    explanation->setText(i18nc("@info",
        "The %1 server allows to share items through the local network using a web browser. "
        "Motion JPEG is a video compression format in which each video frame is compressed "
        "separately as a JPEG image. MJPEG streams is a standard which allows network clients to be "
        "connected without additional module. Most major web browsers and players support MJPEG stream. "
        "To access to stream from your browser, just use http://address:port as url, with address "       // krazy:exclude=insecurenet
        "the MJPEG server IP, and port the server port set in this config dialog.",
        QLatin1String("<a href='https://en.wikipedia.org/wiki/Motion_JPEG'>Motion JPEG</a>")));

    d->srvButton              = new QPushButton(this);
    d->srvStatus              = new QLabel(this);
    d->progress               = new WorkingWidget(this);
    d->aStats                 = new QLabel(this);
    d->separator              = new QLabel(QLatin1String(" / "), this);
    d->iStats                 = new QLabel(this);

    grid3->addWidget(portLbl,           0, 0, 1, 1);
    grid3->addWidget(d->srvPort,        0, 1, 1, 1);
    grid3->addWidget(d->startOnStartup, 0, 2, 1, 3);
    grid3->addWidget(d->srvButton,      1, 0, 1, 1);
    grid3->addWidget(d->srvStatus,      1, 1, 1, 1);
    grid3->addWidget(d->aStats,         1, 2, 1, 1);
    grid3->addWidget(d->separator,      1, 3, 1, 1);
    grid3->addWidget(d->iStats,         1, 4, 1, 1);
    grid3->addWidget(d->progress,       1, 5, 1, 1);
    grid3->addWidget(explanation,       2, 0, 1, 5);
    grid3->setSpacing(spacing);

    QTabWidget* const tabView = new QTabWidget(this);
    tabView->insertTab(0, serverSettings, i18n("Server"));

    // ---

    d->streamSettings         = new QWidget(this);
    QGridLayout* const grid2  = new QGridLayout(d->streamSettings);

    QLabel* const delayLbl    = new QLabel(i18nc("@label", "Delay in seconds:"), d->streamSettings);
    d->delay                  = new DIntNumInput(d->streamSettings);
    d->delay->setDefaultValue(5);
    d->delay->setRange(1, 3600, 1);
    d->delay->setWhatsThis(i18n("The delay, in seconds, between images."));
    delayLbl->setBuddy(d->delay);

    QLabel* const qualityLbl  = new QLabel(i18nc("@label", "JPEG Quality:"), d->streamSettings);
    d->quality                = new DIntNumInput(d->streamSettings);
    d->quality->setDefaultValue(75);
    d->quality->setRange(50, 100, 1);
    d->quality->setWhatsThis(i18n("The JPEG quality [50:lower - 100:higher]."));
    qualityLbl->setBuddy(d->quality);

    // ---

    QLabel* const typeLabel   = new QLabel(d->streamSettings);
    typeLabel->setWordWrap(false);
    typeLabel->setText(i18nc("@label", "JPEG Size:"));
    d->typeVal                = new DComboBox(d->streamSettings);
    d->typeVal->combo()->setEditable(false);

    QMap<VidSlideSettings::VidType, QString> map                = VidSlideSettings::videoTypeNames();
    QMap<VidSlideSettings::VidType, QString>::const_iterator it = map.constBegin();

    while (it != map.constEnd())
    {
        d->typeVal->addItem(it.value(), (int)it.key());
        ++it;
    }

    d->typeVal->setDefaultIndex(VidSlideSettings::BLUERAY);
    typeLabel->setBuddy(d->typeVal);

    // ---

    d->streamLoop             = new QCheckBox(i18nc("@option:check", "Stream In Loop"), d->streamSettings);
    d->streamLoop->setChecked(true);
    d->streamLoop->setWhatsThis(i18n("The MJPEG stream will be played in loop instead once."));

    grid2->addWidget(qualityLbl,    0, 0, 1, 1);
    grid2->addWidget(d->quality,    0, 1, 1, 1);
    grid2->addWidget(typeLabel,     0, 3, 1, 1);
    grid2->addWidget(d->typeVal,    0, 4, 1, 1);
    grid2->addWidget(delayLbl,      1, 0, 1, 1);
    grid2->addWidget(d->delay,      1, 1, 1, 1);
    grid2->addWidget(d->streamLoop, 1, 3, 1, 2);
    grid2->setColumnStretch(2, 10);
    grid2->setSpacing(spacing);

    tabView->insertTab(1, d->streamSettings, i18n("Stream"));

    // ---

    DVBox* const effectSettings = new DVBox(this);
    QGroupBox* const effGrp     = new QGroupBox(i18n("Effect While Displaying Images"), effectSettings);
    QLabel* const effLabel      = new QLabel(effGrp);
    effLabel->setWordWrap(false);
    effLabel->setText(i18n("Type:"));
    d->effVal                   = new DComboBox(effGrp);
    d->effVal->combo()->setEditable(false);

    QMap<EffectMngr::EffectType, QString> map6                = EffectMngr::effectNames();
    QMap<EffectMngr::EffectType, QString>::const_iterator it6 = map6.constBegin();

    while (it6 != map6.constEnd())
    {
        d->effVal->insertItem((int)it6.key(), it6.value(), (int)it6.key());
        ++it6;
    }

    d->effVal->setDefaultIndex(EffectMngr::None);
    effLabel->setBuddy(d->effVal);

    QLabel* const effNote  = new QLabel(effGrp);
    effNote->setWordWrap(true);
    effNote->setText(i18n("<i>An effect is an visual panning or zooming applied while an image "
                          "is displayed. The effect duration will follow the number of frames used "
                          "to render the image on video stream.</i>"));

    d->effPreview              = new EffectPreview(effGrp);
    d->effPreview->setImagesList(d->settings.urlsList);

    QGridLayout* const effGrid = new QGridLayout(effGrp);
    effGrid->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    effGrid->addWidget(effLabel,      0, 0, 1, 1);
    effGrid->addWidget(d->effVal,     0, 1, 1, 1);
    effGrid->addWidget(effNote,       1, 0, 1, 2);
    effGrid->addWidget(d->effPreview, 0, 2, 2, 1);
    effGrid->setColumnStretch(1, 10);
    effGrid->setRowStretch(1, 10);

    tabView->insertTab(2, effectSettings, i18n("Effects"));

    grid->addWidget(tabView, 1, 0, 1, 6);
    grid->setColumnStretch(0, 10);
    grid->setRowStretch(0, 10);
    grid->setSpacing(spacing);

    // --------------------------------------------------------

    connect(d->srvButton, SIGNAL(clicked()),
            this, SLOT(slotToggleMjpegServer()));

    connect(m_buttons->button(QDialogButtonBox::Cancel), &QPushButton::clicked,
            this, &MjpegStreamDlg::reject);

    connect(m_buttons->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &MjpegStreamDlg::accept);

    connect(d->srvPort, SIGNAL(valueChanged(int)),
            this, SLOT(slotSettingsChanged()));

    connect(d->delay, SIGNAL(valueChanged(int)),
            this, SLOT(slotSettingsChanged()));

    connect(d->quality, SIGNAL(valueChanged(int)),
            this, SLOT(slotSettingsChanged()));

    connect(d->streamLoop, SIGNAL(stateChanged(int)),
            this, SLOT(slotSettingsChanged()));

    connect(d->typeVal, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSettingsChanged()));

    connect(d->effVal, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSettingsChanged()));

    // -------------------

    readSettings();
}

MjpegStreamDlg::~MjpegStreamDlg()
{
    delete d;
}

void MjpegStreamDlg::accept()
{
    if (d->dirty)
    {
        bool empty = false;

        if (d->albumSupport)
        {
            empty = d->iface->albumChooserItems().isEmpty();
        }
        else
        {
            empty = d->listView->imageUrls().isEmpty();
        }

        if (!empty)
        {
            int rc = QMessageBox::question(this, i18nc("@title", "Media Server Contents"),
                                           i18nc("@info", "The items list to share has changed. "
                                                "Do you want to start now the MJPEG server with this contents?"));
            if (rc == QMessageBox::Yes)
            {
                startMjpegServer();
            }
        }
    }

    saveSettings();
    QDialog::accept();
}

void MjpegStreamDlg::readSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->mngr->configGroupName());

    d->startOnStartup->setChecked(group.readEntry(d->mngr->configStartServerOnStartupEntry(), false));
    d->settings.readSettings(group);

    d->srvPort->blockSignals(true);
    d->delay->blockSignals(true);
    d->quality->blockSignals(true);
    d->streamLoop->blockSignals(true);
    d->typeVal->blockSignals(true);
    d->effVal->blockSignals(true);

    d->srvPort->setValue(d->settings.port);
    d->delay->setValue(d->settings.delay);
    d->quality->setValue(d->settings.quality);
    d->streamLoop->setChecked(d->settings.loop);
    d->typeVal->setCurrentIndex(d->settings.outSize);
    d->effVal->setCurrentIndex(d->settings.effect);

    d->srvPort->blockSignals(false);
    d->delay->blockSignals(false);
    d->quality->blockSignals(false);
    d->streamLoop->blockSignals(false);
    d->typeVal->blockSignals(false);
    d->effVal->blockSignals(false);

    slotSettingsChanged();

    updateServerStatus();
}

void MjpegStreamDlg::saveSettings()
{
    setMjpegServerContents();

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->mngr->configGroupName());
    group.writeEntry(d->mngr->configStartServerOnStartupEntry(), d->startOnStartup->isChecked());
    d->settings.writeSettings(group);
    config->sync();
}

void MjpegStreamDlg::slotSettingsChanged()
{
    d->settings.port    = d->srvPort->value();
    d->settings.delay   = d->delay->value();
    d->settings.quality = d->quality->value();
    d->settings.loop    = d->streamLoop->isChecked();
    d->settings.outSize = d->typeVal->currentIndex();
    d->settings.effect  = (EffectMngr::EffectType)d->effVal->currentIndex();
}

void MjpegStreamDlg::updateServerStatus()
{
    if (d->mngr->isRunning())
    {
        d->srvStatus->setText(i18nc("@label", "Server is running"));
        d->aStats->setText(i18ncp("@info", "1 album shared", "%1 albums shared", d->mngr->albumsShared()));
        d->separator->setVisible(true);
        d->iStats->setText(i18ncp("@info", "1 item shared",  "%1 items shared",  d->mngr->itemsShared()));
        d->srvButton->setText(i18nc("@action: button", "Stop"));
        d->srvButton->setIcon(QIcon::fromTheme(QLatin1String("media-playback-stop")));
        d->progress->toggleTimer(true);
        d->progress->setVisible(true);
    }
    else
    {
        d->srvStatus->setText(i18nc("@label", "Server is not running"));
        d->aStats->clear();
        d->separator->setVisible(false);
        d->iStats->clear();
        d->srvButton->setText(i18nc("@action: button", "Start"));
        d->srvButton->setIcon(QIcon::fromTheme(QLatin1String("media-playback-start")));
        d->progress->toggleTimer(false);
        d->progress->setVisible(false);
    }
}

bool MjpegStreamDlg::setMjpegServerContents()
{
    if (d->albumSupport)
    {
        DInfoInterface::DAlbumIDs albums = d->iface->albumChooserItems();
        MjpegServerMap map;

        foreach (int id, albums)
        {
            DAlbumInfo anf(d->iface->albumInfo(id));
            map.insert(anf.title(), d->iface->albumItems(id));
        }

        if (map.isEmpty())
        {
            QMessageBox::information(this, i18nc("@title", "Media Server Contents"),
                                     i18nc("@info", "There is no collection to share with the current selection..."));
            return false;
        }

        d->mngr->setCollectionMap(map);
    }
    else
    {
        QList<QUrl> urls = d->listView->imageUrls();

        if (urls.isEmpty())
        {
            QMessageBox::information(this, i18nc("@title", "Media Server Contents"),
                                     i18nc("@info", "There is no item to share with the current selection..."));

            return false;
        }

        d->mngr->setItemsList(i18nc("@info", "Shared Items"), urls);
    }

    return true;
}

bool MjpegStreamDlg::startMjpegServer()
{
    if (d->dirty)
    {
        d->dirty = false;
    }

    if (!setMjpegServerContents())
    {
        return false;
    }

    d->mngr->setSettings(d->settings);

    if (!d->mngr->startMjpegServer())
    {
        QMessageBox::warning(this, i18nc("@title", "Starting Media Server"),
                             i18nc("@info", "An error occurs while to start Media Server..."));
    }
    else
    {
        d->mngr->mjpegServerNotification(true);
    }

    updateServerStatus();

    return true;
}

void MjpegStreamDlg::slotSelectionChanged()
{
    d->dirty = true;
}

void MjpegStreamDlg::slotToggleMjpegServer()
{
    bool b = false;

    if (!d->mngr->isRunning())
    {
        if (startMjpegServer())
        {
            b = true;
        }
    }
    else
    {
        d->mngr->cleanUp();
        updateServerStatus();
    }

    d->streamSettings->setDisabled(b);
}

} // namespace DigikamGenericMjpegStreamPlugin
