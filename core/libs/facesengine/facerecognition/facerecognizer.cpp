#include "facerecognizer.h"


// Qt includes

#include <QElapsedTimer>

// Local includes

//#include "facedbaccess.h"
//#include "facedb.h"
#include "dnnfacemodel.h"
#include "digikam_debug.h"
#include "faceextractor.h"
//#include "dnndbscan.h"

using namespace Digikam;

namespace RecognitionTest
{

class FaceRecognizer::Private
{
public:

    Private()
        : extractor(new FaceExtractor)
    {
    }

    ~Private()
    {
        delete extractor;
    }

public:
    FaceExtractor* extractor;
};

FaceRecognizer::FaceRecognizer()
    : d(new Private)
{

}

FaceRecognizer::~FaceRecognizer()
{
    delete d;
}

}
