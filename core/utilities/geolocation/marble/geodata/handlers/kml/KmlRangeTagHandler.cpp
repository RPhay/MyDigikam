#include "KmlRangeTagHandler.h"

#include "digikam_debug.h"

#include "KmlElementDictionary.h"

#include "MarbleGlobal.h"
#include "GeoDataLookAt.h"
#include "GeoParser.h"
#include "GeoDataCoordinates.h"


namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER (range)
    GeoNode *KmlrangeTagHandler::parse (GeoParser & parser) const
    {
        Q_ASSERT (parser.isStartElement ()
                  && parser.isValidElement (QString::fromUtf8(kmlTag_range)));

        GeoStackItem parentItem = parser.parentElement ();

        if ( parentItem.is<GeoDataLookAt>() ){

            QString rangeTemp = parser.readElementText().trimmed();
            qreal range = rangeTemp.toDouble();

            parentItem.nodeAs<GeoDataLookAt>()->setRange( range );
        }

      return nullptr;
    }
}
}

