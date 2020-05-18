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

class Q_DECL_HIDDEN FaceRecognizer::Private
{
public:

    Private(bool debug)
        : debugMode(debug),
          extractor(new FaceExtractor)
    {
    }

    ~Private()
    {
        delete extractor;
    }

public:

    bool           debugMode;
    FaceExtractor* extractor;
};

FaceRecognizer::FaceRecognizer(bool debug)
    : d(new Private(debug))
{

}

FaceRecognizer::~FaceRecognizer()
{
    delete d;
}

}
