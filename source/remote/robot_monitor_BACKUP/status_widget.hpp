#ifndef STATUS_WIDGET_HPP
#define STATUS_WIDGET_HPP

#include <QtGui>

#include "Sensor_structs.hpp"
#include "Sensor_Connection.hpp"

extern Sensor_Connection *sensor_connection;


class status_widget : public QWidget
{
    Q_OBJECT

public:
    status_widget(QWidget *parent);


    void paintEvent(QPaintEvent *event);
};


#endif // STATUS_WIDGET_HPP
