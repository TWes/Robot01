#include "GraphPlotter.h"

GraphStruct_t GraphPlotter::defaultGraphStyle = GraphStruct_t(1, Qt::black, Point, false);

GraphPlotter::GraphPlotter(QWidget *parent)
    : QWidget(parent)
{
    viewRect = RectF(0,0,1,1);

    /*this->addGraph(2, Qt::red, Point, true );

    this->addPoint(1, QPointF( 0.25, 0.25));
    this->addPoint(1, QPointF( 0.5, 0.5));
    this->addPoint(1, QPointF( 0.75, 0.75));


    this->addGraph(3, Qt::blue, Point, false );
    this->addPoint(2, QPointF( 0.25, 0.75));
    this->addPoint(2, QPointF( 0.25, 0.7));
    this->addPoint(2, QPointF( 0.2, 0.75)); */

    setupBrushes();
}

GraphPlotter::~GraphPlotter()
{

}

void GraphPlotter::addPoint(QPointF _newPoint)
{
    if( this->graphs.size() <= 0 )
    {
        this->graphs.push_back( GraphPlotter::defaultGraphStyle );
    }

    this->graphs.begin()->points.push_back( _newPoint );
}

void GraphPlotter::addPoint(unsigned int graphIndex, QPointF _newPoint, bool scrollTonewPoint)
{
    if( this->graphs.size() <= graphIndex )
    {
        qDebug() << "Tried to add Point to graph with Index: " << graphIndex;
    }

    this->graphs.at( graphIndex).points.push_back( _newPoint );


    if( scrollTonewPoint )
    {
        this->scrollToPoint( _newPoint );
    }

    this->update();

}

void GraphPlotter::clear()
{
    for( GraphStruct_t graph : graphs )
    {
        graph.points.erase( graph.points.begin(), graph.points.end() );
    }

    graphs.erase( graphs.begin(), graphs.end() );
}

void GraphPlotter::paintEvent(QPaintEvent *event)
{
    this->setMinimumSize( 100, 100 );

    QPainter painter;
    painter.begin( this );

    setupAxis( event->rect() );

    painter.fillRect( actDrawRect, Qt::white );

    painter.setPen( thinBlackPen );

    painter.drawLine( actYAxis );
    painter.drawLine( actXAxis );
    painter.drawLine( getZeroAxis() );

    drawLabels( painter );

    drawPoints( painter );

    painter.end();
}

void GraphPlotter::addGraph( int _size, QColor _color, enum PointTypeEnum _type, bool _connectPoints )
{
    if( this->graphs.size() == 0 )
    {
        this->graphs.push_back( GraphPlotter::defaultGraphStyle );
    }

    this->graphs.push_back( GraphStruct_t(_size, _color, _type, _connectPoints ) );
}

QLine GraphPlotter::getYAxis( QRect _viewPort )
{
    float distToLeft = _viewPort.width() * 0.1;
    if( distToLeft > maxHorizontalDistYAxis ) distToLeft = maxHorizontalDistYAxis;

    float distToTop = _viewPort.height() * 0.1;
    if( distToTop > maxVerticalDistYAxis ) distToTop = maxVerticalDistYAxis;

    float lineLength = _viewPort.height() - 2 * distToTop;

    QLine AxisLine = QLine( distToLeft, distToTop, distToLeft, distToTop + lineLength );

    return AxisLine;
}

QLine GraphPlotter::getXAxis( QRect _viewPort )
{
    float distToBottom = _viewPort.height() * 0.1;
    if( distToBottom > maxVerticalDistXAxis ) distToBottom = maxVerticalDistXAxis;

    float distToLeft = _viewPort.width() * 0.1;
    if( distToLeft > maxHorizontalDistXAxis ) distToLeft = maxHorizontalDistXAxis;

    float lineLength = _viewPort.width() - 2 * distToLeft;

    QLine AxisLine = QLine( distToLeft, _viewPort.height() - distToBottom, distToLeft + lineLength, _viewPort.height() - distToBottom);

    return AxisLine;
}

