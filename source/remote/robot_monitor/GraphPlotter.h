#ifndef GRAPHPLOTTER_H
#define GRAPHPLOTTER_H

#include <QtGui>
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>

enum PointTypeEnum {Point, Rectange, Triangle};

struct GraphStruct_t
{
    GraphStruct_t(int _size, QColor _color, enum PointTypeEnum _type, bool _connectPoints)
        : size(_size), color(_color), type(_type), connect(_connectPoints) {}

    std::list<QPointF> points;
    int size;
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
    void addPoint(unsigned int graphIndex, QPointF _newPoint);

    void addGraph( int _size, QColor _color, enum PointTypeEnum _type, bool _connectPoints );


private:
    std::vector<GraphStruct_t> graphs;
    QRectF viewRect;

    const float maxHorizontalDistXAxis = 20;
    const float maxVerticalDistXAxis = 30;
    const float maxHorizontalDistYAxis = 30;
    const float maxVerticalDistYAxis = 20;
    const float mouseWheelSkalePerDegree = 0.01;

    QLine actXAxis, actYAxis;
    QRect actDrawRect;

    void setupAxis(QRect _viewPort);
    QLine getYAxis(QRect _viewPort);
    QLine getXAxis(QRect _viewPort);
    QRect getDrawRect( QRect _viewPort);

    void drawLabels(QPainter &_painter );
    void drawPoints(QPainter &_painter);


    // Brushes
    QPen thinBlackPen;
    QBrush whiteBrush;

    void setupBrushes();

    bool mousePressed = false;
    QPoint startMouseDragPos;
    QPointF startViewPortDragPos;
};

#endif // GRAPHPLOTTER_H
