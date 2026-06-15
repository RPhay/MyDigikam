/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-05-04
 * Description : Various operation on items
 *
 * SPDX-FileCopyrightText: 2002-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2002-2026 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2009-2010 by Andi Clemens <andi dot clemens at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "itemviewutilities.h"

// C++ includes

#include <algorithm>
#include <limits>

// Qt includes

#include <QDate>
#include <QList>
#include <QStandardPaths>
#include <QTime>
#include <QFileInfo>
#include <QUrl>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "digikam_debug.h"
#include "album.h"
#include "albummanager.h"
#include "albumselectdialog.h"
#include "applicationsettings.h"
#include "deletedialog.h"
#include "dfiledialog.h"
#include "dio.h"
#include "imagewindow.h"
#include "lighttablewindow.h"
#include "surveywindow.h"
#include "loadingcacheinterface.h"
#include "queuemgrwindow.h"
#include "thumbnailloadthread.h"
#include "fileactionmngr.h"
#include "dfileoperations.h"
#include "coredb.h"
#include "coredbaccess.h"

namespace Digikam
{

ItemViewUtilities::ItemViewUtilities(QWidget* const parentWidget)
    : QObject (parentWidget),
      m_widget(parentWidget)
{
    connect(this, SIGNAL(signalImagesDeleted(QList<qlonglong>)),
            AlbumManager::instance(), SLOT(slotImagesDeleted(QList<qlonglong>)));
}

void ItemViewUtilities::setAsAlbumThumbnail(Album* album,
                                            const ItemInfo& itemInfo)
{
    if (!album)
    {
        return;
    }

    if      (album->type() == Album::PHYSICAL)
    {
        PAlbum* const palbum = static_cast<PAlbum*>(album);

        QString err;
        AlbumManager::instance()->updatePAlbumIcon(palbum, itemInfo.id(), err);
    }
    else if (album->type() == Album::TAG)
    {
        TAlbum* const talbum = static_cast<TAlbum*>(album);

        QString err;
        AlbumManager::instance()->updateTAlbumIcon(talbum, QString(), itemInfo.id(), err);
    }
}

void ItemViewUtilities::rename(const QUrl& imageUrl,
                               const QString& newName,
                               bool overwrite)
{
    if (imageUrl.isEmpty() || !imageUrl.isLocalFile() || newName.isEmpty())
    {
        return;
    }

    DIO::rename(imageUrl, newName, overwrite);
}

bool ItemViewUtilities::deleteImages(const QList<ItemInfo>& infos,
                                     const DeleteMode deleteMode)
{
    if (infos.isEmpty())
    {
        return false;
    }

    QList<ItemInfo> deleteInfos = infos;

    QList<QUrl> urlList;
    QList<qlonglong> imageIds;

    // Buffer the urls for deletion and imageids for notification of the AlbumManager

    for (const ItemInfo& info : std::as_const(deleteInfos))
    {
        urlList  << info.fileUrl();
        imageIds << info.id();
    }

    DeleteDialog dialog(m_widget);

    DeleteDialogMode::DeleteMode deleteDialogMode = DeleteDialogMode::NoChoiceTrash;

    if (deleteMode == ItemViewUtilities::DeletePermanently)
    {
        deleteDialogMode = DeleteDialogMode::NoChoiceDeletePermanently;
    }

    if (!dialog.confirmDeleteList(urlList, DeleteDialogMode::Files, deleteDialogMode))
    {
        return false;
    }

    const bool useTrash = !dialog.shouldDelete();

    DIO::del(deleteInfos, useTrash);

    // Signal the Albummanager about the ids of the deleted images.

    Q_EMIT signalImagesDeleted(imageIds);

    return true;
}

void ItemViewUtilities::deleteImagesDirectly(const QList<ItemInfo>& infos,
                                             const DeleteMode deleteMode)
{
    // This method deletes the selected items directly, without confirmation.
    // It is not used in the default setup.

    if (infos.isEmpty())
    {
        return;
    }

    QList<qlonglong> imageIds;

    for (const ItemInfo& info : std::as_const(infos))
    {
        imageIds << info.id();
    }

    const bool useTrash = (deleteMode == ItemViewUtilities::DeleteUseTrash);

    DIO::del(infos, useTrash);

    // Signal the Albummanager about the ids of the deleted images.

    Q_EMIT signalImagesDeleted(imageIds);
}

void ItemViewUtilities::notifyFileContentChanged(const QList<QUrl>& urls)
{
    for (const QUrl& url : std::as_const(urls))
    {
        QString path = url.toLocalFile();
        ThumbnailLoadThread::deleteThumbnail(path);

        // clean LoadingCache as well - be pragmatic, do it here.

        LoadingCacheInterface::fileChanged(path);
    }
}

void ItemViewUtilities::copyItemsToExternalFolder(const QList<ItemInfo>& infos)
{
    if (infos.isEmpty())
    {
        return;
    }

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("Copy To Folder Settings"));
    QString startingPath      = group.readEntry(QLatin1String("Last Copy To Folder Path"), QString());

