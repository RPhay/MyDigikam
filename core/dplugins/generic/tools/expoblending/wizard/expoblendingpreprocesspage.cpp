/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a tool to blend bracketed images.
 *
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2015      by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#include "expoblendingpreprocesspage.h"

// Qt includes

#include <QDir>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QPixmap>
#include <QPushButton>
#include <QCheckBox>
#include <QStyle>
#include <QApplication>
#include <QTextBrowser>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "digikam_debug.h"
#include "alignbinary.h"
#include "expoblendingmanager.h"
#include "expoblendingthread.h"
#include "dlayoutbox.h"
#include "dworkingpixmap.h"

namespace DigikamGenericExpoBlendingPlugin
{

class Q_DECL_HIDDEN ExpoBlendingPreProcessPage::Private
{
public:

    explicit Private()
     :  progressCount (0),
        progressLabel (nullptr),
        progressTimer (nullptr),
        title         (nullptr),
        alignCheckBox (nullptr),
        detailsText   (nullptr),
        progressPix   (nullptr),
        mngr          (nullptr)
    {
    }

    int                  progressCount;
    QLabel*              progressLabel;
    QTimer*              progressTimer;

    QLabel*              title;

    QCheckBox*           alignCheckBox;

    QTextBrowser*        detailsText;

    DWorkingPixmap*      progressPix;

    ExpoBlendingManager* mngr;
};

ExpoBlendingPreProcessPage::ExpoBlendingPreProcessPage(ExpoBlendingManager* const mngr, QWizard* const dlg)
    : DWizardPage(dlg, QString::fromLatin1("<b>%1</b>").arg(i18nc("@title: window", "Pre-Processing Bracketed Images"))),
      d          (new Private)
{
    d->mngr                 = mngr;
    d->progressTimer        = new QTimer(this);
    d->progressPix          = new DWorkingPixmap(this);
    DVBox* const vbox       = new DVBox(this);
    d->title                = new QLabel(vbox);
    d->title->setWordWrap(true);
    d->title->setOpenExternalLinks(true);

    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group      = config->group("ExpoBlending Settings");
    d->alignCheckBox        = new QCheckBox(i18nc("@option: check", "Align bracketed images"), vbox);
    d->alignCheckBox->setChecked(group.readEntry("Auto Alignment", true));

    vbox->setStretchFactor(new QWidget(vbox), 2);

    d->detailsText          = new QTextBrowser(vbox);
    d->detailsText->hide();

    vbox->setStretchFactor(new QWidget(vbox), 2);

    d->progressLabel        = new QLabel(vbox);
    d->progressLabel->setAlignment(Qt::AlignCenter);

    vbox->setStretchFactor(new QWidget(vbox), 10);

    setPageWidget(vbox);

    resetTitle();

    QPixmap leftPix(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("digikam/data/assistant-preprocessing.png")));
    setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));

    connect(d->mngr->thread(), SIGNAL(starting(DigikamGenericExpoBlendingPlugin::ExpoBlendingActionData)),
            this, SLOT(slotExpoBlendingAction(DigikamGenericExpoBlendingPlugin::ExpoBlendingActionData)));

    connect(d->progressTimer, SIGNAL(timeout()),
            this, SLOT(slotProgressTimerDone()));
}

ExpoBlendingPreProcessPage::~ExpoBlendingPreProcessPage()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group      = config->group("ExpoBlending Settings");
    group.writeEntry("Auto Alignment", d->alignCheckBox->isChecked());
    config->sync();

    delete d;
}

void ExpoBlendingPreProcessPage::resetTitle()
{
    d->title->setText(QString::fromUtf8("<qt>"
                                        "<p>%1</p>"
                                        "<p>%2</p>"
                                        "<p>%3</p>"
                                        "<p>%4</p>"
                                        "</qt>")
                      .arg(i18nc("@info", "Now, we will pre-process bracketed images before fusing them."))
                      .arg(i18nc("@info", "Alignment must be performed if you have not used a tripod to take bracketed images. Alignment operations can take a while."))
                      .arg(i18nc("@info", "Pre-processing operations include Raw demosaicing. Raw images will be converted to 16-bit sRGB images with auto-gamma."))
                      .arg(i18nc("@info", "Press the \"Next\" button to start pre-processing.")));

    d->detailsText->hide();
    d->alignCheckBox->show();
}