QLine GraphPlotter::getZeroAxis()
{
    float fracture = (-viewRect.y) / viewRect.h;

    if( fracture > 0 && fracture <= 1)
    {
        QPoint startPoint( actYAxis.x1(), actDrawRect.height() - fracture * actDrawRect.height() + actDrawRect.y()  );

        QLine zeroLine( startPoint, QPoint( actXAxis.x2(), startPoint.y() ));

        return zeroLine;
    }

    return actXAxis;

}


QRect GraphPlotter::getDrawRect( QRect _viewPort)
{
    float distToTop = _viewPort.height() * 0.1;
    if( distToTop > maxVerticalDistYAxis ) distToTop = maxVerticalDistYAxis;

    float distToBottom = _viewPort.height() * 0.1;
    if( distToBottom > maxVerticalDistXAxis ) distToBottom = maxVerticalDistXAxis;

    float distToLeft = _viewPort.width() * 0.1;
    if( distToLeft > maxHorizontalDistYAxis ) distToLeft = maxHorizontalDistYAxis;

    float distToRight = _viewPort.width() * 0.1;
    if( distToRight > maxHorizontalDistXAxis) distToRight = maxHorizontalDistXAxis;

    QRect DrawRect = QRect( distToLeft, distToTop,
                            _viewPort.width() - ( distToLeft + distToRight),
                            _viewPort.height() - ( distToTop + distToBottom ) );

    return DrawRect;
}

void GraphPlotter::setupBrushes()
{
    thinBlackPen.setWidth( 1 );
    thinBlackPen.setColor( QColor::fromRgb(0,0,0) );

    whiteBrush.setColor( QColor::fromRgb(255,255,255));
}

void GraphPlotter::setupAxis(QRect _viewPort)
{
    this->actDrawRect = getDrawRect( _viewPort );

    this->actXAxis = getXAxis( _viewPort );
    this->actYAxis = getYAxis( _viewPort );
}

void GraphPlotter::drawLabels(QPainter &_painter)
{
    QFontMetrics FontMetric = _painter.fontMetrics();

    // Draw top Y Label
    QString topYLabel = QString::number(viewRect.h + viewRect.y, 10, 2 );
    int topYLabelXPos = actDrawRect.x();
    _painter.drawText(  topYLabelXPos , actDrawRect.y(), 100, 100, 0, topYLabel );

    // Draw bottom y Label
    QString bottomYLabel = QString::number(viewRect.y, 10, 2 );
    int bottomYLabelXPos = actDrawRect.x();
    _painter.drawText( bottomYLabelXPos, actDrawRect.y() + actDrawRect.height() - 15, 100, 100, 0, bottomYLabel );

    // Draw left x Label
    QString leftXLabel = QString::number(viewRect.x, 10, 2 );
    int leftXLabelPos = actDrawRect.x() + 0.5 * FontMetric.width( leftXLabel);
    _painter.drawText( leftXLabelPos, actDrawRect.y() + actDrawRect.height(), 100, 100, 0, leftXLabel );

    // Draw right x Label
    QString rightXLabel = QString::number(viewRect.w + viewRect.x, 10, 2 );
    int rightXLabelPos = actDrawRect.x() + actDrawRect.width() - FontMetric.width( rightXLabel ) * 0.5;
    _painter.drawText( rightXLabelPos, actDrawRect.y() + actDrawRect.height(), 100, 100, 0, rightXLabel );

}

