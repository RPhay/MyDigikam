/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-07-09
 * Description : Preprocessor for face recognition
 *
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2019-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "recognitionpreprocessor.h"

// Local includes

#include "openfacepreprocessor.h"
#include "digikam_debug.h"

namespace Digikam
{

class RecognitionPreprocessor::Private
{

public:

    explicit Private();
    ~Private();

    void init(PreprocessorSelection mode);
    cv::Mat preprocess(const cv::Mat& image) const;

private:

    int                   preprocessingMode;

    OpenfacePreprocessor* ofpreprocessor;
};

// -------------------------------------------------------------------------------

RecognitionPreprocessor::Private::Private()
  : preprocessingMode(-1),
    ofpreprocessor   (nullptr)
{
}

RecognitionPreprocessor::Private::~Private()
{
    delete ofpreprocessor;
}

void RecognitionPreprocessor::Private::init(PreprocessorSelection mode)
{
    preprocessingMode = mode;

    switch (mode)
    {
        case OPENFACE:
        {
            ofpreprocessor = new OpenfacePreprocessor;
            ofpreprocessor->loadModels();
            break;
        }

        default:
        {
            qCDebug(DIGIKAM_FACEDB_LOG) << "Error unknown preprocessingMode " << preprocessingMode;
            preprocessingMode = -1;
        }
    }
}

cv::Mat RecognitionPreprocessor::Private::preprocess(const cv::Mat& image) const
{
    switch (preprocessingMode)
    {
        case OPENFACE:
        {
            qCDebug(DIGIKAM_FACEDB_LOG) << "Align face for OpenFace neural network model";

            return ofpreprocessor->process(image);
        }

        default:
        {
            qCDebug(DIGIKAM_FACEDB_LOG) << "Error unknown preprocessingMode " << preprocessingMode;

            return image;
        }
    }
}

// ------------------------------------------------------------------------------------------------------

RecognitionPreprocessor::RecognitionPreprocessor()
    : FacePreprocessor(),
      d               (new Private)
{
}

RecognitionPreprocessor::~RecognitionPreprocessor()
{
    delete d;
}

void RecognitionPreprocessor::init(PreprocessorSelection mode)
{
    d->init(mode);
}

cv::Mat RecognitionPreprocessor::preprocess(const cv::Mat& image) const
{
    return d->preprocess(image);
}

} // namespace Digikam
