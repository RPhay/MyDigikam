#include "focuspointitem.h"

// Qt includes

#include <QPainter>
#include <QPen>

// Local includes

#include "digikam_debug.h"
#include "assignnamewidgetstates.h"

namespace Digikam
{

FocusPointItem::FocusPointItem(QGraphicsItem* const parent)
    : RegionFrameItem(parent),
      m_color(QColor::fromRgb(0,0,0,255)), // alpha is 100 to let more transparency
      m_width(3)
{
    m_widgetName = new QLabel(nullptr);
    // setHudWidget(m_widgetName);
}

FocusPointItem::~FocusPointItem()
{
    
}

void FocusPointItem::setPoint(const FocusPoint& point)
{
    m_point = point;
    setEditable(false);

    if(m_point.getType() == FocusPoint::TypePoint::Inactive)
    {
        m_color.setAlpha(130);
        m_width = 1;
    }

    if((m_point.getType() & FocusPoint::TypePoint::Selected) == FocusPoint::TypePoint::Selected)
    {
        m_color.setRed(255);
    }
}

FocusPoint FocusPointItem::point() const
{
    return m_point;
}

void FocusPointItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    QPen pen;
    pen.setWidth(m_width);
    pen.setColor(m_color);

    QRectF drawRect          = boundingRect();

    painter->setPen(pen);
    painter->drawRect(drawRect);
}

void FocusPointItem::setEditable(bool allowEdit)
{
    changeFlags(GeometryEditable, allowEdit);
}

} // namespace Digikam
