/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-09-24
 * Description : assign captions metadata batch tool.
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "assigncaptions.h"

// Qt includes

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QStyle>
#include <QLayout>
#include <QCheckBox>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "dimg.h"
#include "dinfointerface.h"
#include "dmetadata.h"
#include "dpluginbqm.h"
#include "altlangstredit.h"
#include "dlayoutbox.h"
#include "captionvalues.h"

namespace DigikamBqmAssignCaptionsPlugin
{

class Q_DECL_HIDDEN AssignCaptions::Private
{
public:

    explicit Private()
      : setTitles     (nullptr),
        titlesWidget  (nullptr),
        setCaptions   (nullptr),
        captionsWidget(nullptr),
        cleanupCB     (nullptr),
        changeSettings(true)
    {
    }

    QCheckBox*          setTitles;
    AltLangStrEdit*     titlesWidget;

    QCheckBox*          setCaptions;
    AltLangStrEdit*     captionsWidget;

    QCheckBox*          cleanupCB;

    bool                changeSettings;
};

AssignCaptions::AssignCaptions(QObject* const parent)
    : BatchTool(QLatin1String("AssignCaptions"), MetadataTool, parent),
      d        (new Private)
{
}

AssignCaptions::~AssignCaptions()
{
    delete d;
}

BatchTool* AssignCaptions::clone(QObject* const parent) const
{
    return new AssignCaptions(parent);
}

void AssignCaptions::registerSettingsWidget()
{
    const int spacing     = qMin(QApplication::style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing),
                                 QApplication::style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing));

    DVBox* const vbox     = new DVBox;
    vbox->setContentsMargins(spacing, spacing, spacing, spacing);

    d->titlesWidget       = new AltLangStrEdit(vbox, 0);
    d->setTitles          = new QCheckBox(i18n("Titles:"), d->titlesWidget);
    d->setTitles->setChecked(false);
    d->titlesWidget->setPlaceholderText(i18nc("@info", "Enter title text here."));
    d->titlesWidget->setTitleWidget(d->setTitles);
    d->titlesWidget->slotEnabledInternalWidgets(false);

    d->captionsWidget     = new AltLangStrEdit(vbox, 0);

    d->setCaptions        = new QCheckBox(i18n("Captions:"), d->captionsWidget);
    d->setCaptions->setChecked(false);
    d->captionsWidget->setPlaceholderText(i18nc("@info", "Enter caption text here."));
    d->captionsWidget->setTitleWidget(d->setCaptions);
    d->captionsWidget->slotEnabledInternalWidgets(false);

    d->cleanupCB          = new QCheckBox(i18n("Cleanup up and overwrite"), vbox);
    d->cleanupCB->setToolTip(i18nc("@info", "If you turn on this options, titles and captions"
                                   "will be cleaned and replaced by the new values,\nelse "
                                   "old values will be merged with new values."));

    m_settingsWidget      = vbox;

    connect(d->setTitles, SIGNAL(toggled(bool)),
            this, SLOT(slotSettingsChanged()));

    connect(d->setTitles, SIGNAL(toggled(bool)),
            d->titlesWidget, SLOT(slotEnabledInternalWidgets(bool)));

    connect(d->titlesWidget, SIGNAL(signalModified(QString,QString)),
            this, SLOT(slotSettingsChanged()));

    connect(d->titlesWidget, SIGNAL(signalValueAdded(QString,QString)),
            this, SLOT(slotSettingsChanged()));

    connect(d->titlesWidget, SIGNAL(signalValueDeleted(QString)),
            this, SLOT(slotSettingsChanged()));

    connect(d->setCaptions, SIGNAL(toggled(bool)),
            this, SLOT(slotSettingsChanged()));

    connect(d->setCaptions, SIGNAL(toggled(bool)),
            d->captionsWidget, SLOT(slotEnabledInternalWidgets(bool)));

    connect(d->captionsWidget, SIGNAL(signalModified(QString,QString)),
            this, SLOT(slotSettingsChanged()));

    connect(d->captionsWidget, SIGNAL(signalValueAdded(QString,QString)),
            this, SLOT(slotSettingsChanged()));

    connect(d->captionsWidget, SIGNAL(signalValueDeleted(QString)),
            this, SLOT(slotSettingsChanged()));

    connect(d->cleanupCB, SIGNAL(toggled(bool)),
            this, SLOT(slotSettingsChanged()));

    BatchTool::registerSettingsWidget();
}