    if (startingPath.isEmpty())
    {
        startingPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    }

    QUrl url = DFileDialog::getExistingDirectoryUrl(m_widget, i18nc("@title:window", "Select Target Folder"),
                                                    QUrl::fromLocalFile(startingPath));

    if (url.isEmpty() || !url.isLocalFile())
    {
        return;
    }

    group.writeEntry(QLatin1String("Last Copy To Folder Path"), url.toLocalFile());

    DIO::copy(infos, url);
}

void ItemViewUtilities::createNewAlbumForInfos(const QList<ItemInfo>& infos,
                                               Album* currentAlbum)
{
    if (infos.isEmpty())
    {
        return;
    }

    if (currentAlbum && (currentAlbum->type() != Album::PHYSICAL))
    {
        currentAlbum = nullptr;
    }

    QString header(i18n("<p>Please select the destination album from the digiKam library to "
                        "move the selected images into.</p>"));

    Album* select = AlbumManager::instance()->findAlbum(m_lastAlbumId);

    if (select && (select->type() != Album::PHYSICAL))
    {
        select = currentAlbum;
    }

    Album* const album = AlbumSelectDialog::selectAlbum(m_widget, dynamic_cast<PAlbum*>(select), header);

    if (!album)
    {
        return;
    }

    PAlbum* const palbum = dynamic_cast<PAlbum*>(album);

    if (!palbum)
    {
        return;
    }

    m_lastAlbumId = palbum->globalID();

    DIO::move(infos, palbum);
}

void ItemViewUtilities::insertToLightTableAuto(const QList<ItemInfo>& all,
                                               const QList<ItemInfo>& selected,
                                               const ItemInfo& current)
{
    ItemInfoList list   = ItemInfoList(selected);
    ItemInfo singleInfo = current;

    if (list.isEmpty() || ((list.size() == 1) && LightTableWindow::lightTableWindow()->isEmpty()))
    {
        list = ItemInfoList(all);
    }

    if (singleInfo.isNull() && !list.isEmpty())
    {
        singleInfo = list.first();
    }

    insertToLightTable(list, current, (list.size() <= 1));
}

void ItemViewUtilities::insertToLightTable(const QList<ItemInfo>& list,
                                           const ItemInfo& current,
                                           bool addTo)
{
    LightTableWindow* const ltview = LightTableWindow::lightTableWindow();

    // If addTo is false, the light table will be emptied before adding
    // the images.

    ltview->loadItemInfos(ItemInfoList(list), current, addTo);
    ltview->setLeftRightItems(ItemInfoList(list), addTo);

    if (ltview->isHidden())
    {
        ltview->show();
    }

    ltview->unminimizeAndActivateWindow();
}

void ItemViewUtilities::insertToQueueManager(const QList<ItemInfo>& list, const ItemInfo& current, bool newQueue)
{
    Q_UNUSED(current);

    QueueMgrWindow* const bqmview = QueueMgrWindow::queueManagerWindow();

    if (bqmview->isHidden())
    {
        bqmview->show();
    }

    bqmview->unminimizeAndActivateWindow();

    if (newQueue)
    {
        bqmview->loadItemInfosToNewQueue(ItemInfoList(list));
    }
    else
    {
        bqmview->loadItemInfosToCurrentQueue(ItemInfoList(list));
    }
}

