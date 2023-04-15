/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-06-05
 * Description : DB Jobs Info
 *
 * SPDX-FileCopyrightText: 2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DB_JOB_INFO_H
#define DIGIKAM_DB_JOB_INFO_H

// Qt includes

#include <QString>
#include <QSet>

// Local includes

#include "coredburl.h"
#include "digikam_export.h"
#include "haariface.h"

namespace Digikam
{

class DIGIKAM_DATABASE_EXPORT DBJobInfo
{
public:

    void setFoldersJob();
    bool isFoldersJob()                 const;

    void setListAvailableImagesOnly();
    bool isListAvailableImagesOnly()    const;

    void setRecursive();
    bool isRecursive()                  const;

protected:

    explicit DBJobInfo();

private:

    bool m_folders;
    bool m_listAvailableImagesOnly;
    bool m_recursive;
};

// ---------------------------------------------

class DIGIKAM_DATABASE_EXPORT AlbumsDBJobInfo : public DBJobInfo
{
public:

    explicit AlbumsDBJobInfo();

    void setAlbumRootId(int id);
    int  albumRootId();

    void setAlbum(const QString& album);
    QString album();

private:

    int     m_albumRootId;
    QString m_album;
};

// ---------------------------------------------

class DIGIKAM_DATABASE_EXPORT TagsDBJobInfo : public DBJobInfo
{
public:

    explicit TagsDBJobInfo();

    void setFaceFoldersJob();
    bool isFaceFoldersJob()         const;

    void setSpecialTag(const QString& tag);
    QString specialTag()            const;

    void setTagsIds(const QList<int>& tagsIds);
    QList<int> tagsIds()            const;

private:

    bool       m_faceFolders;
    QString    m_specialTag;
    QList<int> m_tagsIds;
};

// ---------------------------------------------

class DIGIKAM_DATABASE_EXPORT GPSDBJobInfo : public DBJobInfo
{
public:

    explicit GPSDBJobInfo();

    void setDirectQuery();
    bool isDirectQuery()            const;

    void setLat1(qreal lat);
    qreal lat1()                    const;

    void setLng1(qreal lng);
    qreal lng1()                    const;

    void setLat2(qreal lat);
    qreal lat2()                    const;

    void setLng2(qreal lng);
    qreal lng2()                    const;

private:

    bool  m_directQuery;
    qreal m_lat1;
    qreal m_lng1;
    qreal m_lat2;
    qreal m_lng2;
};

// ---------------------------------------------

class DIGIKAM_DATABASE_EXPORT SearchesDBJobInfo : public DBJobInfo
{
public:

    explicit SearchesDBJobInfo(QList<int>&& searchIds);
    SearchesDBJobInfo(QSet<qlonglong>&& imageIds,
                      bool isAlbumUpdate,
                      HaarIface::RefImageSelMethod referenceSelectionMethod,
                      QSet<qlonglong>&& refImageIds);

    bool isDuplicatesJob()                                 const;
    bool isAlbumUpdate()                                   const;

    const QList<int>& searchIds()                          const;
    const QSet<qlonglong>& imageIds()                      const;

    HaarIface::RefImageSelMethod refImageSelectionMethod() const;
    const QSet<qlonglong>& refImageIds()                   const;

    void setMinThreshold(double t);
    double minThreshold()                                  const;

    void setMaxThreshold(double t);
    double maxThreshold()                                  const;

    void setSearchResultRestriction(int type);
    int searchResultRestriction()                          const;

public:

    bool                         m_duplicates;
    bool                         m_albumUpdate;
    int                          m_searchResultRestriction;
    QList<int>                   m_searchIds;
    QSet<qlonglong>              m_imageIds;
    double                       m_minThreshold;
    double                       m_maxThreshold;
    QSet<qlonglong>              m_refImageIds; // Image ids of the reference images if duplicates are available
    HaarIface::RefImageSelMethod m_refImageSelectionMethod;
};

// ---------------------------------------------

class DIGIKAM_DATABASE_EXPORT DatesDBJobInfo : public DBJobInfo
{
public:

    explicit DatesDBJobInfo();

    void setStartDate(const QDate& date);
    QDate startDate()   const;

    void setEndDate(const QDate& date);
    QDate endDate()     const;

private:

    QDate m_startDate;
    QDate m_endDate;
};

} // namespace Digikam

#endif // DIGIKAM_DB_JOB_INFO_H
