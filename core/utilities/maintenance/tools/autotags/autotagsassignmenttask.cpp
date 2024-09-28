/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-09-02
 * Description : Thread actions task for autotags assignment.
 *
 * SPDX-FileCopyrightText: 2023 by Quoc Hung Tran <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "autotagsassignmenttask.h"

// Local includes

#include "digikam_debug.h"
#include "previewloadthread.h"
#include "localizeselector.h"
#include "maintenancedata.h"
#include "autotagsassign.h"
#include "scancontroller.h"
#include "metadatahub.h"
#include "tagscache.h"
#include "iteminfo.h"
#include "dimg.h"

namespace Digikam
{

class Q_DECL_HIDDEN AutotagsAssignmentTask::Private
{
public:

    Private() = default;

    const int loadCount        = 3;
    const int batchSize        = 16;
    MaintenanceData* data      = nullptr;
    QStringList      langs;
    int              modelType = DetectorModel::YOLOV5NANO;
};

// -------------------------------------------------------

AutotagsAssignmentTask::AutotagsAssignmentTask()
    : ActionJob(),
      d        (new Private)
{
}

AutotagsAssignmentTask::~AutotagsAssignmentTask()
{
    cancel();

    delete d;
}

void AutotagsAssignmentTask::setMaintenanceData(MaintenanceData* const data)
{
    d->data = data;
}

void AutotagsAssignmentTask::setLanguages(const QStringList& langs)
{
    d->langs = langs;
}

void AutotagsAssignmentTask::setModelType(int modelType)
{
    d->modelType = modelType;
}

void AutotagsAssignmentTask::assignTags(const QString& pathImage, const QList<QString>& tagsList)
{
    bool tagsChanged           = false;
    const QString rootTag      = QLatin1String("auto/");
    TagsCache* const tagsCache = Digikam::TagsCache::instance();
    const int rootTagId        = tagsCache->getOrCreateTag(rootTag);
    ItemInfo info              = ItemInfo::fromLocalFile(pathImage);

    // Clear auto-tags

    const auto ids = info.tagIds();

    for (int tid : ids)
    {
        if (tagsCache->parentTags(tid).contains(rootTagId))
        {
            info.removeTag(tid);
            tagsChanged = true;
        }
    }

    for (const auto& tag : tagsList)
    {
        int tagId = -1;

        if (!d->langs.isEmpty())
        {
            for (const QString& trLang : std::as_const(d->langs))
            {
                QString trOut;
                QString error;
                bool trRet = s_inlineTranslateString(tag, trLang, trOut, error);

                if (trRet)
                {
                    tagId = tagsCache->getOrCreateTag(rootTag + trLang +
                                                      QLatin1Char('/') + trOut);
                }
                else
                {
                    qCDebug(DIGIKAM_AUTOTAGSENGINE_LOG) << "Auto-Tags online translation error:"
                                                        << error;
                    tagId = tagsCache->getOrCreateTag(rootTag + trLang +
                                                      QLatin1Char('/') +  tag);
                }
            }
        }
        else
        {
            tagId = tagsCache->getOrCreateTag(rootTag + tag);
        }

        if (tagId != -1)
        {
            info.setTag(tagId);
            tagsChanged = true;
        }
    }

    // Write tags to the metadata too

    if (tagsChanged)
    {
        MetadataHub hub;
        hub.load(info);

        ScanController::FileMetadataWrite writeScope(info);
        writeScope.changed(hub.writeToMetadata(info, MetadataHub::WRITE_TAGS));
    }
}

void AutotagsAssignmentTask::run()
{
    // While we have data (using this as check for non-null)

    while (d->data)
    {
        if (m_cancel)
        {
            return;
        }

        QList<DImg> inputImages;

        for (int i = 0 ; i < d->loadCount ; ++i)
        {
            // cppcheck-suppress knownConditionTrueFalse
            if (m_cancel)
            {
                return;
            }

            QString path = d->data->getImagePath();

            if (path.isEmpty())
            {
                break;
            }

            inputImages << PreviewLoadThread::loadFastSynchronously(path, 2000);
        }

        if (inputImages.isEmpty())
        {
            break;
        }

        qCDebug(DIGIKAM_AUTOTAGSENGINE_LOG) << "Current batch size:" << inputImages.size();

        // Run Autotags backend here
        // Assign Tags in database using API from itemInfo

        QElapsedTimer timer;
        timer.start();

        QScopedPointer<AutoTagsAssign> autotagsEngine(new AutoTagsAssign(DetectorModel(d->modelType)));
        QList<QList<QString> > tagsLists = autotagsEngine->generateTagsList(inputImages, d->batchSize);

        if (tagsLists.size() >= inputImages.size())
        {
            for (int j = 0 ; j < inputImages.size() ; ++j)
            {
                QImage qimg   = inputImages.at(j).smoothScale(48, 48, Qt::KeepAspectRatio).copyQImage();
                assignTags(inputImages.at(j).originalFilePath(), tagsLists.at(j));
                ItemInfo info = ItemInfo::fromLocalFile(inputImages.at(j).originalFilePath());

                Q_EMIT signalFinished(info, qimg);
            }
        }
        else
        {
            Q_EMIT signalFinished(ItemInfo(), QImage());
        }

        qCDebug(DIGIKAM_AUTOTAGSENGINE_LOG) << "Assgin Tags process takes:"
                                            << timer.elapsed() << "ms";
    }

    Q_EMIT signalDone();
}

} // namespace Digikam

#include "moc_autotagsassignmenttask.cpp"