void ItemViewUtilities::insertSilentToQueueManager(const QList<ItemInfo>& list,
                                                   const ItemInfo& /*current*/,
                                                   int queueid)
{
    QueueMgrWindow* const bqmview = QueueMgrWindow::queueManagerWindow();
    bqmview->loadItemInfos(ItemInfoList(list), queueid);
}

void ItemViewUtilities::openInfos(const ItemInfo& info,
                                  const QList<ItemInfo>& allInfosToOpen,
                                  Album* currentAlbum)
{
    if (info.isNull())
    {
        return;
    }

    QFileInfo fi(info.filePath());
    QString imagefilter = ApplicationSettings::instance()->getImageFileFilter();
    imagefilter        += ApplicationSettings::instance()->getRawFileFilter();

    // If the current item is not an image file.

    if (!imagefilter.contains(fi.suffix().toLower()))
    {
        // Openonly the first one from the list.

        openInfosWithDefaultApplication(QList<ItemInfo>() << info);

        return;
    }

    // Run digiKam ImageEditor with all image from current Album.

    ImageWindow* const imview = ImageWindow::imageWindow();

    imview->disconnect(this);

    connect(imview, SIGNAL(signalURLChanged(QUrl)),
            this, SIGNAL(editorCurrentUrlChanged(QUrl)));

    imview->loadItemInfos(ItemInfoList(allInfosToOpen), info,
                          currentAlbum ? i18n("Album \"%1\"", currentAlbum->title())
                                       : QString());

    if (imview->isHidden())
    {
        imview->show();
    }

    imview->unminimizeAndActivateWindow();
}

void ItemViewUtilities::openInfosWithDefaultApplication(const QList<ItemInfo>& infos)
{
    if (infos.isEmpty())
    {
        return;
    }

    QList<QUrl> urls;

    for (const ItemInfo& inf : std::as_const(infos))
    {
        urls << inf.fileUrl();
    }

    DFileOperations::openFilesWithDefaultApplication(urls);
}

// ---

namespace
{

bool lessThanByTimeForItemInfo(const ItemInfo& a, const ItemInfo& b)
{
    return (a.dateTime() < b.dateTime());
}

bool lowerThanByNameForItemInfo(const ItemInfo& a, const ItemInfo& b)
{
    return (a.name() < b.name());
}

bool lowerThanBySizeForItemInfo(const ItemInfo& a, const ItemInfo& b)
{
    return (a.fileSize() < b.fileSize());
}

} // namespace

// ---

