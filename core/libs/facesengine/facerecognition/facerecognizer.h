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

    explicit FaceRecognizer(bool debug = false);
    ~FaceRecognizer();

private:

    class Private;
    Private* d;
};

}
#endif // FACERECOGNIZER_H
