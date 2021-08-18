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
#include "transitionpreview.h"
#include "effectpreview.h"
#include "dcombobox.h"

namespace DigikamGenericMjpegStreamPlugin
{

class Q_DECL_HIDDEN MjpegStreamDlg::Private
{

public:

    enum TabView
    {
        Server = 0,
        Stream,
        Transition,
        Effect
    };

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
        rate            (nullptr),
        quality         (nullptr),
        streamLoop      (nullptr),
        typeVal         (nullptr),
        effVal          (nullptr),
        effPreview      (nullptr),
        transVal        (nullptr),
        transPreview    (nullptr),
        tabView         (nullptr)
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
    DIntNumInput*       rate;
    DIntNumInput*       quality;
    QCheckBox*          streamLoop;
    DComboBox*          typeVal;
    DComboBox*          effVal;
    EffectPreview*      effPreview;
    DComboBox*          transVal;
    TransitionPreview*  transPreview;
    QTabWidget*         tabView;
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

    d->albumSupport   = (d->iface && d->iface->supportAlbums());
    QWidget* itemsSel = nullptr;

    if (d->albumSupport)
    {
        d->albumSelector = d->iface->albumChooser(this);
        itemsSel         = d->albumSelector;

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
        itemsSel = d->listView;

        connect(d->listView, SIGNAL(signalImageListChanged()),
                this, SLOT(slotSelectionChanged()));
    }

    // -------------------

    d->tabView                    = new QTabWidget(this);
    const int spacing             = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);
    QWidget* const serverSettings = new QWidget(d->tabView);

    QLabel* const portLbl         = new QLabel(i18nc("@label", "Server Port:"), serverSettings);
    d->srvPort                    = new DIntNumInput(serverSettings);
    d->srvPort->setDefaultValue(8080);
    d->srvPort->setRange(1025, 65535, 1);
    d->srvPort->setWhatsThis(i18n("The MJPEG server IP port."));
    portLbl->setBuddy(d->srvPort);

    d->startOnStartup             = new QCheckBox(i18nc("@option", "Start Server at Startup"));
    d->startOnStartup->setWhatsThis(i18nc("@info", "Set this option to turn-on the MJPEG server at application start-up automatically"));
    d->startOnStartup->setChecked(true);

    // ---

    QLabel* const explanation     = new QLabel(serverSettings);
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

    d->srvButton                  = new QPushButton(this);
    d->srvStatus                  = new QLabel(this);
    d->progress                   = new WorkingWidget(this);
    d->aStats                     = new QLabel(this);
    d->separator                  = new QLabel(QLatin1String(" / "), this);
    d->iStats                     = new QLabel(this);

    QGridLayout* const grid3      = new QGridLayout(serverSettings);
    grid3->addWidget(portLbl,           0, 0, 1, 1);
    grid3->addWidget(d->srvPort,        0, 1, 1, 1);
    grid3->addWidget(d->startOnStartup, 0, 2, 1, 4);
    grid3->addWidget(d->srvButton,      1, 0, 1, 1);
    grid3->addWidget(d->srvStatus,      1, 1, 1, 1);
    grid3->addWidget(d->aStats,         1, 2, 1, 1);
    grid3->addWidget(d->separator,      1, 3, 1, 1);
    grid3->addWidget(d->iStats,         1, 4, 1, 1);
    grid3->addWidget(d->progress,       1, 5, 1, 1);
    grid3->addWidget(explanation,       2, 0, 1, 6);
    grid3->setSpacing(spacing);

    d->tabView->insertTab(Private::Server, serverSettings, i18n("Server"));

    // ---

    d->streamSettings         = new QWidget(d->tabView);

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

    QLabel* const delayLbl    = new QLabel(i18nc("@label", "Delay in Seconds:"), d->streamSettings);
    d->delay                  = new DIntNumInput(d->streamSettings);
    d->delay->setDefaultValue(5);
    d->delay->setRange(1, 3600, 1);
    d->delay->setWhatsThis(i18n("The delay in seconds between images."));
    delayLbl->setBuddy(d->delay);

