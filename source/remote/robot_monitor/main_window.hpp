#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtGui>

#include "map_widget.hpp"
#include "Sensor_Connection.hpp"
#include <QtCore>

#include "status_widget.hpp"
#include "output_box.hpp"

extern Sensor_Connection *sensor_connection;

class main_window : public QMainWindow
{
    Q_OBJECT

public:
    main_window();
    ~main_window();


    QWidget *centralWidget;


    map_widget *MapWidget;
    status_widget *StatusWidget;
    output_box *debug_output;

    // Globaly needed

};




#endif // MAIN_WINDOW_H