BatchToolSettings AssignCaptions::defaultSettings()
{
    BatchToolSettings settings;

    settings.insert(QLatin1String("SetTitles"),     false);
    settings.insert(QLatin1String("TitleValues"),   QVariant::fromValue(MetaEngine::AltLangMap()));
    settings.insert(QLatin1String("SetCaptions"),   false);
    settings.insert(QLatin1String("CaptionValues"), QVariant::fromValue(MetaEngine::AltLangMap()));
    settings.insert(QLatin1String("CleanUp"),       false);

    return settings;
}

void AssignCaptions::slotAssignSettings2Widget()
{
    d->changeSettings               = false;

    bool setTitles                  = settings()[QLatin1String("SetTitles")].toBool();
    MetaEngine::AltLangMap titles   = qvariant_cast<MetaEngine::AltLangMap>(settings()[QLatin1String("TitleValues")]);
    bool setCaptions                = settings()[QLatin1String("SetCaptions")].toBool();
    MetaEngine::AltLangMap captions = qvariant_cast<MetaEngine::AltLangMap>(settings()[QLatin1String("CaptionValues")]);
    bool cleanup                    = settings()[QLatin1String("CleanUp")].toBool();

    d->setTitles->setChecked(setTitles);
    d->titlesWidget->setValues(titles);
    d->setCaptions->setChecked(setCaptions);
    d->captionsWidget->setValues(captions);
    d->cleanupCB->setChecked(cleanup);

    d->changeSettings               = true;
}

void AssignCaptions::slotSettingsChanged()
{
    if (d->changeSettings)
    {
        BatchToolSettings settings;

        settings.insert(QLatin1String("SetTitles"),     d->setTitles->isChecked());
        settings.insert(QLatin1String("TitleValues"),   QVariant::fromValue(d->titlesWidget->values()));
        settings.insert(QLatin1String("SetCaptions"),   d->setCaptions->isChecked());
        settings.insert(QLatin1String("CaptionValues"), QVariant::fromValue(d->captionsWidget->values()));
        settings.insert(QLatin1String("CleanUp"),       d->cleanupCB->isChecked());

        BatchTool::slotSettingsChanged(settings);
    }
}

bool AssignCaptions::toolOperations()
{
    bool ret = true;
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

    bool setTitles                  = settings()[QLatin1String("SetTitles")].toBool();
    MetaEngine::AltLangMap titles   = qvariant_cast<MetaEngine::AltLangMap>(settings()[QLatin1String("TitleValues")]);
    bool setCaptions                = settings()[QLatin1String("SetCaptions")].toBool();
    MetaEngine::AltLangMap captions = qvariant_cast<MetaEngine::AltLangMap>(settings()[QLatin1String("CaptionValues")]);
    bool cleanup                    = settings()[QLatin1String("CleanUp")].toBool();

    if (setTitles)
    {
        CaptionsMap orgTitlesMap;

        if (!cleanup)
        {
            orgTitlesMap = meta->getItemTitles();
        }

        MetaEngine::AltLangMap orgTitles = orgTitlesMap.toAltLangMap();
        MetaEngine::AltLangMap::const_iterator it;

        for (it = titles.constBegin() ; it != titles.constEnd() ; ++it)
        {
            orgTitles.insert(it.value(), it.key());
        }

        CaptionsMap newTitlesMap;
        newTitlesMap.fromAltLangMap(orgTitles);
        meta->setItemTitles(newTitlesMap);
        qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Assign Titles:" << orgTitles;
    }

    if (setCaptions)
    {
        CaptionsMap orgCaptionsMap;

        if (!cleanup)
        {
            orgCaptionsMap = meta->getItemComments();
        }

        MetaEngine::AltLangMap orgCaptions = orgCaptionsMap.toAltLangMap();
        MetaEngine::AltLangMap::const_iterator it;

        for (it = captions.constBegin() ; it != captions.constEnd() ; ++it)
        {
            orgCaptions.insert(it.value(), it.key());
        }

        CaptionsMap newCaptionsMap;
        newCaptionsMap.fromAltLangMap(orgCaptions);
        meta->setItemComments(newCaptionsMap);
        qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Assign Titles:" << orgCaptions;
    }

    if (image().isNull())
    {
        QFile::remove(outputUrl().toLocalFile());
        ret &= QFile::copy(inputUrl().toLocalFile(), outputUrl().toLocalFile());

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

} // namespace DigikamBqmAssignCaptionsPlugin