    QLabel* const rateLbl     = new QLabel(i18nc("@label", "Frames by Second:"), d->streamSettings);
    d->rate                   = new DIntNumInput(d->streamSettings);
    d->rate->setDefaultValue(10);
    d->rate->setRange(5, 24, 1);
    d->rate->setWhatsThis(i18n("The number of frames by second to render the stream."));
    rateLbl->setBuddy(d->rate);

    d->streamLoop             = new QCheckBox(i18nc("@option:check", "Stream in Loop"), d->streamSettings);
    d->streamLoop->setChecked(true);
    d->streamLoop->setWhatsThis(i18n("The MJPEG stream will be played in loop instead once."));

    QGridLayout* const grid2  = new QGridLayout(d->streamSettings);
    grid2->addWidget(qualityLbl,    0, 0, 1, 1);
    grid2->addWidget(d->quality,    0, 1, 1, 1);
    grid2->addWidget(typeLabel,     1, 0, 1, 1);
    grid2->addWidget(d->typeVal,    1, 1, 1, 1);
    grid2->addWidget(delayLbl,      2, 0, 1, 1);
    grid2->addWidget(d->delay,      2, 1, 1, 1);
    grid2->addWidget(rateLbl,       3, 0, 1, 1);
    grid2->addWidget(d->rate,       3, 1, 1, 1);
    grid2->addWidget(d->streamLoop, 4, 0, 1, 2);

    d->tabView->insertTab(Private::Stream, d->streamSettings, i18n("Stream"));

    // ---

    QWidget* const transitionSettings = new QWidget(d->tabView);

    QLabel* const transLabel          = new QLabel(transitionSettings);
    transLabel->setWordWrap(false);
    transLabel->setText(i18n("Type:"));
    d->transVal                       = new DComboBox(transitionSettings);
    d->transVal->combo()->setEditable(false);

    QMap<TransitionMngr::TransType, QString> map4                = TransitionMngr::transitionNames();
    QMap<TransitionMngr::TransType, QString>::const_iterator it4 = map4.constBegin();

    while (it4 != map4.constEnd())
    {
        d->transVal->addItem(it4.value(), (int)it4.key());
        ++it4;
    }

    d->transVal->setDefaultIndex(TransitionMngr::None);
    transLabel->setBuddy(d->transVal);

    QLabel* const transNote  = new QLabel(transitionSettings);
    transNote->setWordWrap(true);
    transNote->setText(i18n("<i>A transition is an visual effect applied between two images. "
                            "For some effects, the duration can depend of random values and "
                            "can change while the stream.</i>"));

    d->transPreview              = new TransitionPreview(transitionSettings);
    d->transPreview->setImagesList(QList<QUrl>());

    QGridLayout* const transGrid = new QGridLayout(transitionSettings);
    transGrid->setSpacing(spacing);
    transGrid->addWidget(transLabel,      0, 0, 1, 1);
    transGrid->addWidget(d->transVal,     0, 1, 1, 1);
    transGrid->addWidget(transNote,       1, 0, 1, 2);
    transGrid->addWidget(d->transPreview, 0, 2, 2, 1);
    transGrid->setColumnStretch(1, 10);
    transGrid->setRowStretch(1, 10);

    d->tabView->insertTab(Private::Transition, transitionSettings, i18n("Transition"));

    // ---

    QWidget* const effectSettings = new QWidget(d->tabView);

    QLabel* const effLabel        = new QLabel(effectSettings);
    effLabel->setWordWrap(false);
    effLabel->setText(i18n("Type:"));
    d->effVal                     = new DComboBox(effectSettings);
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

    QLabel* const effNote      = new QLabel(effectSettings);
    effNote->setWordWrap(true);
    effNote->setText(i18n("<i>An effect is an visual panning or zooming applied while an image "
                          "is displayed in MJPEG stream.</i>"));

