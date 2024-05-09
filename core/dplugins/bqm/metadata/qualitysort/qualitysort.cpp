/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-09-22
 * Description : assign pick label metadata by image quality batch tool.
 *
 * SPDX-FileCopyrightText: 2021-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "qualitysort.h"

// Qt includes

#include <QApplication>
#include <QGridLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include <QFile>
#include <QMenu>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "dimg.h"
#include "dinfointerface.h"
#include "dmetadata.h"
#include "dpluginbqm.h"
#include "imagequalityconfselector.h"
#include "imagequalitycontainer.h"
#include "imagequalityparser.h"
#include "dlayoutbox.h"
#include "previewloadthread.h"
#include "dfileoperations.h"

namespace DigikamBqmQualitySortPlugin
{

class Q_DECL_HIDDEN QualitySort::Private
{
public:

    Private() = default;

    ImageQualityConfSelector* qualitySelector   = nullptr;
    ImageQualityParser*       imgqsort          = nullptr;

    bool                      changeSettings    = true;
};

QualitySort::QualitySort(QObject* const parent)
    : BatchTool(QLatin1String("QualitySort"), MetadataTool, parent),
      d        (new Private)
{
}

QualitySort::~QualitySort()
{
    delete d;
}

BatchTool* QualitySort::clone(QObject* const parent) const
{
    return new QualitySort(parent);
}

void QualitySort::registerSettingsWidget()
{
    DVBox* const vbox  = new DVBox;
    d->qualitySelector = new ImageQualityConfSelector(vbox);

    m_settingsWidget   = vbox;

    connect(d->qualitySelector, SIGNAL(signalSettingsChanged()),
            this, SLOT(slotSettingsChanged()));

    connect(d->qualitySelector, SIGNAL(signalQualitySetup()),
            this, SLOT(slotQualitySetup()));

    BatchTool::registerSettingsWidget();
}

void QualitySort::slotQualitySetup()
{
    DInfoInterface* const iface = plugin()->infoIface();

    if (iface)
    {
        if (d->qualitySelector)
        {
            connect(iface, SIGNAL(signalSetupChanged()),
                    this, SLOT(slotSettingsChanged()));
        }

        iface->openSetupPage(DInfoInterface::ImageQualityPage);
    }
}

BatchToolSettings QualitySort::defaultSettings()
{
    BatchToolSettings settings;
    ImageQualityContainer prm;

    settings.insert(QLatin1String("SettingsSelected"),                  ImageQualityConfSelector::GlobalSettings);
    settings.insert(QLatin1String("CustomSettingsDetectBlur"),          prm.detectBlur);
    settings.insert(QLatin1String("CustomSettingsDetectNoise"),         prm.detectNoise);
    settings.insert(QLatin1String("CustomSettingsDetectCompression"),   prm.detectCompression);
    settings.insert(QLatin1String("CustomSettingsDetectExposure"),      prm.detectExposure);
    settings.insert(QLatin1String("CustomSettingsDetectAesthetic"),     prm.detectAesthetic);
    settings.insert(QLatin1String("CustomSettingsLowQRejected"),        prm.lowQRejected);
    settings.insert(QLatin1String("CustomSettingsMediumQPending"),      prm.mediumQPending);
    settings.insert(QLatin1String("CustomSettingsHighQAccepted"),       prm.highQAccepted);
    settings.insert(QLatin1String("CustomSettingsRejectedThreshold"),   prm.rejectedThreshold);
    settings.insert(QLatin1String("CustomSettingsPendingThreshold"),    prm.pendingThreshold);
    settings.insert(QLatin1String("CustomSettingsAcceptedThreshold"),   prm.acceptedThreshold);
    settings.insert(QLatin1String("CustomSettingsBlurWeight"),          prm.blurWeight);
    settings.insert(QLatin1String("CustomSettingsNoiseWeight"),         prm.noiseWeight);
    settings.insert(QLatin1String("CustomSettingsCompressionWeight"),   prm.compressionWeight);
    settings.insert(QLatin1String("CustomSettingsExposureWeight"),      prm.exposureWeight);

    return settings;
}

void QualitySort::slotAssignSettings2Widget()
{
    d->changeSettings = false;

    ImageQualityContainer prm;

    d->qualitySelector->setSettingsSelected((ImageQualityConfSelector::SettingsType)
                                            settings()[QLatin1String("SettingsSelected")].toInt());

    prm.detectBlur          = settings()[QLatin1String("CustomSettingsDetectBlur")].toBool();
    prm.detectNoise         = settings()[QLatin1String("CustomSettingsDetectNoise")].toBool();
    prm.detectCompression   = settings()[QLatin1String("CustomSettingsDetectCompression")].toBool();
    prm.detectExposure      = settings()[QLatin1String("CustomSettingsDetectExposure")].toBool();
    prm.detectAesthetic     = settings()[QLatin1String("CustomSettingsDetectAesthetic")].toBool();
    prm.lowQRejected        = settings()[QLatin1String("CustomSettingsLowQRejected")].toBool();
    prm.mediumQPending      = settings()[QLatin1String("CustomSettingsMediumQPending")].toBool();
    prm.highQAccepted       = settings()[QLatin1String("CustomSettingsHighQAccepted")].toBool();
    prm.rejectedThreshold   = settings()[QLatin1String("CustomSettingsRejectedThreshold")].toInt();
    prm.pendingThreshold    = settings()[QLatin1String("CustomSettingsPendingThreshold")].toInt();
    prm.acceptedThreshold   = settings()[QLatin1String("CustomSettingsAcceptedThreshold")].toInt();
    prm.blurWeight          = settings()[QLatin1String("CustomSettingsBlurWeight")].toInt();
    prm.noiseWeight         = settings()[QLatin1String("CustomSettingsNoiseWeight")].toInt();
    prm.compressionWeight   = settings()[QLatin1String("CustomSettingsCompressionWeight")].toInt();
    prm.exposureWeight      = settings()[QLatin1String("CustomSettingsExposureWeight")].toInt();

    d->qualitySelector->setCustomSettings(prm);

    d->changeSettings = true;
}

void QualitySort::slotSettingsChanged()
{
    if (d->changeSettings)
    {
        BatchToolSettings settings;
        ImageQualityContainer prm = d->qualitySelector->customSettings();

        settings.insert(QLatin1String("SettingsSelected"),                  d->qualitySelector->settingsSelected());
        settings.insert(QLatin1String("CustomSettingsDetectBlur"),          prm.detectBlur);
        settings.insert(QLatin1String("CustomSettingsDetectNoise"),         prm.detectNoise);
        settings.insert(QLatin1String("CustomSettingsDetectCompression"),   prm.detectCompression);
        settings.insert(QLatin1String("CustomSettingsDetectExposure"),      prm.detectExposure);
        settings.insert(QLatin1String("CustomSettingsDetectAesthetic"),     prm.detectAesthetic);
        settings.insert(QLatin1String("CustomSettingsLowQRejected"),        prm.lowQRejected);
        settings.insert(QLatin1String("CustomSettingsMediumQPending"),      prm.mediumQPending);
        settings.insert(QLatin1String("CustomSettingsHighQAccepted"),       prm.highQAccepted);
        settings.insert(QLatin1String("CustomSettingsRejectedThreshold"),   prm.rejectedThreshold);
        settings.insert(QLatin1String("CustomSettingsPendingThreshold"),    prm.pendingThreshold);
        settings.insert(QLatin1String("CustomSettingsAcceptedThreshold"),   prm.acceptedThreshold);
        settings.insert(QLatin1String("CustomSettingsBlurWeight"),          prm.blurWeight);
        settings.insert(QLatin1String("CustomSettingsNoiseWeight"),         prm.noiseWeight);
        settings.insert(QLatin1String("CustomSettingsCompressionWeight"),   prm.compressionWeight);
        settings.insert(QLatin1String("CustomSettingsExposureWeight"),      prm.exposureWeight);

        BatchTool::slotSettingsChanged(settings);
    }
}

bool QualitySort::toolOperations()
{
    bool ret = true;
    QScopedPointer<DMetadata> meta(new DMetadata);
    DImg dimg;

    if (image().isNull())
    {
        if (!meta->load(inputUrl().toLocalFile()))
        {
            return false;
        }

        dimg = PreviewLoadThread::loadFastSynchronously(inputUrl().toLocalFile(), 1024);
    }
    else
    {
        meta->setData(image().getMetadata());
        QSize scaledSize = image().size();
        scaledSize.scale(1024, 1024, Qt::KeepAspectRatio);
        dimg             = image().smoothScale(scaledSize.width(), scaledSize.height());
    }

    ImageQualityConfSelector::SettingsType type = (ImageQualityConfSelector::SettingsType)
                                                  settings()[QLatin1String("SettingsSelected")].toInt();
    ImageQualityContainer prm;

    if (type == ImageQualityConfSelector::GlobalSettings)
    {
        prm.readFromConfig();
    }
    else
    {
        prm.detectBlur          = settings()[QLatin1String("CustomSettingsDetectBlur")].toBool();
        prm.detectNoise         = settings()[QLatin1String("CustomSettingsDetectNoise")].toBool();
        prm.detectCompression   = settings()[QLatin1String("CustomSettingsDetectCompression")].toBool();
        prm.detectExposure      = settings()[QLatin1String("CustomSettingsDetectExposure")].toBool();
        prm.detectAesthetic     = settings()[QLatin1String("CustomSettingsDetectAesthetic")].toBool();
        prm.lowQRejected        = settings()[QLatin1String("CustomSettingsLowQRejected")].toBool();
        prm.mediumQPending      = settings()[QLatin1String("CustomSettingsMediumQPending")].toBool();
        prm.highQAccepted       = settings()[QLatin1String("CustomSettingsHighQAccepted")].toBool();
        prm.rejectedThreshold   = settings()[QLatin1String("CustomSettingsRejectedThreshold")].toInt();
        prm.pendingThreshold    = settings()[QLatin1String("CustomSettingsPendingThreshold")].toInt();
        prm.acceptedThreshold   = settings()[QLatin1String("CustomSettingsAcceptedThreshold")].toInt();
        prm.blurWeight          = settings()[QLatin1String("CustomSettingsBlurWeight")].toInt();
        prm.noiseWeight         = settings()[QLatin1String("CustomSettingsNoiseWeight")].toInt();
        prm.compressionWeight   = settings()[QLatin1String("CustomSettingsCompressionWeight")].toInt();
        prm.exposureWeight      = settings()[QLatin1String("CustomSettingsExposureWeight")].toInt();
    }

    PickLabel pick;
    d->imgqsort = new ImageQualityParser(dimg, prm, &pick);
    d->imgqsort->startAnalyse();

    meta->setItemPickLabel(pick);
    qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Assign Picklabel:" << pick;

    delete d->imgqsort;
    d->imgqsort = nullptr;

    if (image().isNull())
    {
        QFile::remove(outputUrl().toLocalFile());
        ret &= DFileOperations::copyFile(inputUrl().toLocalFile(), outputUrl().toLocalFile());

        if (ret)
        {
            ret &= meta->save(outputUrl().toLocalFile());
            qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Save metadata to file:" << ret;
        }
    }
    else
    {
        qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Save metadata to image";
        image().setMetadata(meta->data());
        ret &= savefromDImg();
    }

    return ret;
}

void QualitySort::cancel()
{
    if (d->imgqsort)
    {
        d->imgqsort->cancelAnalyse();
    }

    BatchTool::cancel();
}

} // namespace DigikamBqmQualitySortPlugin

#include "moc_qualitysort.cpp"
