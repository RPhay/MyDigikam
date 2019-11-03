/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-01-03
 * Description : Abstract base classes for all face prediction and recognition models.
 *               Inspired from OpenCV contrib module 'face'
 *               https://github.com/Itseez/opencv_contrib/tree/master/modules/face
 *
 * Copyright (C) 2010      by Aditya Bhatt <adityabhatt at gmail dot com>
 * Copyright (C) 2010      by Alex Jironkin <alexjironkin at gmail dot com>
 * Copyright (c) 2011-2012 by Philipp Wagner <bytefish at gmx dot de>
 * Copyright (C) 2010-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "opencv_face.h"

// C++ includes

#include <iostream>
#include <set>
#include <limits>

// Local includes

#include "digikam_opencv.h"

namespace cv
{

namespace face
{

static std::pair<int, double> toPair(const StandardCollector::PredictResult& val)
{
    return std::make_pair(val.label, val.distance);
}

static bool pairLess(const std::pair<int, double>& lhs, const std::pair<int, double>& rhs)
{
    return (lhs.second < rhs.second);
}

// ---------------------------------------------------------------------------------------------

StandardCollector::StandardCollector(double threshold_)
    : threshold(threshold_)
{
    init(0);
}

void StandardCollector::init(size_t size)
{
    minRes = PredictResult();
    data.clear();
    data.reserve(size);
}

bool StandardCollector::collect(int label, double dist)
{
    if (dist < threshold)
    {
        PredictResult res(label, dist);

        if (res.distance < minRes.distance)
        {
            minRes = res;
        }

        data.push_back(res);
    }

    return true;
}

int StandardCollector::getMinLabel() const
{
    return minRes.label;
}

double StandardCollector::getMinDist() const
{
    return minRes.distance;
}

std::vector< std::pair<int, double> > StandardCollector::getResults(bool sorted) const
{
    std::vector< std::pair<int, double> > res(data.size());
    std::transform(data.begin(), data.end(), res.begin(), &toPair);

    if (sorted)
    {
        std::sort(res.begin(), res.end(), &pairLess);
    }

    return res;
}

std::map<int, double> StandardCollector::getResultsMap() const
{
    std::map<int, double> res;

    for (std::vector<PredictResult>::const_iterator it = data.begin() ;
         it != data.end() ; ++it)
    {
        std::map<int, double>::iterator lbl = res.find(it->label);

        if (lbl == res.end())
        {
            res.insert(toPair(*it));
        }
        else if (it->distance < lbl->second)
        {
            lbl->second = it->distance;
        }
    }

    return res;
}

Ptr<StandardCollector> StandardCollector::create(double threshold)
{
    return makePtr<StandardCollector>(threshold);
}

// ---------------------------------------------------------------------------------------------

std::vector<int> FaceRecognizer::getLabelsByString(const String& str) const
{
    std::vector<int> labels;

    for (std::map<int, String>::const_iterator it = _labelsInfo.begin() ;
         it != _labelsInfo.end() ; ++it)
    {
        size_t found = (it->second).find(str);

        if (found != String::npos)
        {
            labels.push_back(it->first);
        }
    }

    return labels;
}

String FaceRecognizer::getLabelInfo(int label) const
{
    std::map<int, String>::const_iterator iter(_labelsInfo.find(label));

    return ((iter != _labelsInfo.end()) ? iter->second : "");
}

void FaceRecognizer::setLabelInfo(int label, const String& strInfo)
{
    _labelsInfo[label] = strInfo;
}

void FaceRecognizer::update(InputArrayOfArrays src, InputArray labels)
{
    (void)src;
    (void)labels;
    String error_msg = format("This FaceRecognizer does not support updating, "
                              "you have to use FaceRecognizer::train to update it.");
    CV_Error(Error::StsNotImplemented, error_msg);
}

#if OPENCV_TEST_VERSION(3,4,0)
void FaceRecognizer::load(const String& filename)
#else
void FaceRecognizer::read(const String& filename)
#endif
{
    FileStorage fs(filename, FileStorage::READ);

    if (!fs.isOpened())
    {
        CV_Error(Error::StsError, "File can't be opened for writing!");
    }

#if OPENCV_TEST_VERSION(3,4,0)
    this->load(fs);
#else
    this->read(fs);
#endif

    fs.release();
}

#if OPENCV_TEST_VERSION(3,4,0)
void FaceRecognizer::save(const String& filename) const
#else
void FaceRecognizer::write(const String& filename) const
#endif
{
    FileStorage fs(filename, FileStorage::WRITE);

    if (!fs.isOpened())
    {
        CV_Error(Error::StsError, "File can't be opened for writing!");
    }

#if OPENCV_TEST_VERSION(3,4,0)
    this->save(fs);
#else
    this->write(fs);
#endif

    fs.release();
}

int FaceRecognizer::predict(InputArray src) const
{
    int    _label = 0;
    double _dist  = 0.0;

    predict(src, _label, _dist);

    return _label;
}

void FaceRecognizer::predict(InputArray src,
                             CV_OUT int& label,
                             CV_OUT double& confidence) const
{
    Ptr<StandardCollector> collector = StandardCollector::create(getThreshold());

    predict(src, collector);

    label                            = collector->getMinLabel();
    confidence                       = collector->getMinDist();
}

} // namespace face

} // namespace cv