void ItemViewUtilities::createGroupByTimeFromInfoList(const ItemInfoList& itemInfoList)
{
    QList<ItemInfo> groupingList = itemInfoList;

    // sort by time

    std::stable_sort(groupingList.begin(), groupingList.end(), lessThanByTimeForItemInfo);

    QList<ItemInfo>::iterator it, it2;

    for (it = groupingList.begin() ; it != groupingList.end() ; )
    {
        const ItemInfo& leader = *it;
        QList<ItemInfo> group;
        QDateTime time         = it->dateTime();

        if (time.isValid())
        {
            for (it2 = it + 1 ; it2 != groupingList.end() ; ++it2)
            {
                if (qAbs(time.secsTo(it2->dateTime())) < 2)
                {
                    group << *it2;
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            ++it;
            continue;
        }

        // increment to next item not put in the group

        it = it2;

        if (!group.isEmpty())
        {
            FileActionMngr::instance()->addToGroup(leader, group);
        }
    }
}

void ItemViewUtilities::createGroupByFilenameFromInfoList(const ItemInfoList& itemInfoList)
{
    QList<ItemInfo> groupingList = itemInfoList;

    // sort by Name

    std::stable_sort(groupingList.begin(), groupingList.end(), lowerThanByNameForItemInfo);

    QList<ItemInfo>::iterator it, it2;

    for (it = groupingList.begin() ; it != groupingList.end() ; )
    {
        QList<ItemInfo> group;
        QString fname = it->name().left(it->name().indexOf(QLatin1Char('.')));

        // don't know the leader yet so put first element also in group

        group << *it;

        for (it2 = it + 1 ; it2 != groupingList.end() ; ++it2)
        {
            QString fname2 = it2->name().left(it2->name().indexOf(QLatin1Char('.')));

            if (fname == fname2)
            {
                group << *it2;
            }
            else
            {
                break;
            }
        }

        // increment to next item not put in the group

        it = it2;

        if (group.size() > 1)
        {
            // sort by filesize and take smallest as leader

            std::stable_sort(group.begin(), group.end(), lowerThanBySizeForItemInfo);
            int rawCount = 0;

            for (int i = 0 ; i < group.size() ; ++i)
            {
                if (group.at(i).format().startsWith(QLatin1String("RAW")))
                {
                    ++rawCount;

                    if (i < group.size() - 1)
                    {
                        continue;
                    }
                }

                const ItemInfo& leader = group.takeAt((rawCount == group.size()) ? 0 : i);
                FileActionMngr::instance()->addToGroup(leader, group);
                break;
            }
        }
    }
}

// ---

namespace
{

class PrefixDateToken
{
public:

    QDate   date;
    QString textBeforeDate;
    QString textAfterDate;
    bool    isValid = false;
};

PrefixDateToken prefixDateToken(const QString& prefix)
{
    qsizetype tokenEnd = prefix.size();

    while ((tokenEnd > 0) && !prefix.at(tokenEnd - 1).isDigit())
    {
        if (prefix.at(tokenEnd - 1).isLetterOrNumber())
        {
            return {};
        }

        --tokenEnd;
    }

    if (tokenEnd < 8)
    {
        return {};
    }

    const qsizetype tokenStart = tokenEnd - 8;

    if ((tokenStart > 0) && prefix.at(tokenStart - 1).isDigit())
    {
        return {};
    }

    for (qsizetype index = tokenStart ; index < tokenEnd ; ++index)
    {
        if (!prefix.at(index).isDigit())
        {
            return {};
        }
    }

    const int   year  = prefix.mid(tokenStart,     4).toInt();
    const int   month = prefix.mid(tokenStart + 4, 2).toInt();
    const int   day   = prefix.mid(tokenStart + 6, 2).toInt();
    const QDate date(year, month, day);

    if (!date.isValid())
    {
        return {};
    }

    return { date, prefix.left(tokenStart), prefix.mid(tokenEnd), true };
}

QTime timeFromFilenameValue(qulonglong value, qsizetype valueLength)
{
    if (valueLength == 6)
    {
        const int hour   = static_cast<int>(value / 10000);
        const int minute = static_cast<int>((value / 100) % 100);
        const int second = static_cast<int>(value % 100);

        return QTime(hour, minute, second);
    }

    if (valueLength == 9)
    {
        const int hour        = static_cast<int>(value / 10000000);
        const int minute      = static_cast<int>((value / 100000) % 100);
        const int second      = static_cast<int>((value / 1000) % 100);
        const int millisecond = static_cast<int>(value % 1000);

        return QTime(hour, minute, second, millisecond);
    }

    return QTime();
}

class Q_DECL_HIDDEN NumberInFilenameMatch
{
public:

    NumberInFilenameMatch() = default;

    explicit NumberInFilenameMatch(const QString& filename)
        : NumberInFilenameMatch()
    {
        if (filename.isEmpty())
        {
            return;
        }

        const qsizetype suffixStart = filename.lastIndexOf(QLatin1Char('.'));
        const qsizetype searchEnd   = (suffixStart > 0) ? suffixStart : filename.size();

        qsizetype lastDigit = searchEnd - 1;

        while ((lastDigit >= 0) && !filename.at(lastDigit).isDigit())
        {
            --lastDigit;
        }

        if (lastDigit < 0)
        {
            prefix = filename;

            return;
        }

        qsizetype firstDigit = lastDigit;

        while ((firstDigit > 0) && filename.at(firstDigit - 1).isDigit())
        {
            --firstDigit;
        }

        prefix = filename.left(firstDigit);
        valueLength = lastDigit - firstDigit + 1;
        value  = filename.mid(firstDigit,
                              valueLength).toULongLong(&containsValue);

        suffix = filename.mid(lastDigit + 1);
    }

    bool directlyPreceeds(NumberInFilenameMatch const& other) const;

public:

    qulonglong value            = 0;
    qsizetype  valueLength      = 0;
    QString    prefix;
    QString    suffix;
    bool       containsValue    = false;
};

bool timestampPreceeds(const NumberInFilenameMatch& first, const NumberInFilenameMatch& second)
{
    if (first.valueLength != second.valueLength)
    {
        return false;
    }

    const PrefixDateToken firstDateToken  = prefixDateToken(first.prefix);
    const PrefixDateToken secondDateToken = prefixDateToken(second.prefix);

    if (!firstDateToken.isValid || !secondDateToken.isValid)
    {
        return false;
    }

    if (firstDateToken.textBeforeDate != secondDateToken.textBeforeDate)
    {
        return false;
    }

    if (firstDateToken.textAfterDate != secondDateToken.textAfterDate)
    {
        return false;
    }

    const QTime firstTime  = timeFromFilenameValue(first.value, first.valueLength);
    const QTime secondTime = timeFromFilenameValue(second.value, second.valueLength);

    if (!firstTime.isValid() || !secondTime.isValid())
    {
        return false;
    }

    if (firstDateToken.date != secondDateToken.date)
    {
        return (firstDateToken.date < secondDateToken.date);
    }

    return (firstTime < secondTime);
}

QString suffixWithoutCoverMarker(const QString& suffix)
{
    if (suffix.startsWith(QLatin1String("_COVER")))
    {
        return suffix.mid(6);
    }

    return suffix;
}

bool isLargestFixedWidthCounterValue(const NumberInFilenameMatch& match)
{
    if (match.valueLength <= 0)
    {
        return false;
    }

    qulonglong largestValue = 0;

    for (qsizetype index = 0 ; index < match.valueLength ; ++index)
    {
        if (largestValue > ((std::numeric_limits<qulonglong>::max() - 9) / 10))
        {
            return false;
        }

        largestValue = (largestValue * 10) + 9;
    }

    return (match.value == largestValue);
}

bool counterPreceeds(const NumberInFilenameMatch& first, const NumberInFilenameMatch& second)
{
    if (first.prefix != second.prefix)
    {
        return false;
    }

    if ((first.value < std::numeric_limits<qulonglong>::max()) &&
        ((first.value + 1) == second.value))
    {
        return true;
    }

    if ((first.valueLength != second.valueLength) || (second.value != 0))
    {
        return false;
    }

    return isLargestFixedWidthCounterValue(first);
}

bool burstCoverPreceeds(const NumberInFilenameMatch& first, const NumberInFilenameMatch& second)
{
    if (!counterPreceeds(first, second))
    {
        return false;
    }

    if (!first.prefix.contains(QLatin1String("BURST")))
    {
        return false;
    }

    if (first.suffix == second.suffix)
    {
        return false;
    }

    return (suffixWithoutCoverMarker(first.suffix) == second.suffix);
}

bool NumberInFilenameMatch::directlyPreceeds(NumberInFilenameMatch const& other) const
{
    if (!containsValue || !other.containsValue)
    {
        return false;
    }

    if (suffix != other.suffix)
    {
        return burstCoverPreceeds(*this, other);
    }

    if (counterPreceeds(*this, other))
    {
        return true;
    }

    return timestampPreceeds(*this, other);
}

bool imageMatchesTimelapseGroup(const ItemInfoList& group, const ItemInfo& itemInfo)
{
    if (group.size() < 2)
    {
        return true;
    }

    auto const timeBetweenPhotos      = qAbs(group.first().dateTime()
                                                          .secsTo(group.last()
                                                          .dateTime())) / (group.size()-1);

    auto const predictedNextTimestamp = group.last().dateTime()
                                                    .addSecs(timeBetweenPhotos);

    return (qAbs(itemInfo.dateTime().secsTo(predictedNextTimestamp)) <= 1);
}

bool matchesSameFilenameSequence(const NumberInFilenameMatch& first, const NumberInFilenameMatch& second)
{
    if (!first.containsValue || !second.containsValue)
    {
        return false;
    }

    if (first.prefix != second.prefix)
    {
        return false;
    }

    if (first.suffix == second.suffix)
    {
        return true;
    }

    if (!first.prefix.contains(QLatin1String("BURST")))
    {
        return false;
    }

    return (suffixWithoutCoverMarker(first.suffix) == suffixWithoutCoverMarker(second.suffix));
}

qsizetype sequenceStartIndex(const QList<NumberInFilenameMatch>& nameSortedMatches,
                             qsizetype startIndex,
                             qsizetype endIndex)
{
    if ((endIndex - startIndex) < 2)
    {
        return startIndex;
    }

    if (!nameSortedMatches.at(endIndex - 1).directlyPreceeds(nameSortedMatches.at(startIndex)))
    {
        return startIndex;
    }

    qsizetype result = startIndex;

    for (qsizetype index = startIndex + 1 ; index < endIndex ; ++index)
    {
        if (!nameSortedMatches.at(index - 1).directlyPreceeds(nameSortedMatches.at(index)))
        {
            result = index;
        }
    }

    return result;
}

QList<qsizetype> timelapseFilenameSequenceOrder(const QList<NumberInFilenameMatch>& nameSortedMatches)
{
    QList<qsizetype> orderedIndexes;
    orderedIndexes.reserve(nameSortedMatches.size());

    for (qsizetype runStart = 0 ; runStart < nameSortedMatches.size() ; )
    {
        qsizetype runEnd = runStart + 1;

        while ((runEnd < nameSortedMatches.size()) &&
               matchesSameFilenameSequence(nameSortedMatches.at(runStart), nameSortedMatches.at(runEnd)))
        {
            ++runEnd;
        }

        const qsizetype runSequenceStart = sequenceStartIndex(nameSortedMatches, runStart, runEnd);

        for (qsizetype index = runSequenceStart ; index < runEnd ; ++index)
        {
            orderedIndexes.append(index);
        }

        for (qsizetype index = runStart ; index < runSequenceStart ; ++index)
        {
            orderedIndexes.append(index);
        }

        runStart = runEnd;
    }

    return orderedIndexes;
}

void sortTimelapseGroupingListBySequence(ItemInfoList& groupingList)
{
    QList<NumberInFilenameMatch> nameSortedMatches;
    nameSortedMatches.reserve(groupingList.size());

    for (const ItemInfo& itemInfo : std::as_const(groupingList))
    {
        nameSortedMatches.append(NumberInFilenameMatch(itemInfo.name()));
    }

    const QList<qsizetype> sequenceOrder = timelapseFilenameSequenceOrder(nameSortedMatches);
    ItemInfoList sortedList;
    sortedList.reserve(groupingList.size());

    for (const qsizetype index : sequenceOrder)
    {
        sortedList.append(groupingList.at(index));
    }

    groupingList = sortedList;
}

} // namespace

// ---

void ItemViewUtilities::createGroupByTimelapseFromInfoList(const ItemInfoList& itemInfoList)
{
    if (itemInfoList.size() < 3)
    {
        return;
    }

    ItemInfoList groupingList = itemInfoList;

    std::stable_sort(groupingList.begin(), groupingList.end(), lowerThanByNameForItemInfo);
    sortTimelapseGroupingListBySequence(groupingList);

    NumberInFilenameMatch previousNumberMatch;
    ItemInfoList group;

    for (const auto& itemInfo : groupingList)
    {
        NumberInFilenameMatch numberMatch(itemInfo.name());

        // if this is an end of currently processed group

        if (!previousNumberMatch.directlyPreceeds(numberMatch) || !imageMatchesTimelapseGroup(group, itemInfo))
        {
            if (group.size() > 2)
            {
                FileActionMngr::instance()->addToGroup(group.takeFirst(), group);
            }

            group.clear();
        }

        group.append(itemInfo);
        previousNumberMatch = std::move(numberMatch);
    }

    if (group.size() > 2)
    {
        FileActionMngr::instance()->addToGroup(group.takeFirst(), group);
    }
}

} // namespace Digikam

#include "moc_itemviewutilities.cpp"