void ExpoBlendingPreProcessPage::process()
{
    d->title->setText(QString::fromUtf8("<qt>"
                                        "<p>%1</p>"
                                        "<p>%2</p>"
                                        "</qt>")
                                        .arg(i18nc("@info", "Pre-processing is in progress, please wait."))
                                        .arg(i18nc("@info", "This can take a while...")));

    d->alignCheckBox->hide();
    d->progressTimer->start(300);

    connect(d->mngr->thread(), SIGNAL(finished(DigikamGenericExpoBlendingPlugin::ExpoBlendingActionData)),
            this, SLOT(slotExpoBlendingAction(DigikamGenericExpoBlendingPlugin::ExpoBlendingActionData)));

    d->mngr->thread()->setPreProcessingSettings(d->alignCheckBox->isChecked());
    d->mngr->thread()->preProcessFiles(d->mngr->itemsList(), d->mngr->alignBinary().path());

    if (!d->mngr->thread()->isRunning())
    {
        d->mngr->thread()->start();
    }
}

void ExpoBlendingPreProcessPage::cancel()
{
    disconnect(d->mngr->thread(), SIGNAL(finished(DigikamGenericExpoBlendingPlugin::ExpoBlendingActionData)),
               this, SLOT(slotExpoBlendingAction(DigikamGenericExpoBlendingPlugin::ExpoBlendingActionData)));

    d->mngr->thread()->cancel();
    d->progressTimer->stop();
    d->progressLabel->clear();
    resetTitle();
}

void ExpoBlendingPreProcessPage::slotProgressTimerDone()
{
    d->progressLabel->setPixmap(d->progressPix->frameAt(d->progressCount));

    d->progressCount++;

    if (d->progressCount == 8)
    {
        d->progressCount = 0;
    }

    d->progressTimer->start(300);
}

void ExpoBlendingPreProcessPage::slotExpoBlendingAction(const DigikamGenericExpoBlendingPlugin::ExpoBlendingActionData& ad)
{
    QString text;

    if (!ad.starting)           // Something is complete...
    {
        if (!ad.success)        // Something is failed...
        {
            switch (ad.action)
            {
                case(EXPOBLENDING_PREPROCESSING):
                {
                    d->title->setText(QString::fromUtf8("<qt>"
                                                        "<p>%1</p>"
                                                        "<p>%2</p>"
                                                        "<p>%3</p>"
                                                        "</qt>")
                                                        .arg(i18nc("@info", "Pre-processing has failed."))
                                                        .arg(i18nc("@info", "Please check your bracketed images stack..."))
                                                        .arg(i18nc("@info", "See processing messages below.")));

                    d->progressTimer->stop();
                    d->alignCheckBox->hide();
                    d->detailsText->show();
                    d->progressLabel->clear();
                    d->detailsText->setText(ad.message);
                    emit signalPreProcessed(ExpoBlendingItemUrlsMap());
                    break;
                }

                default:
                {
                    qCWarning(DIGIKAM_DPLUGIN_GENERIC_LOG) << "Unknown action";
                    break;
                }
            }
        }
        else                    // Something is done...
        {
            switch (ad.action)
            {
                case(EXPOBLENDING_PREPROCESSING):
                {
                    d->progressTimer->stop();
                    d->progressLabel->clear();
                    emit signalPreProcessed(ad.preProcessedUrlsMap);
                    break;
                }

                default:
                {
                    qCWarning(DIGIKAM_DPLUGIN_GENERIC_LOG) << "Unknown action";
                    break;
                }
            }
        }
    }
}

} // namespace DigikamGenericExpoBlendingPlugin