void GraphPlotter::drawPoints(QPainter &_painter)
{
    QPen actPen = _painter.pen();

    for( GraphStruct_t graphToDraw : graphs )
    {
        // change Color
        if( actPen.color() != graphToDraw.color )
        {
            actPen.setColor( graphToDraw.color );
            _painter.setPen( actPen );
        }

        // Set Size
        if( actPen.width() != graphToDraw.size )
        {
            actPen.setWidth( graphToDraw.size );
            _painter.setPen( actPen );
        }


        QPoint lastPoint;
        bool firstPoint = true;

        for( QPointF pointToDraw : graphToDraw.points)
        {

            if( !viewRect.contains( pointToDraw )) continue;

            // calc draw points
            QPoint drawPoint;

            drawPoint.setX( actDrawRect.x() + ((pointToDraw.x() - viewRect.x) * ( actDrawRect.width() / viewRect.w)) );
            drawPoint.setY( (actDrawRect.height() + actDrawRect.y()) - ( pointToDraw.y() - viewRect.y ) * ( actDrawRect.height() / viewRect.h ) );

            switch( graphToDraw.type )
            {
            case PointTypeEnum::Triangle:
            case PointTypeEnum::Point:
                _painter.drawEllipse( drawPoint, graphToDraw.size, graphToDraw.size);
            case PointTypeEnum::Rectange:
            default:
                _painter.drawPoint( drawPoint );
                break;
            }

            if( graphToDraw.connect && !firstPoint )
            {
                _painter.drawLine( lastPoint, drawPoint );

                lastPoint = drawPoint;
            }

            lastPoint = drawPoint;
            firstPoint = false;
        }
    }

}

void GraphPlotter::wheelEvent(QWheelEvent *event)
{
    float rotationDegree = 0.125 * event->delta();
    float scaleChange = mouseWheelSkalePerDegree * rotationDegree;

    float scaleChangeFactor = 1 + scaleChange;

    float newWidth = viewRect.w / scaleChangeFactor;
    float newHeight = viewRect.h /scaleChangeFactor;

    float deltaWidth = viewRect.w - newWidth;
    float deltaHeight = viewRect.h - newHeight;

    float newXPos = viewRect.x + 0.5 * deltaWidth;
    float newYPos = viewRect.y + 0.5 * deltaHeight;

    viewRect.x = newXPos;
    viewRect.y = newYPos;
    viewRect.w = newWidth;
    viewRect.h = newHeight;

    this->update();
}

void GraphPlotter::mousePressEvent(QMouseEvent *event)
{
    if( event->button() == Qt::LeftButton )
    {
        startMouseDragPos = event->pos();
        startViewPortDragPos = viewRect.topLeft();

        leftmousePressed = true;
    }

    if( event->button() == Qt::RightButton )
    {
        startMouseDragPos = event->pos();
        startViewPortDragPos = viewRect.topLeft();

        rightmousePressed = true;
    }

}

void GraphPlotter::mouseReleaseEvent(QMouseEvent *event)
{
    if( event->button() == Qt::LeftButton )
    {
        leftmousePressed = false;
    }

    if( event->button() == Qt::RightButton )
    {
        rightmousePressed = false;
    }

}

void GraphPlotter::mouseMoveEvent(QMouseEvent *event)
{
    if( leftmousePressed )
    {
        QPoint deltaPos = startMouseDragPos - event->pos();

        float horizontalScale = viewRect.w / actDrawRect.width();
        float verticalScale = viewRect.h / actDrawRect.height();

        float newXPos = startViewPortDragPos.x() + horizontalScale * deltaPos.x();
        float newYPos = startViewPortDragPos.y() - verticalScale * deltaPos.y();

        QPointF newPos = QPointF(newXPos, newYPos );

        viewRect = RectF( newPos.x(), newPos.y(), viewRect.w, viewRect.h );

        this->update();
    }
    else if( rightmousePressed )
    {
        QPoint deltaPos = startMouseDragPos - event->pos();

        float scaleChangeX = deltaPos.x() * scalePerPixelMove;
        float scaleChangeFactorX = 1 + scaleChangeX;

        float scaleChangeY = deltaPos.y() * scalePerPixelMove;
        float scaleChangeFactorY = 1 + scaleChangeY;


        viewRect = RectF( viewRect.x, viewRect.y, viewRect.w / scaleChangeFactorX, viewRect.h/scaleChangeFactorY );

        this->update();


    }
}

void GraphPlotter::scrollToPoint( QPointF _pointToScrollTo )
{
    float newRightRange = _pointToScrollTo.x() + (0.2 * viewRect.w );

    viewRect = RectF( newRightRange - viewRect.w, viewRect.y, viewRect.w, viewRect.h);
}


