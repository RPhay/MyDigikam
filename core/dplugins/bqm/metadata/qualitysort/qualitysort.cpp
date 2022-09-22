/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-09-22
 * Description : assign pick label metadata by image quality batch tool.
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "qualitysort.h"

// Qt includes

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
#include "dimg.h"
#include "imagequalityconfselector.h"
#include "imagequalitycontainer.h"
#include "dlayoutbox.h"

namespace DigikamBqmQualitySortPlugin
{

class Q_DECL_HIDDEN QualitySort::Private
{

public:

    enum Entries
    {
        Title       = 0x01,
        Caption     = 0x02,
        Copyrights  = 0x04,
        UsageTerms  = 0x08
    };

public:

    explicit Private()
      : qualitySelector(nullptr),
        changeSettings (true)
    {
    }

    ImageQualityConfSelector* qualitySelector;

    bool                      changeSettings;
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

    BatchTool::registerSettingsWidget();
}

BatchToolSettings QualitySort::defaultSettings()
{
    BatchToolSettings settings;
    ImageQualityContainer prm;

    settings.insert(QLatin1String("SettingsSelected"),                  ImageQualityConfSelector::DefaultSettings);
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

/*
    d->titleCB->setChecked(settings()[QLatin1String("Title")].toBool());
    d->captionCB->setChecked(settings()[QLatin1String("Caption")].toBool());
    d->copyrightsCB->setChecked(settings()[QLatin1String("Copyrights")].toBool());
    d->usageTermsCB->setChecked(settings()[QLatin1String("UsageTerms")].toBool());
*/

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
/*
    QScopedPointer<DMetadata> meta(new DMetadata);

    if (image().isNull())
    {
        if (!meta->load(inputUrl().toLocalFile()))
        {
            return false;
        }
    }
    else
    {
        meta->setData(image().getMetadata());
    }

    bool titleStage      = settings()[QLatin1String("Title")].toBool();
    bool captionStage    = settings()[QLatin1String("Caption")].toBool();
    bool copyrightsStage = settings()[QLatin1String("Copyrights")].toBool();
    bool usageTermsStage = settings()[QLatin1String("UsageTerms")].toBool();
    QStringList langs    = settings()[QLatin1String("TrLangs")].toStringList();

    Q_FOREACH (const QString& trLang, langs)
    {
        if (titleStage)
        {
            qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "QualitySort Title";
            ret &= insertTranslation(Private::Title, trLang, meta.data());
        }

        if (captionStage)
        {
            qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "QualitySort Caption";
            ret &= insertTranslation(Private::Caption, trLang, meta.data());
        }

        if (copyrightsStage)
        {
            qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "QualitySort Copyrights";
            ret &= insertTranslation(Private::Copyrights, trLang, meta.data());
        }

        if (usageTermsStage)
        {
            qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "QualitySort Usage Terms";
            ret &= insertTranslation(Private::UsageTerms, trLang, meta.data());
        }

        qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Title     :" << meta->getXmpTagStringListLangAlt("Xmp.dc.title", false);
        qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Caption   :" << meta->getXmpTagStringListLangAlt("Xmp.dc.description", false);
        qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "CopyRights:" << meta->getXmpTagStringListLangAlt("Xmp.dc.rights", false);
        qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "UsageTerms:" << meta->getXmpTagStringListLangAlt("Xmp.xmpRights.UsageTerms", false);

        if (image().isNull())
        {
            QFile::remove(outputUrl().toLocalFile());
            ret &= QFile::copy(inputUrl().toLocalFile(), outputUrl().toLocalFile());

            if (ret && (titleStage || captionStage || copyrightsStage || usageTermsStage))
            {
                ret &= meta->save(outputUrl().toLocalFile());
                qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Save metadata to file:" << ret;
            }
        }
        else
        {
            if (titleStage || captionStage || copyrightsStage || usageTermsStage)
            {
                qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Save metadata to image";
                image().setMetadata(meta->data());
            }

            ret &= savefromDImg();
        }
    }
*/
    return ret;
}

} // namespace DigikamBqmQualitySortPlugin
