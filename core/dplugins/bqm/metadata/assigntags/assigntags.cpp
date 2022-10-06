/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-10-03
 * Description : assign tags metadata batch tool.
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "assigntags.h"

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
#include "disjointmetadata.h"
#include "dinfointerface.h"
#include "dmetadata.h"
#include "dpluginbqm.h"
#include "tagsedit.h"
#include "fileactionmngr.h"
#include "dfileoperations.h"
#include "iteminfolist.h"
#include "iteminfo.h"
#include "dlayoutbox.h"

namespace DigikamBqmAssignTagsPlugin
{

class Q_DECL_HIDDEN AssignTags::Private
{
public:

    explicit Private()
      : hub           (nullptr),
        tagsEdit      (nullptr),
        changeSettings(true)
    {
    }

    DisjointMetadata* hub;
    TagsEdit*         tagsEdit;

    bool              changeSettings;
};

AssignTags::AssignTags(QObject* const parent)
    : BatchTool(QLatin1String("AssignTags"), MetadataTool, parent),
      d        (new Private)
{
    d->hub = new DisjointMetadata;
}

AssignTags::~AssignTags()
{
    delete d->hub;
    delete d;
}

BatchTool* AssignTags::clone(QObject* const parent) const
{
    return new AssignTags(parent);
}

void AssignTags::registerSettingsWidget()
{
    d->tagsEdit      = new TagsEdit(d->hub, nullptr);
    m_settingsWidget = d->tagsEdit;

    connect(d->tagsEdit, SIGNAL(signalChanged()),
            this, SLOT(slotSettingsChanged()));

    BatchTool::registerSettingsWidget();
}

BatchToolSettings AssignTags::defaultSettings()
{
    BatchToolSettings settings;
/*
    settings.insert(QLatin1String("SetPick"),     false);
    settings.insert(QLatin1String("PickLabel"),   (int)NoPickLabel);
    settings.insert(QLatin1String("SetColor"),    false);
    settings.insert(QLatin1String("ColorLabel"),  (int)NoColorLabel);
    settings.insert(QLatin1String("SetRating"),   false);
    settings.insert(QLatin1String("RatingValue"), (int)NoRating);
*/
    return settings;
}

void AssignTags::slotAssignSettings2Widget()
{
    d->changeSettings = false;
/*
    bool setPick     = settings()[QLatin1String("SetPick")].toBool();
    PickLabel pick   = (PickLabel)settings()[QLatin1String("PickLabel")].toInt();
    bool setColor    = settings()[QLatin1String("SetColor")].toBool();
    ColorLabel color = (ColorLabel)settings()[QLatin1String("ColorLabel")].toInt();
    bool setRating   = settings()[QLatin1String("SetRating")].toBool();
    int rating       = settings()[QLatin1String("RatingValue")].toInt();

    d->setPick->setChecked(setPick);
    d->pickLabelSelector->setPickLabel(pick);
    d->setColor->setChecked(setColor);
    d->colorLabelSelector->setColorLabel(color);
    d->setRating->setChecked(setRating);
    d->ratingWidget->setRating(rating);
*/
    d->changeSettings = true;
}

void AssignTags::slotSettingsChanged()
{
    if (d->changeSettings)
    {
        BatchToolSettings settings;
/*
        settings.insert(QLatin1String("SetPick"),     d->setPick->isChecked());
        settings.insert(QLatin1String("PickLabel"),   (int)d->pickLabelSelector->pickLabel());
        settings.insert(QLatin1String("SetColor"),    d->setColor->isChecked());
        settings.insert(QLatin1String("ColorLabel"),  (int)d->colorLabelSelector->colorLabel());
        settings.insert(QLatin1String("SetRating"),   d->setRating->isChecked());
        settings.insert(QLatin1String("RatingValue"), d->ratingWidget->rating());
*/
        BatchTool::slotSettingsChanged(settings);
    }
}

bool AssignTags::toolOperations()
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

    bool setPick     = settings()[QLatin1String("SetPick")].toBool();
    PickLabel pick   = (PickLabel)settings()[QLatin1String("PickLabel")].toInt();
    bool setColor    = settings()[QLatin1String("SetColor")].toBool();
    ColorLabel color = (ColorLabel)settings()[QLatin1String("ColorLabel")].toInt();
    bool setRating   = settings()[QLatin1String("SetRating")].toBool();
    int rating       = settings()[QLatin1String("RatingValue")].toInt();

    if (setPick)
    {
        meta->setItemPickLabel(pick);
        qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Assign Picklabel:" << pick;
    }

    if (setColor)
    {
        meta->setItemColorLabel(color);
        qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Assign Colorlabel:" << color;
    }

    if (setRating)
    {
        meta->setItemRating(rating);
        qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Assign Rating:" << rating;
    }

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
*/
    return ret;
}

} // namespace DigikamBqmAssignTagsPlugin
