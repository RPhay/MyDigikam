/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-24
 * Description : MJPEG Stream configuration dialog - Settings methods
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

#include "mjpegstreamdlg_p.h"

namespace DigikamGenericMjpegStreamPlugin
{

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
    d->showDate->blockSignals(true);
    d->showApertureFocal->blockSignals(true);
    d->showCapIfNoTitle->blockSignals(true);
    d->showComment->blockSignals(true);
    d->showExpoSensitivity->blockSignals(true);
    d->showLensModel->blockSignals(true);
    d->showMakeModel->blockSignals(true);
    d->showName->blockSignals(true);
    d->showTags->blockSignals(true);
    d->showRating->blockSignals(true);
    d->showTitle->blockSignals(true);
    d->osdFont->blockSignals(true);


    d->srvPort->setValue(d->settings.port);
    d->delay->setValue(d->settings.delay);
    d->rate->setValue(d->settings.rate);
    d->quality->setValue(d->settings.quality);
    d->streamLoop->setChecked(d->settings.loop);
    d->typeVal->setCurrentIndex(d->settings.outSize);
    d->effVal->setCurrentIndex(d->settings.effect);
    d->transVal->setCurrentIndex(d->settings.transition);
    d->showDate->setChecked(d->settings.printDate);
    d->showApertureFocal->setChecked(d->settings.printApertureFocal);
    d->showCapIfNoTitle->setChecked(d->settings.printCapIfNoTitle);
    d->showComment->setChecked(d->settings.printComment);
    d->showExpoSensitivity->setChecked(d->settings.printExpoSensitivity);
    d->showLensModel->setChecked(d->settings.printLensModel);
    d->showMakeModel->setChecked(d->settings.printMakeModel);
    d->showName->setChecked(d->settings.printName);
    d->showTags->setChecked(d->settings.printTags);
    d->showRating->setChecked(d->settings.printRating);
    d->showTitle->setChecked(d->settings.printTitle);
    d->osdFont->setFont(d->settings.osdFont);

    d->srvPort->blockSignals(false);
    d->delay->blockSignals(false);
    d->rate->blockSignals(false);
    d->quality->blockSignals(false);
    d->streamLoop->blockSignals(false);
    d->typeVal->blockSignals(false);
    d->effVal->blockSignals(false);
    d->transVal->blockSignals(false);
    d->showDate->blockSignals(false);
    d->showApertureFocal->blockSignals(false);
    d->showCapIfNoTitle->blockSignals(false);
    d->showComment->blockSignals(false);
    d->showExpoSensitivity->blockSignals(false);
    d->showLensModel->blockSignals(false);
    d->showMakeModel->blockSignals(false);
    d->showName->blockSignals(false);
    d->showTags->blockSignals(false);
    d->showRating->blockSignals(false);
    d->showTitle->blockSignals(false);
    d->osdFont->blockSignals(false);

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
    d->effPreview->startPreview(d->settings.effect);
    d->transPreview->stopPreview();
    d->transPreview->startPreview(d->settings.transition);
}

} // namespace DigikamGenericMjpegStreamPlugin
