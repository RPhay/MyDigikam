/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-01-17
 * Description : Haar Database interface
 *
 * Copyright (C) 2016-2018 by Mario Frank <mario dot frank at uni minus potsdam dot de>
 * Copyright (C) 2003      by Ricardo Niederberger Cabral <nieder at mail dot ru>
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009-2011 by Andi Clemens <andi dot clemens at gmail dot com>
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

#include "haariface_p.h"

// C++ includes

#include <functional>

namespace Digikam
{

HaarIface::HaarIface()
    : d(new Private())
{
    qRegisterMetaType<DuplicatesResultsMap>("HaarIface::DuplicatesResultsMap");
}

HaarIface::HaarIface(const QSet<qlonglong>& images2Scan)
    : HaarIface()
{
    d->rebuildSignatureCache(images2Scan);
}

HaarIface::~HaarIface()
{
    delete d;
}

void HaarIface::setAlbumRootsToSearch(const QList<int>& albumRootIds)
{
    setAlbumRootsToSearch(albumRootIds.toSet());
}

void HaarIface::setAlbumRootsToSearch(const QSet<int>& albumRootIds)
{
    d->setAlbumRootsToSearch(albumRootIds);
}

int HaarIface::preferredSize()
{
    return Haar::NumberOfPixels;
}

bool HaarIface::indexImage(const QString& filename)
{
    QImage image = loadQImage(filename);

    if (image.isNull())
    {
        return false;
    }

    return indexImage(filename, image);
}

bool HaarIface::indexImage(const QString& filename, const QImage& image)
{
    ItemInfo info = ItemInfo::fromLocalFile(filename);

    if (info.isNull())
    {
        return false;
    }

    return indexImage(info.id(), image);
}

bool HaarIface::indexImage(const QString& filename, const DImg& image)
{
    ItemInfo info = ItemInfo::fromLocalFile(filename);

    if (info.isNull())
    {
        return false;
    }

    return indexImage(info.id(), image);
}

bool HaarIface::indexImage(qlonglong imageid, const QImage& image)
{
    if (image.isNull())
    {
        return false;
    }

    d->setImageDataFromImage(image);

    return indexImage(imageid);
}

bool HaarIface::indexImage(qlonglong imageid, const DImg& image)
{
    if (image.isNull())
    {
        return false;
    }

    d->setImageDataFromImage(image);

    return indexImage(imageid);
}

// NOTE: private method: d->m_data has been filled

bool HaarIface::indexImage(qlonglong imageid)
{
    Haar::Calculator haar;
    haar.transform(d->imageData());

    Haar::SignatureData sig;
    haar.calcHaar(d->imageData(), &sig);

    // Store main entry

    DatabaseBlob blob;
    QByteArray array = blob.write(sig);

    ItemInfo info(imageid);

    if (!info.isNull() && info.isVisible())
    {
        SimilarityDbAccess().backend()->execSql(QString::fromUtf8("REPLACE INTO ImageHaarMatrix "
                                                                  " (imageid, modificationDate, uniqueHash, matrix) "
                                                                  " VALUES(?, ?, ?, ?);"),
                                                imageid, info.modDateTime(), info.uniqueHash(), array);
    }

    return true;
}

QString HaarIface::signatureAsText(const QImage& image)
{
    d->setImageDataFromImage(image);

    Haar::Calculator haar;
    haar.transform(d->imageData());
    Haar::SignatureData sig;
    haar.calcHaar(d->imageData(), &sig);

    DatabaseBlob blob;
    QByteArray array = blob.write(sig);

    return QString::fromUtf8(array.toBase64());
}

QPair<double, QMap<qlonglong, double> > HaarIface::bestMatchesForImageWithThreshold(const QString& imagePath,
                                                                                    double requiredPercentage,
                                                                                    double maximumPercentage,
                                                                                    const QList<int>& targetAlbums,
                                                                                        DuplicatesSearchRestrictions
                                                                                    searchResultRestriction,
                                                                                    SketchType type)
{
    DImg image(imagePath);

    if (image.isNull())
    {
        return QPair<double, QMap<qlonglong, double> >();
    }

    d->setImageDataFromImage(image);

    Haar::Calculator haar;
    haar.transform(d->imageData());
    Haar::SignatureData sig;
    haar.calcHaar(d->imageData(), &sig);

    // Remove all previous similarities from pictures

    SimilarityDbAccess().db()->removeImageSimilarity(0);

    // Apply duplicates search for the image. Use the image id 0 which cannot be present.

    return bestMatchesWithThreshold(0,
                                    &sig,
                                    requiredPercentage,
                                    maximumPercentage,
                                    targetAlbums,
                                    searchResultRestriction,
                                    type);
}

QPair<double, QMap<qlonglong, double> > HaarIface::bestMatchesForImageWithThreshold(qlonglong imageId,
                                                                                    double requiredPercentage,
                                                                                    double maximumPercentage,
                                                                                    const QList<int>& targetAlbums,
                                                                                        DuplicatesSearchRestrictions
                                                                                    searchResultRestriction,
                                                                                    SketchType type)
{
    Haar::SignatureData sig;

    if (d->hasSignatureCache())
    {
        if (!d->retrieveSignatureFromCache(imageId, sig))
        {
            return {};
        }
    }
    else
    {
        if (!retrieveSignatureFromDB(imageId, sig))
        {
            return {};
        }
    }

    return bestMatchesWithThreshold(imageId,
                                    &sig,
                                    requiredPercentage,
                                    maximumPercentage,
                                    targetAlbums,
                                    searchResultRestriction,
                                    type);
}

QMap<qlonglong,double> HaarIface::bestMatchesForSignature(const QString& signature,
                                                          const QList<int>& targetAlbums,
                                                          int numberOfResults,
                                                          SketchType type)
{
    QByteArray bytes = QByteArray::fromBase64(signature.toLatin1());

    DatabaseBlob blobReader;
    Haar::SignatureData sig;
    blobReader.read(bytes, sig);

    // Get all matching images with their score and save their similarity to the signature, i.e. id -2

    QMultiMap<double, qlonglong> matches = bestMatches(&sig, numberOfResults, targetAlbums, type);
    QMap<qlonglong, double> result;

    for (QMultiMap<double, qlonglong>::const_iterator it = matches.constBegin() ;
         it != matches.constEnd() ; ++it)
    {
        // Add the image id and the normalised score (make sure that it is positive and between 0 and 1.

        result.insert(it.value(), (0.0 - (it.key() / 100)));
    }

    return result;
}

QMultiMap<double, qlonglong> HaarIface::bestMatches(Haar::SignatureData* const querySig,
                                                    int numberOfResults,
                                                    const QList<int>& targetAlbums,
                                                    SketchType type)
{
    QMap<qlonglong, double> scores = searchDatabase(querySig, type, targetAlbums);

    // Find out the best matches, those with the lowest score
    // We make use of the feature that QMap keys are sorted in ascending order
    // Of course, images can have the same score, so we need a multi map

    QMultiMap<double, qlonglong> bestMatches;
    bool                         initialFill = false;
    double                       score, worstScore, bestScore;
    qlonglong                    id;

    for (QMap<qlonglong, double>::const_iterator it = scores.constBegin() ;
         it != scores.constEnd() ; ++it)
    {
        score = it.value();
        id    = it.key();

        if (!initialFill)
        {
            // as long as the maximum number of results is not reached, just fill up the map

            bestMatches.insert(score, id);
            initialFill = (bestMatches.size() >= numberOfResults);
        }
        else
        {
            // find the last entry, the one with the highest (=worst) score

            QMap<double, qlonglong>::iterator last = bestMatches.end();
            --last;
            worstScore = last.key();

            // if the new entry has a higher score, put it in the list and remove that last one

            if (score < worstScore)
            {
                bestMatches.erase(last);
                bestMatches.insert(score, id);
            }
            else if (score == worstScore)
            {
                bestScore = bestMatches.begin().key();

                // if the score is identical for all entries, increase the maximum result number

                if (score == bestScore)
                {
                    bestMatches.insert(score, id);
                }
            }
        }
    }

/*
    for (QMap<double, qlonglong>::iterator it = bestMatches.begin(); it != bestMatches.end(); ++it)
    {
        qCDebug(DIGIKAM_DATABASE_LOG) << it.key() << it.value();
    }
*/

    return bestMatches;
}

QPair<double, QMap<qlonglong, double> > HaarIface::bestMatchesWithThreshold(qlonglong imageid,
                                                                            Haar::SignatureData* const querySig,
                                                                            double requiredPercentage,
                                                                            double maximumPercentage,
                                                                            const QList<int>& targetAlbums,
                                                                                DuplicatesSearchRestrictions
                                                                            searchResultRestriction,
                                                                            SketchType type)
{
    int albumId                    = CoreDbAccess().db()->getItemAlbum(imageid);
    QMap<qlonglong, double> scores = searchDatabase(querySig,
                                                    type,
                                                    targetAlbums,
                                                    searchResultRestriction,
                                                    imageid,
                                                    albumId);
    double lowest, highest;
    getBestAndWorstPossibleScore(querySig, type, &lowest, &highest);

    // The range between the highest (worst) and lowest (best) score
    // example: 0.2 and 0.5 -> 0.3

    double scoreRange      = highest - lowest;

    // The lower the requiredPercentage is, the higher will the result be.
    // example: 0.7 -> 0.3

    double percentageRange = 1.0 - requiredPercentage;

    // example: 0.2 + (0.3 * 0.3) = 0.2 + 0.09 = 0.29

    double requiredScore   = lowest + scoreRange * percentageRange;

    // Set the supremum which solves the problem that if
    // required == maximum, no results will be returned.
    // Eg, id required == maximum == 50.0, only images with exactly this
    // similarity are returned. But users expect also to see images
    // with similarity 50,x.

    double supremum = (floor(maximumPercentage*100 + 1.0))/100;

    QMap<qlonglong, double> bestMatches;
    double score, percentage, avgPercentage = 0.0;
    QPair<double, QMap<qlonglong, double> > result;
    qlonglong id;

    for (QMap<qlonglong, double>::const_iterator it = scores.constBegin() ;
         it != scores.constEnd() ; ++it)
    {
        score = it.value();
        id    = it.key();

        // If the score of the picture is at most the required (maximum) score and

        if (score <= requiredScore)
        {
            percentage = 1.0 - (score - lowest) / scoreRange;

            // If the found image is the original one (check by id) or the percentage is below the maximum.

            if ((id == imageid) || (percentage < supremum))
            {
                bestMatches.insert(id, percentage);

                // If the current image is not the original, use the images similarity for the average percentage
                // Also, save the similarity of the found image to the original image.

                if (id != imageid)
                {
                    // Store the similarity if the reference image has a valid image id

                    if (imageid > 0)
                    {
                        SimilarityDbAccess().db()->setImageSimilarity(id, imageid, percentage);
                    }

                    avgPercentage += percentage;
                }
            }
        }
    }

    // Debug output

    if (bestMatches.count() > 1)
    {
        // The average percentage is the sum of all percentages
        // (without the original picture) divided by the count of pictures -1.
        // Subtracting 1 is necessary since the original picture is not used for the calculation.

        avgPercentage = avgPercentage / (bestMatches.count() - 1);

        qCDebug(DIGIKAM_DATABASE_LOG) << "Duplicates with id and score:";

        for (QMap<qlonglong, double>::const_iterator it = bestMatches.constBegin() ; it != bestMatches.constEnd() ; ++it)
        {
            qCDebug(DIGIKAM_DATABASE_LOG) << it.key() << QString::number(it.value() * 100) + QLatin1Char('%');
        }
    }

    result.first  = avgPercentage;
    result.second = bestMatches;

    return result;
}

bool HaarIface::fulfillsRestrictions(qlonglong imageId, int albumId,
                                     qlonglong originalImageId,
                                     int originalAlbumId,
                                     const QList<int>& targetAlbums,
                                     DuplicatesSearchRestrictions searchResultRestriction)
{
    if (imageId == originalImageId)
    {
        return true;
    }
    else if (targetAlbums.isEmpty() || targetAlbums.contains(albumId))
    {
        return ( searchResultRestriction == None)                                            ||
               ((searchResultRestriction == SameAlbum)      && (originalAlbumId == albumId)) ||
               ((searchResultRestriction == DifferentAlbum) && (originalAlbumId != albumId));
    }
    else
    {
        return false;
    }
}

/**
 * This method is the core functionality: It assigns a score to every image in the database.
 */
QMap<qlonglong, double> HaarIface::searchDatabase(Haar::SignatureData* const querySig,
                                                  SketchType type, const QList<int>& targetAlbums,
                                                  DuplicatesSearchRestrictions searchResultRestriction,
                                                  qlonglong originalImageId,
                                                  int originalAlbumId)
{
    // The table of constant weight factors applied to each channel and the weight bin

    Haar::Weights weights((Haar::Weights::SketchType)type);

    // layout the query signature for fast lookup

    Haar::SignatureMap queryMapY, queryMapI, queryMapQ;
    queryMapY.fill(querySig->sig[0]);
    queryMapI.fill(querySig->sig[1]);
    queryMapQ.fill(querySig->sig[2]);
    std::reference_wrapper<Haar::SignatureMap> queryMaps[3] = { queryMapY, queryMapI, queryMapQ };

    // Map imageid -> score. Lowest score is best.
    // any newly inserted value will be initialized with a score of 0, as required

    QMap<qlonglong, double> scores;

    // if no cache is used or the cache signature map is empty, query the database

    if (!d->hasSignatureCache())
    {
        d->rebuildSignatureCache();
    }

    for (auto it = d->signatureCache()->cbegin() ; it != d->signatureCache()->cend() ; ++it)
    {
        // If the image is the original one or
        // No restrictions apply or
        // SameAlbum restriction applies and the albums are equal or
        // DifferentAlbum restriction applies and the albums differ
        // then calculate the score.

        const qlonglong& imageId = it.key();
        if (fulfillsRestrictions(imageId, d->albumCache()->value(imageId), originalImageId,
                                 originalAlbumId, targetAlbums, searchResultRestriction))
        {
            const Haar::SignatureData& data = it.value();
            scores[imageId]                 = calculateScore(*querySig, data, weights, queryMaps);
        }
    }

    return scores;
}

QImage HaarIface::loadQImage(const QString& filename)
{
    // NOTE: Can be optimized using DImg.

    QImage image;

    if (JPEGUtils::isJpegImage(filename))
    {
        // use fast jpeg loading

        if (!JPEGUtils::loadJPEGScaled(image, filename, Haar::NumberOfPixels))
        {
            // try QT now.

            if (!image.load(filename))
            {
                return QImage();
            }
        }
    }
    else
    {
        // use default QT image loading

        if (!image.load(filename))
        {
            return QImage();
        }
    }

    return image;
}

bool HaarIface::retrieveSignatureFromDB(qlonglong imageid, Haar::SignatureData& sig)
{
    QList<QVariant> values;
    SimilarityDbAccess().backend()->execSql(QString::fromUtf8("SELECT matrix FROM ImageHaarMatrix WHERE imageid=?"),
                                            imageid, &values);

    if (values.isEmpty())
    {
        return false;
    }

    DatabaseBlob blob;

    blob.read(values.first().toByteArray(), sig);
    return true;
}

void HaarIface::getBestAndWorstPossibleScore(Haar::SignatureData* const sig,
                                             SketchType type,
                                             double* const lowestAndBestScore,
                                             double* const highestAndWorstScore)
{
    Haar::Weights weights(static_cast<Haar::Weights::SketchType>(type));
    double score = 0;

    // In the first step, the score is initialized with the weighted color channel averages.
    // We don't know the target channel average here, we only now its not negative => assume 0

    for (int channel = 0 ; channel < 3 ; ++channel)
    {
        score += weights.weightForAverage(channel) * fabs(sig->avg[channel] /*- targetSig.avg[channel]*/);
    }

    *highestAndWorstScore = score;

    // Next consideration: The lowest possible score is reached if the signature is identical.
    // The first step (see above) will result in 0 - skip it.
    // In the second step, for every coefficient in the sig that have query and target in common,
    // so in our case all 3*40, subtract the specifically assigned weighting.

    score = 0;

    for (int channel = 0 ; channel < 3 ; ++channel)
    {
        Haar::Idx* coefs = sig->sig[channel];

        for (int coef = 0 ; coef < Haar::NumberOfCoefficients ; ++coef)
        {
            score -= weights.weight(d->weightBin.binAbs(coefs[coef]), channel);
        }
    }

    *lowestAndBestScore = score;
}


QMap<QString, QString> HaarIface::writeSAlbumQueries(const DuplicatesResultsMap& searchResults)
{
    // Build search XML from the results. Store list of ids of similar images.

    QMap<QString, QString> queries;

    for (auto it = searchResults.cbegin() ; it != searchResults.cend(); ++it)
    {
        SearchXmlWriter writer;
        writer.writeGroup();
        writer.writeField(QLatin1String("imageid"), SearchXml::OneOf);
        writer.writeValue(it->second);
        writer.finishField();

        // Add the average similarity as field

        writer.writeField(QLatin1String("noeffect_avgsim"), SearchXml::Equal);
        writer.writeValue(it->first * 100);
        writer.finishField();
        writer.finishGroup();
        writer.finish();

        // Use the id of the first duplicate as name of the search

        queries.insert(QString::number(it.key()), writer.xml());
    }

    return queries;
}

void HaarIface::rebuildDuplicatesAlbums(const DuplicatesResultsMap& results, bool isAlbumUpdate)
{
    // Build search XML from the results. Store list of ids of similar images.

    QMap<QString, QString> queries = writeSAlbumQueries(results);

    // Write the new search albums to the database.

    CoreDbAccess access;
    CoreDbTransaction transaction(&access);

    // Full rebuild: delete all old searches.

    if (!isAlbumUpdate)
    {
        access.db()->deleteSearches(DatabaseSearch::DuplicatesSearch);
    }

    // Create new groups, or update existing searches.

    for (QMap<QString, QString>::const_iterator it = queries.constBegin() ;
         it != queries.constEnd() ; ++it)
    {
        if (isAlbumUpdate)
        {
            access.db()->deleteSearch(it.key().toInt());
        }

        access.db()->addSearch(DatabaseSearch::DuplicatesSearch, it.key(), it.value());
    }
}

QSet<qlonglong> HaarIface::imagesFromAlbumsAndTags(const QList<int>& albums2Scan,
                                                   const QList<int>& tags2Scan,
                                                   AlbumTagRelation relation)
{
    QSet<qlonglong> imagesFromAlbums;
    QSet<qlonglong> imagesFromTags;
    QSet<qlonglong> images;

    // Get all items DB id from all albums and all collections

    foreach (int albumId, albums2Scan)
    {
        imagesFromAlbums.unite(CoreDbAccess().db()->getItemIDsInAlbum(albumId).toSet());
    }

    // Get all items DB id from all tags

    foreach (int albumId, tags2Scan)
    {
        imagesFromTags.unite(CoreDbAccess().db()->getItemIDsInTag(albumId).toSet());
    }

    switch (relation)
    {
        case Union:
        {
            // ({} UNION A) UNION T = A UNION T

            images.unite(imagesFromAlbums).unite(imagesFromTags);
            break;
        }

        case Intersection:
        {
            // ({} UNION A) INTERSECT T = A INTERSECT T

            images.unite(imagesFromAlbums).intersect(imagesFromTags);
            break;
        }

        case AlbumExclusive:
        {
            // ({} UNION A) = A

            images.unite(imagesFromAlbums);

            // (A INTERSECT T) = A'

            imagesFromAlbums.intersect(imagesFromTags);

            // A\A' = albums without tags

            images.subtract(imagesFromAlbums);
            break;
        }

        case TagExclusive:
        {
            // ({} UNION T) = TT

            images.unite(imagesFromTags);

            // (A INTERSECT T) = A' = T'

            imagesFromAlbums.intersect(imagesFromTags);

            // T\T' = tags without albums

            images.subtract(imagesFromAlbums);
            break;
        }

        case NoMix:
        {
            if ((albums2Scan.isEmpty() && tags2Scan.isEmpty()))
            {
                qCWarning(DIGIKAM_GENERAL_LOG) << "Duplicates search: Both the albums and the tags "
                                                  "list are non-empty but the album/tag relation "
                                                  "stated a NoMix. Skipping duplicates search";
                return {};
            }
            else
            {
                // ({} UNION A) UNION T = A UNION T = A Xor T

                images.unite(imagesFromAlbums).unite(imagesFromTags);
            }
        }
    }

    return images;
}

HaarIface::DuplicatesResultsMap HaarIface::findDuplicates(const QSet<qlonglong>& images2Scan,
                                                          const QSet<qlonglong>::const_iterator& rangeBegin,
                                                          const QSet<qlonglong>::const_iterator& rangeEnd,
                                                          double requiredPercentage,
                                                          double maximumPercentage,
                                                          DuplicatesSearchRestrictions searchResultRestriction,
                                                          HaarProgressObserver* const observer)
{
    static const QList<int>                 emptyTargetAlbums;
    DuplicatesResultsMap                    resultsMap;
    DuplicatesResultsMap::iterator          resultsIterator;
    QSet<qlonglong>::const_iterator         images2ScanIterator;
    QPair<double, QMap<qlonglong, double> > bestMatches;
    QList<qlonglong>                        duplicates;
    QSet<qlonglong>                         resultsCandidates;
    const bool                              singleThread = rangeBegin == images2Scan.cbegin() &&
                                                           rangeEnd == images2Scan.cend();

    // create signature cache map for fast lookup

    if (!d->hasSignatureCache())
    {
        d->rebuildSignatureCache(images2Scan);
    }

    for (images2ScanIterator = rangeBegin ; images2ScanIterator != rangeEnd ; ++images2ScanIterator)
    {
        if (observer && observer->isCanceled())
        {
            break;
        }

        if (!resultsCandidates.contains(*images2ScanIterator))
        {
            // find images with required similarity

            bestMatches = bestMatchesForImageWithThreshold(*images2ScanIterator,
                                                           requiredPercentage,
                                                           maximumPercentage,
                                                           emptyTargetAlbums,
                                                           searchResultRestriction,
                                                           ScannedSketch);

            // We need only the image ids from the best matches map.

            duplicates = bestMatches.second.keys();

            // the list will usually contain one image: the original. Filter out.

            if (!(duplicates.isEmpty()) && !((duplicates.count() == 1) && (duplicates.first() == *images2ScanIterator)))
            {
                resultsMap.insert(*images2ScanIterator, qMakePair(bestMatches.first, duplicates));

                resultsCandidates << *images2ScanIterator;
                resultsCandidates.unite(duplicates.toSet());
            }
        }

        // if an imageid is not a results candidate, remove it
        // from the cached signature map as well,
        // to greatly improve speed

        if (singleThread && !resultsCandidates.contains(*images2ScanIterator))
        {
            d->signatureCache()->remove(*images2ScanIterator);
        }

        if (observer)
        {
            observer->imageProcessed();
        }
    }

    return resultsMap;
}

double HaarIface::calculateScore(const Haar::SignatureData& querySig,
                                 const Haar::SignatureData& targetSig,
                                 Haar::Weights& weights,
                                 std::reference_wrapper<Haar::SignatureMap>* const queryMaps)
{
    double score = 0.0;

    // Step 1: Initialize scores with average intensity values of all three channels

    for (int channel = 0 ; channel < 3 ; ++channel)
    {
        score += weights.weightForAverage(channel) * fabs(querySig.avg[channel] - targetSig.avg[channel]);
    }

    // Step 2: Decrease the score if query and target have significant coefficients in common

    int x                        = 0;

    for (int channel = 0 ; channel < 3 ; ++channel)
    {
        const Haar::SignatureMap& queryMap = queryMaps[channel];

        for (int coef = 0 ; coef < Haar::NumberOfCoefficients ; ++coef)
        {
            // x is a pixel index, either positive or negative, 0..16384

            x = targetSig.sig[channel][coef];

            // If x is a significant coefficient with the same sign in the query signature as well,
            // decrease the score (lower is better)
            // Note: both method calls called with x accept positive or negative values

            if ((queryMap)[x])
            {
                score -= weights.weight(d->weightBin.binAbs(x), channel);
            }
        }
    }

    return score;
}

} // namespace Digikam
