#ifndef FACERECOGNIZER_H
#define FACERECOGNIZER_H

// Qt include

#include <QImage>

// Local library

#include "digikam_opencv.h"

namespace RecognitionTest
{

class FaceRecognizer
{
public:

    /**
     *  @brief FaceRecognizer:Master class to control entire recognition using OpenFace algorithm
     */
    explicit FaceRecognizer(bool debug = false);
    ~FaceRecognizer();

public:

    /**
     *  Returns a cvMat created from the inputImage, optimized for recognition
     */
    cv::Mat prepareForRecognition(const QImage& inputImage);

private:

    /// Hidden copy constructor and assignment operator.
    FaceRecognizer(const FaceRecognizer&);
    FaceRecognizer& operator=(const FaceRecognizer&);

    class Private;
    Private* d;
};

}
#endif // FACERECOGNIZER_H
