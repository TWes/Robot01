#ifndef GRAPHPLOTTER_H
#define GRAPHPLOTTER_H

#include <QtWidgets>
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>
#include <QtCore>

#include <iostream>

enum PointTypeEnum {Point, Rectange, Triangle};

struct RectF{

    RectF() {}
    RectF(float _x, float _y, float _w, float _h ) : x(_x), y(_y), w(_w), h(_h){}

    float x;
    float y;
    float w;
    float h;

    bool contains( QPointF _pointToTest )
    {
        if( _pointToTest.x() >= x && _pointToTest.x() <= ( x + w)
                && _pointToTest.y() >= y && _pointToTest.y() <= (y + h) )
        {
            return true;
        }

        return false;
    }

    QPointF topLeft()
    {
        return QPointF(x, y);
    }

};


struct GraphStruct_t
{
    GraphStruct_t(int _size, QColor _color, enum PointTypeEnum _type, bool _connectPoints)
        : size(_size), color(_color), type(_type), connect(_connectPoints) {}

    std::list<QPointF> points;
    int maxPoints = 1000;

    int size;// size of dots
    QColor color;
    enum PointTypeEnum type;
    bool connect;
};

class GraphPlotter : public QWidget
{
    Q_OBJECT

public:
    GraphPlotter(QWidget *parent = 0);
    ~GraphPlotter();

    void paintEvent(QPaintEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    static GraphStruct_t defaultGraphStyle;

public slots:
    void clear();
    void addPoint(QPointF _newPoint);
    void addPoint(unsigned int graphIndex, QPointF _newPoint, bool scrollTonewPoint = false);

    void addGraph( int _size, QColor _color, enum PointTypeEnum _type, bool _connectPoints );


private:
    std::vector<GraphStruct_t> graphs;
    RectF viewRect;

    const float maxHorizontalDistXAxis = 20;
    const float maxVerticalDistXAxis = 30;
    const float maxHorizontalDistYAxis = 30;
    const float maxVerticalDistYAxis = 20;
    const float mouseWheelSkalePerDegree = 0.01;
    const float scalePerPixelMove = 0.0001;

    QLine actXAxis, actYAxis, axtZeroAxis;
    QRect actDrawRect;

    void setupAxis(QRect _viewPort);
    QLine getYAxis(QRect _viewPort);
    QLine getXAxis(QRect _viewPort);
    QLine getZeroAxis();
    QRect getDrawRect( QRect _viewPort);

    QPoint MousePosition;
    void drawMousePos(QPainter &_painter);


    void drawLabels(QPainter &_painter );
    void drawPoints(QPainter &_painter);

    void scrollToPoint( QPointF _pointToScrollTo );

    // Brushes
    QPen thinBlackPen;
    QBrush whiteBrush;

    void setupBrushes();

    bool leftmousePressed = false;
    bool rightmousePressed = false;
    QPoint startMouseDragPos;
    QPointF startViewPortDragPos;
};

#endif // GRAPHPLOTTER_H
