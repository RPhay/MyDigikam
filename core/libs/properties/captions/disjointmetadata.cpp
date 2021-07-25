/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-08-17
 * Description : Helper class for Image Description Editor Tab
 *
 * Copyright (C) 2015 by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "disjointmetadata_p.h"

namespace Digikam
{

DisjointMetadata::DisjointMetadata()
    : QObject(),
      d      (new Private())
{
    d->makeConnections(this);
}

DisjointMetadata::DisjointMetadata(const DisjointMetadata& other)
    : QObject(other.parent()),
      d      (new Private(*other.d))
{
    d->makeConnections(this);
}

DisjointMetadata::~DisjointMetadata()
{
    delete d;
}

DisjointMetadata& DisjointMetadata::operator=(const DisjointMetadata& other)
{
    if (this != &other)
    {
        delete d;
        d = new Private(*other.d);
    }

    return *this;
}

void DisjointMetadata::reset()
{
    delete d;
    d = new Private();
}

void DisjointMetadata::load(const ItemInfo& info)
{
    Template metadataTemplate;
    CaptionsMap commentMap;
    CaptionsMap titleMap;
    QDateTime dateTime;
    int colorLabel;
    int pickLabel;
    int rating;

    if (d->dateTimeStatus == DisjointMetadataDataFields::MetadataDisjoint)
    {
        dateTime = d->dateTime;
    }
    else
    {
        dateTime = info.dateTime();
    }

    if ((d->titlesStatus   == DisjointMetadataDataFields::MetadataDisjoint) &&
        (d->commentsStatus == DisjointMetadataDataFields::MetadataDisjoint))
    {
        commentMap = d->comments;
        titleMap   = d->titles;
    }
    else
    {
        CoreDbAccess access;
        ItemComments comments = info.imageComments(access);
        commentMap            = comments.toCaptionsMap();
        titleMap              = comments.toCaptionsMap(DatabaseComment::Title);
    }

    if (d->colorLabelStatus == DisjointMetadataDataFields::MetadataDisjoint)
    {
        colorLabel = d->colorLabel;
    }
    else
    {
        colorLabel = info.colorLabel();
    }

    if (d->pickLabelStatus == DisjointMetadataDataFields::MetadataDisjoint)
    {
        pickLabel = d->pickLabel;
    }
    else
    {
        pickLabel = info.pickLabel();
    }

    if (d->ratingStatus == DisjointMetadataDataFields::MetadataDisjoint)
    {
        rating = d->rating;
    }
    else
    {
        rating = info.rating();
    }

    if (d->templateStatus == DisjointMetadataDataFields::MetadataDisjoint)
    {
        metadataTemplate = d->metadataTemplate;
    }
    else
    {
        Template tref    = info.metadataTemplate();
        Template t       = TemplateManager::defaultManager()->findByContents(tref);
/*
        qCDebug(DIGIKAM_GENERAL_LOG) << "Found Metadata Template: " << t.templateTitle();
*/
        metadataTemplate = t.isNull() ? tref : t;
    }

    load(dateTime,
         titleMap,
         commentMap,
         colorLabel,
         pickLabel,
         rating,
         metadataTemplate);

    loadTags(info.tagIds());
}

//-----------------------------Status -------------------------

DisjointMetadataDataFields::Status DisjointMetadata::dateTimeStatus() const
{
    return d->dateTimeStatus;
}

DisjointMetadataDataFields::Status DisjointMetadata::titlesStatus() const
{
    return d->titlesStatus;
}

DisjointMetadataDataFields::Status DisjointMetadata::commentsStatus() const
{
    return d->commentsStatus;
}

DisjointMetadataDataFields::Status DisjointMetadata::pickLabelStatus() const
{
    return d->pickLabelStatus;
}

DisjointMetadataDataFields::Status DisjointMetadata::colorLabelStatus() const
{
    return d->colorLabelStatus;
}

DisjointMetadataDataFields::Status DisjointMetadata::ratingStatus() const
{
    return d->ratingStatus;
}

DisjointMetadataDataFields::Status DisjointMetadata::templateStatus() const
{
    return d->templateStatus;
}

DisjointMetadataDataFields::Status DisjointMetadata::tagStatus(int albumId) const
{
    return d->tags.value(albumId, DisjointMetadataDataFields::DisjointMetadataDataFields::MetadataInvalid);
}

DisjointMetadataDataFields::Status DisjointMetadata::tagStatus(const QString& tagPath) const
{
    return tagStatus(TagsCache::instance()->tagForPath(tagPath));
}

//---- Changed status ---------------------

bool DisjointMetadata::dateTimeChanged() const
{
    return d->dateTimeChanged;
}

bool DisjointMetadata::titlesChanged() const
{
    return d->titlesChanged;
}

bool DisjointMetadata::commentsChanged() const
{
    return d->commentsChanged;
}

bool DisjointMetadata::pickLabelChanged() const
{
    return d->pickLabelChanged;
}

bool DisjointMetadata::colorLabelChanged() const
{
    return d->colorLabelChanged;
}

bool DisjointMetadata::ratingChanged() const
{
    return d->ratingChanged;
}

bool DisjointMetadata::templateChanged() const
{
    return d->templateChanged;
}

bool DisjointMetadata::tagsChanged() const
{
    return d->tagsChanged;
}

void DisjointMetadata::setDateTime(const QDateTime& dateTime, DisjointMetadataDataFields::Status status)
{
    d->dateTimeStatus  = status;
    d->dateTime        = dateTime;
    d->dateTimeChanged = true;
}

void DisjointMetadata::setTitles(const CaptionsMap& titles, DisjointMetadataDataFields::Status status)
{
    d->titlesStatus  = status;
    d->titles        = titles;
    d->titlesChanged = true;
}

void DisjointMetadata::setComments(const CaptionsMap& comments, DisjointMetadataDataFields::Status status)
{
    d->commentsStatus  = status;
    d->comments        = comments;
    d->commentsChanged = true;
}

void DisjointMetadata::setPickLabel(int pickId, DisjointMetadataDataFields::Status status)
{
    d->pickLabelStatus  = status;
    d->pickLabel        = pickId;
    d->pickLabelChanged = true;
}

void DisjointMetadata::setColorLabel(int colorId, DisjointMetadataDataFields::Status status)
{
    d->colorLabelStatus  = status;
    d->colorLabel        = colorId;
    d->colorLabelChanged = true;
}

void DisjointMetadata::setRating(int rating, DisjointMetadataDataFields::Status status)
{
    d->ratingStatus   = status;
    d->rating         = rating;
    d->ratingChanged  = true;
}

void DisjointMetadata::setMetadataTemplate(const Template& t, DisjointMetadataDataFields::Status status)
{
    d->templateStatus   = status;
    d->metadataTemplate = t;
    d->templateChanged  = true;
}

void DisjointMetadata::setTag(int albumID, DisjointMetadataDataFields::Status status)
{
    d->tags[albumID] = status;
    d->tagsChanged   = true;
}

void DisjointMetadata::resetChanged()
{
    d->dateTimeChanged   = false;
    d->titlesChanged     = false;
    d->commentsChanged   = false;
    d->pickLabelChanged  = false;
    d->colorLabelChanged = false;
    d->ratingChanged     = false;
    d->templateChanged   = false;
    d->tagsChanged       = false;
}

bool DisjointMetadata::write(ItemInfo info, WriteMode writeMode)
{
    applyChangeNotifications();

    bool changed        = false;

    // find out in advance if we have something to write - needed for FullWriteIfChanged mode

    bool saveTitle      = (d->titlesStatus     == DisjointMetadataDataFields::MetadataAvailable);
    bool saveComment    = (d->commentsStatus   == DisjointMetadataDataFields::MetadataAvailable);
    bool saveDateTime   = (d->dateTimeStatus   == DisjointMetadataDataFields::MetadataAvailable);
    bool savePickLabel  = (d->pickLabelStatus  == DisjointMetadataDataFields::MetadataAvailable);
    bool saveColorLabel = (d->colorLabelStatus == DisjointMetadataDataFields::MetadataAvailable);
    bool saveRating     = (d->ratingStatus     == DisjointMetadataDataFields::MetadataAvailable);
    bool saveTemplate   = (d->templateStatus   == DisjointMetadataDataFields::MetadataAvailable);
    bool saveTags       = true;

    bool writeAllFields;

    if      (writeMode == FullWrite)
    {
        writeAllFields = true;
    }
    else if (writeMode == FullWriteIfChanged)
    {
        writeAllFields = (
                             (saveTitle      && d->titlesChanged)     ||
                             (saveComment    && d->commentsChanged)   ||
                             (saveDateTime   && d->dateTimeChanged)   ||
                             (savePickLabel  && d->pickLabelChanged)  ||
                             (saveColorLabel && d->colorLabelChanged) ||
                             (saveRating     && d->ratingChanged)     ||
                             (saveTemplate   && d->templateChanged)   ||
                             (saveTags       && d->tagsChanged)
                         );
    }
    else // PartialWrite
    {
        writeAllFields = false;
    }

    if (saveTitle && (writeAllFields || d->titlesChanged))
    {
        CoreDbAccess access;
        ItemComments comments = info.imageComments(access);
        comments.replaceComments(d->titles, DatabaseComment::Title);
        changed               = true;
    }

    if (saveComment && (writeAllFields || d->commentsChanged))
    {
        CoreDbAccess access;
        ItemComments comments = info.imageComments(access);
        comments.replaceComments(d->comments);
        changed               = true;
    }

    if (saveDateTime && (writeAllFields || d->dateTimeChanged))
    {
        info.setDateTime(d->dateTime);
        changed = true;
    }

    if (savePickLabel && (writeAllFields || d->pickLabelChanged))
    {
        info.setPickLabel(d->pickLabel);
        changed = true;
    }

    if (saveColorLabel && (writeAllFields || d->colorLabelChanged))
    {
        info.setColorLabel(d->colorLabel);
        changed = true;
    }

    if (saveRating && (writeAllFields || d->ratingChanged))
    {
        info.setRating(d->rating);
        changed = true;
    }

    if (saveTemplate && writeAllFields)
    {
        QString title = d->metadataTemplate.templateTitle();

        if (title == Template::removeTemplateTitle())
        {
            info.removeMetadataTemplate();
        }

        if (title.isEmpty())
        {
            // Nothing to do.
        }
        else
        {
            info.setMetadataTemplate(d->metadataTemplate);
        }

        changed = true;
    }

    if (writeAllFields || d->tagsChanged)
    {
        QList<int> keys = d->tags.keys();

        foreach (int key, keys)
        {
            if (d->tags.value(key) == DisjointMetadataDataFields::DisjointMetadataDataFields::MetadataAvailable)
            {
                info.setTag(key);
                changed = true;
            }

            if (d->tags.value(key) == DisjointMetadataDataFields::DisjointMetadataDataFields::MetadataInvalid)
            {
                info.removeTag(key);
                changed = true;
            }
        }
    }

    return changed;
}

bool DisjointMetadata::willWriteMetadata(DisjointMetadata::WriteMode writeMode,
                                         const MetaEngineSettingsContainer& settings) const
{
    // This is the same logic as in write(DMetadata) but without actually writing.
    // Adapt if the method above changes

    bool saveTitle      = (settings.saveComments   && (d->titlesStatus     == DisjointMetadataDataFields::MetadataAvailable));
    bool saveComment    = (settings.saveComments   && (d->commentsStatus   == DisjointMetadataDataFields::MetadataAvailable));
    bool saveDateTime   = (settings.saveDateTime   && (d->dateTimeStatus   == DisjointMetadataDataFields::MetadataAvailable));
    bool savePickLabel  = (settings.savePickLabel  && (d->pickLabelStatus  == DisjointMetadataDataFields::MetadataAvailable));
    bool saveColorLabel = (settings.saveColorLabel && (d->colorLabelStatus == DisjointMetadataDataFields::MetadataAvailable));
    bool saveRating     = (settings.saveRating     && (d->ratingStatus     == DisjointMetadataDataFields::MetadataAvailable));
    bool saveTemplate   = (settings.saveTemplate   && (d->templateStatus   == DisjointMetadataDataFields::MetadataAvailable));
    bool saveTags       = (settings.saveTags || settings.saveFaceTags);

    bool writeAllFields;

    if      (writeMode == FullWrite)
    {
        writeAllFields = true;
    }
    else if (writeMode == FullWriteIfChanged)
    {
        writeAllFields = (
                             (saveTitle      && d->titlesChanged)     ||
                             (saveComment    && d->commentsChanged)   ||
                             (saveDateTime   && d->dateTimeChanged)   ||
                             (savePickLabel  && d->pickLabelChanged)  ||
                             (saveColorLabel && d->colorLabelChanged) ||
                             (saveRating     && d->ratingChanged)     ||
                             (saveTemplate   && d->templateChanged)   ||
                             (saveTags       && d->tagsChanged)
                         );
    }
    else // PartialWrite
    {
        writeAllFields = false;
    }

    return (
               (saveTitle      && (writeAllFields || d->titlesChanged))     ||
               (saveComment    && (writeAllFields || d->commentsChanged))   ||
               (saveDateTime   && (writeAllFields || d->dateTimeChanged))   ||
               (savePickLabel  && (writeAllFields || d->pickLabelChanged))  ||
               (saveColorLabel && (writeAllFields || d->colorLabelChanged)) ||
               (saveRating     && (writeAllFields || d->ratingChanged))     ||
               (saveTags       && (writeAllFields || d->tagsChanged))       ||
               (saveTemplate   && (writeAllFields || d->templateChanged))
           );
}

int DisjointMetadata::changedFlags()
{
    int value = 0;

    if (d->titlesChanged)
    {
        value |= MetadataHub::WRITE_TITLE;
    }

    if (d->commentsChanged)
    {
        value |= MetadataHub::WRITE_COMMENTS;
    }

    if (d->dateTimeChanged)
    {
        value |= MetadataHub::WRITE_DATETIME;
    }

    if (d->pickLabelChanged)
    {
        value |= MetadataHub::WRITE_PICKLABEL;
    }

    if (d->colorLabelChanged)
    {
        value |= MetadataHub::WRITE_COLORLABEL;
    }

    if (d->ratingChanged)
    {
        value |= MetadataHub::WRITE_RATING;
    }

    if (d->tagsChanged)
    {
        value |= MetadataHub::WRITE_TAGS;
    }

    if (d->templateChanged)
    {
        value |= MetadataHub::WRITE_TEMPLATE;
    }

    return value;
}

void DisjointMetadata::load(const QDateTime& dateTime,const CaptionsMap& titles,
                            const CaptionsMap& comment, int colorLabel,
                            int pickLabel, int rating, const Template& t)
{
    if (dateTime.isValid())
    {
        d->loadSingleValue<QDateTime>(dateTime, d->dateTime, d->dateTimeStatus);
    }

    d->loadSingleValue<int>(pickLabel, d->pickLabel, d->pickLabelStatus);

    d->loadSingleValue<int>(colorLabel, d->colorLabel, d->colorLabelStatus);

    d->loadSingleValue<int>(rating, d->rating, d->ratingStatus);

    d->loadSingleValue<CaptionsMap>(titles, d->titles, d->titlesStatus);

    d->loadSingleValue<CaptionsMap>(comment, d->comments, d->commentsStatus);

    d->loadSingleValue<Template>(t, d->metadataTemplate, d->templateStatus);
}

void DisjointMetadata::loadTags(const QList<int>& tagIds)
{
    QList<int> loadedTagIds;

    foreach (int tagId, tagIds)
    {
        if (!TagsCache::instance()->isInternalTag(tagId))
        {
            loadedTagIds << tagId;
        }
    }

    if (loadedTagIds.isEmpty())
    {
        if (!d->withoutTags)
        {
            QMap<int, DisjointMetadataDataFields::Status>::iterator it;

            for (it = d->tags.begin() ; it != d->tags.end() ; ++it)
            {
                it.value() = DisjointMetadataDataFields::MetadataDisjoint;
            }

            d->withoutTags = true;
        }

        return;
    }

    // If tags map is empty, set them all as Available

    if (d->tags.isEmpty())
    {
        foreach (int tagId, loadedTagIds)
        {
            if (d->withoutTags)
            {
                d->tags[tagId] = DisjointMetadataDataFields::MetadataDisjoint;
            }
            else
            {
                d->tags[tagId] = DisjointMetadataDataFields::MetadataAvailable;
            }
        }

        return;
    }

    std::sort(loadedTagIds.begin(), loadedTagIds.end());

    // We search for metadata available tags, and
    // it is not present in current list, set it to
    // disjoint

    QMap<int, DisjointMetadataDataFields::Status>::iterator it;

    for (it = d->tags.begin() ; it != d->tags.end() ; ++it)
    {
        if (it.value() == DisjointMetadataDataFields::MetadataAvailable)
        {
            if (!std::binary_search(loadedTagIds.begin(), loadedTagIds.end(), it.key()))
            {
                it.value() = DisjointMetadataDataFields::MetadataDisjoint;
            }
        }
    }

    // new tags which are not yet in the set,
    // are added as Disjoint

    foreach (int tagId, loadedTagIds)
    {
        if (!d->tags.contains(tagId))
        {
            d->tags[tagId] = DisjointMetadataDataFields::MetadataDisjoint;
        }
    }
}

void DisjointMetadata::notifyTagDeleted(int id)
{
    d->tags.remove(id);
}

void DisjointMetadata::slotTagDeleted(int tagId)
{
    QMutexLocker locker(&d->mutex);
    d->tagIds << tagId;
}

void DisjointMetadata::slotInvalidate()
{
    QMutexLocker locker(&d->mutex);
    d->invalid = true;
}

void DisjointMetadata::applyChangeNotifications()
{
    if (d->invalid)
    {
        reset();
    }

    QList<int> tagIds;
    {
        QMutexLocker locker(&d->mutex);
        tagIds = d->tagIds;
        d->tagIds.clear();
    }

    foreach (int tagId, tagIds)
    {
        notifyTagDeleted(tagId);
    }
}

QDateTime DisjointMetadata::dateTime() const
{
    return d->dateTime;
}

CaptionsMap DisjointMetadata::titles() const
{
    return d->titles;
}

CaptionsMap DisjointMetadata::comments() const
{
    return d->comments;
}

int DisjointMetadata::pickLabel() const
{
    return d->pickLabel;
}

int DisjointMetadata::colorLabel() const
{
    return d->colorLabel;
}

int DisjointMetadata::rating() const
{
    return d->rating;
}

Template DisjointMetadata::metadataTemplate() const
{
    return d->metadataTemplate;
}

void DisjointMetadata::dateTimeInterval(QDateTime& lowest, QDateTime& highest) const
{
    switch (d->dateTimeStatus)
    {
        case DisjointMetadataDataFields::MetadataInvalid:
        {
            lowest  = QDateTime();
            highest = QDateTime();
            break;
        }

        case DisjointMetadataDataFields::MetadataAvailable:
        {
            lowest  = d->dateTime;
            highest = d->dateTime;
            break;
        }

        case DisjointMetadataDataFields::MetadataDisjoint:
        {
            lowest  = d->dateTime;
            highest = d->lastDateTime;
            break;
        }
    }
}

void DisjointMetadata::pickLabelInterval(int& lowest, int& highest) const
{
    switch (d->pickLabelStatus)
    {
        case DisjointMetadataDataFields::MetadataInvalid:
        {
            lowest  = -1;
            highest = -1;
            break;
        }

        case DisjointMetadataDataFields::MetadataAvailable:
        {
            lowest  = d->pickLabel;
            highest = d->pickLabel;
            break;
        }

        case DisjointMetadataDataFields::MetadataDisjoint:
        {
            lowest  = d->pickLabel;
            highest = d->highestPickLabel;
            break;
        }
    }
}

void DisjointMetadata::colorLabelInterval(int& lowest, int& highest) const
{
    switch (d->colorLabelStatus)
    {
        case DisjointMetadataDataFields::MetadataInvalid:
        {
            lowest  = -1;
            highest = -1;
            break;
        }

        case DisjointMetadataDataFields::MetadataAvailable:
        {
            lowest  = d->colorLabel;
            highest = d->colorLabel;
            break;
        }

        case DisjointMetadataDataFields::MetadataDisjoint:
        {
            lowest  = d->colorLabel;
            highest = d->highestColorLabel;
            break;
        }
    }
}

void DisjointMetadata::ratingInterval(int& lowest, int& highest) const
{
    switch (d->ratingStatus)
    {
        case DisjointMetadataDataFields::MetadataInvalid:
        {
            lowest  = -1;
            highest = -1;
            break;
        }

        case DisjointMetadataDataFields::MetadataAvailable:
        {
            lowest  = d->rating;
            highest = d->rating;
            break;
        }

        case DisjointMetadataDataFields::MetadataDisjoint:
        {
            lowest  = d->rating;
            highest = d->highestRating;
            break;
        }
    }
}

QStringList DisjointMetadata::keywords() const
{
    QStringList tagList;

    QList<int> keys = d->tags.keys();

    foreach (int key, keys)
    {
        if (d->tags.value(key) == DisjointMetadataDataFields::MetadataAvailable)
        {
            tagList.append(TagsCache::instance()->tagPath(key, TagsCache::NoLeadingSlash));
        }
    }

    return tagList;
}

QMap<int, DisjointMetadataDataFields::Status> DisjointMetadata::tags() const
{
    // DatabaseMode == ManagedTags is assumed

    return d->tags;
}

DisjointMetadataDataFields DisjointMetadata::dataFields() const
{
    DisjointMetadataDataFields data(*d);

    return data;
}

void DisjointMetadata::setDataFields(const DisjointMetadataDataFields& data)
{
    if (d)
    {
        delete d;
    }

    d = new Private(data);
    d->makeConnections(this);
}

} // namespace Digikam
