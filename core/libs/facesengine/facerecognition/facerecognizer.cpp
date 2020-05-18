#include "facerecognizer.h"


// Qt includes

#include <QElapsedTimer>

// Local includes

//#include "facedbaccess.h"
//#include "facedb.h"
#include "dnnfacemodel.h"
#include "digikam_debug.h"
#include "dnnfaceextractor.h"
#include "recognitionpreprocessor.h"
//#include "dnndbscan.h"

using namespace Digikam;

namespace RecognitionTest
{

class FaceRecognizer::Private
{
public:

    Private()
    {
    }

    ~Private()
    {
    }

public:

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
