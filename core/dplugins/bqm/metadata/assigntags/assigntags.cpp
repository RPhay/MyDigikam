/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-10-03
 * Description : auto assign tags batch tool.
 *
 * SPDX-FileCopyrightText: 2022-2023 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <QComboBox>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "dimg.h"
#include "dmetadata.h"
#include "dpluginbqm.h"
#include "autotagsassign.h"
#include "tagscache.h"
#include "iteminfo.h"
#include "dlayoutbox.h"

namespace DigikamBqmAssignTagsPlugin
{

class Q_DECL_HIDDEN AssignTags::Private
{
public:

    Private() = default;

    QComboBox* modelSelectionMode = nullptr;
    bool           changeSettings = true;
};

AssignTags::AssignTags(QObject* const parent)
    : BatchTool(QLatin1String("AssignTags"), MetadataTool, parent),
      d        (new Private)
{
}

AssignTags::~AssignTags()
{
    delete d;
}

BatchTool* AssignTags::clone(QObject* const parent) const
{
    return new AssignTags(parent);
}

void AssignTags::registerSettingsWidget()
{
    DVBox* const vbox     = new DVBox;
    QLabel* const lbl     = new QLabel(i18n("Select deep-learning model:"), vbox);
    d->modelSelectionMode = new QComboBox(vbox);
    d->modelSelectionMode->addItem(i18n("YOLOv5 Nano"),   DetectorModel::YOLOV5NANO);
    d->modelSelectionMode->addItem(i18n("YOLOv5 XLarge"), DetectorModel::YOLOV5XLARGE);
    d->modelSelectionMode->addItem(i18n("ResNet50"),      DetectorModel::RESNET50);
    d->modelSelectionMode->setToolTip(i18nc("@info:tooltip",
                                            "<p><b>YOLOv5 Nano</b>: this model is a neural network which offers exceptional speed and efficiency. It enables you to swiftly "
                                            "evaluate the integration of smaller-scale object detection scenarios.</p>"
                                            "<p><b>YOLOv5 XLarge</b>: this model is a neural network dedicated for more complex object detection requirements and "
                                            "showcases remarkable capabilities. Despite the additional complexity introducing more time-latency and "
                                            "computer resources, it's must be used for larger-scale object detection scenarios.</p>"
                                            "<p><b>ResNet50</b>: this model is a specific type of convolutional neural network formed by stacking residual blocks "
                                            "commonly used to power computer vision applications as object detections."));
    m_settingsWidget = vbox;

    connect(d->modelSelectionMode, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSettingsChanged()));

    BatchTool::registerSettingsWidget();
}

BatchToolSettings AssignTags::defaultSettings()
{
    BatchToolSettings settings;

    settings.insert(QLatin1String("AutoTagModel"), (int)DetectorModel::YOLOV5NANO);

    return settings;
}

void AssignTags::slotAssignSettings2Widget()
{
    d->changeSettings = false;

    int model = settings()[QLatin1String("AutoTagModel")].toInt();
    d->modelSelectionMode->setCurrentIdenx(model);

    d->changeSettings = true;
}

void AssignTags::slotSettingsChanged()
{
    if (d->changeSettings)
    {
        BatchToolSettings settings;

        settings.insert(QLatin1String("AutoTagModel"), d->modelSelectionMode->currentIndex());

        BatchTool::slotSettingsChanged(settings);
    }
}

bool AssignTags::toolOperations()
{
    bool ret  = false;
    int model = settings()[QLatin1String("AutoTagModel")].toInt();

    if (!image().isNull())
    {
        AutoTagsAssign* const autotagsEngine = new AutoTagsAssign(DetectorModel(d->modelType));
        QList<QString> tagsList              = autotagsEngine->generateTagsList(dimg);

        if (!tagsList.isEmpty())
        {
            ItemInfo info              = ItemInfo::fromLocalFile(pathImage);
            TagsCache* const tagsCache = Digikam::TagsCache::instance();
            QString rootTags           = QLatin1String("auto/");

            for (const auto& tag : tagsList)
            {
                int tagId = tagsCache->getOrCreateTag(rootTags + tag);
                info.setTag(tagId);
            }
        }

        delete autotagsEngine;
        ret = true;
    }

    return ret;
}

} // namespace DigikamBqmAssignTagsPlugin
