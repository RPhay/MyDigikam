#ifndef FACERECOGNIZER_H
#define FACERECOGNIZER_H

// Qt include

#include <QImage>

// Local library

#include "digikam_opencv.h"
#include "digikam_export.h"
#include "identity.h"

using namespace Digikam;

namespace RecognitionTest
{
// TODO remove export macro after testing
class DIGIKAM_GUI_EXPORT FaceRecognizer
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

    /**
     *  Try to recognize the given image.
     *  Returns the identity id.
     *  If the identity cannot be recognized, returns -1.
     */
    int recognize(const cv::Mat& inputImage);

    /**
     * @brief findIdenity : look for an identity based on the face image
     * @param inputImage
     * @return
     */
    Identity findIdenity(const cv::Mat& inputImage);

    /**
     * @brief saveIdentity : save identity with label and face embedding
     * @param id
     */
    void saveIdentity(const Identity& id);

private:

    /// Hidden copy constructor and assignment operator.
    FaceRecognizer(const FaceRecognizer&);
    FaceRecognizer& operator=(const FaceRecognizer&);

    class Private;
    Private* d;
};

}
#endif // FACERECOGNIZER_H