    d->effPreview              = new EffectPreview(effectSettings);
    d->effPreview->setImagesList(QList<QUrl>());

    QGridLayout* const effGrid = new QGridLayout(effectSettings);
    effGrid->setSpacing(spacing);
    effGrid->addWidget(effLabel,      0, 0, 1, 1);
    effGrid->addWidget(d->effVal,     0, 1, 1, 1);
    effGrid->addWidget(effNote,       1, 0, 1, 2);
    effGrid->addWidget(d->effPreview, 0, 2, 2, 1);
    effGrid->setColumnStretch(1, 10);
    effGrid->setRowStretch(1, 10);
    effGrid->setSpacing(spacing);

    d->tabView->insertTab(Private::Effect, effectSettings, i18n("Effect"));

    // --------------------------------------------------------

    QVBoxLayout* const vlay   = new QVBoxLayout(d->page);
    vlay->addWidget(itemsSel);
    vlay->addWidget(d->tabView);
    vlay->setStretchFactor(itemsSel, 10);
    vlay->setStretchFactor(d->tabView, 1);
    vlay->setSpacing(spacing);

    d->tabView->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));


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

    connect(d->rate, SIGNAL(valueChanged(int)),
            this, SLOT(slotSettingsChanged()));

    connect(d->quality, SIGNAL(valueChanged(int)),
            this, SLOT(slotSettingsChanged()));

    connect(d->streamLoop, SIGNAL(stateChanged(int)),
            this, SLOT(slotSettingsChanged()));

    connect(d->typeVal, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSettingsChanged()));

    connect(d->effVal, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSettingsChanged()));

    connect(d->transVal, SIGNAL(currentIndexChanged(int)),
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
    d->rate->blockSignals(true);
    d->quality->blockSignals(true);
    d->streamLoop->blockSignals(true);
    d->typeVal->blockSignals(true);
    d->effVal->blockSignals(true);
    d->transVal->blockSignals(true);

    d->srvPort->setValue(d->settings.port);
    d->delay->setValue(d->settings.delay);
    d->rate->setValue(d->settings.rate);
    d->quality->setValue(d->settings.quality);
    d->streamLoop->setChecked(d->settings.loop);
    d->typeVal->setCurrentIndex(d->settings.outSize);
    d->effVal->setCurrentIndex(d->settings.effect);
    d->transVal->setCurrentIndex(d->settings.transition);

    d->srvPort->blockSignals(false);
    d->delay->blockSignals(false);
    d->rate->blockSignals(false);
    d->quality->blockSignals(false);
    d->streamLoop->blockSignals(false);
    d->typeVal->blockSignals(false);
    d->effVal->blockSignals(false);
    d->transVal->blockSignals(false);

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
    d->settings.port       = d->srvPort->value();
    d->settings.delay      = d->delay->value();
    d->settings.rate       = d->rate->value();
    d->settings.quality    = d->quality->value();
    d->settings.loop       = d->streamLoop->isChecked();
    d->settings.outSize    = d->typeVal->currentIndex();
    d->settings.effect     = (EffectMngr::EffectType)d->effVal->currentIndex();
    d->settings.transition = (TransitionMngr::TransType)d->transVal->currentIndex();
    d->effPreview->stopPreview();
    d->effPreview->startPreview((EffectMngr::EffectType)d->effVal->currentIndex());
    d->transPreview->stopPreview();
    d->transPreview->startPreview((TransitionMngr::TransType)d->transVal->currentIndex());
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

    d->tabView->setTabEnabled(Private::Stream,     !b);
    d->tabView->setTabEnabled(Private::Effect,     !b);
    d->tabView->setTabEnabled(Private::Transition, !b);
    d->srvPort->setDisabled(b);
}

} // namespace DigikamGenericMjpegStreamPlugin
