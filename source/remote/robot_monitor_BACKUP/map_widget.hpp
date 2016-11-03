#ifndef MAP_WIDGET_HPP
#define MAP_WIDGET_HPP

#include <QtGui>
#include "Sensor_Connection.hpp"

extern Sensor_Connection *sensor_connection;

class map_widget : public QWidget
{
    Q_OBJECT

public:
    map_widget( QWidget *parent);
    ~map_widget();


    void paintEvent(QPaintEvent *event);

    QRect visible_rect;

private:
    void drawKoodinateFrame(QPainter *painter , QPaintEvent *event);
    void drawRobotPose(QPainter *painter , QPaintEvent *event);

    QPoint convertToActFrame(double x_cal, double y_val, QPaintEvent *event);


};




#endif // MAP_WIDGET_HPP
