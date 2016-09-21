#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtGui>
#include <QtCore>

#include "map_widget.hpp"
#include "GraphPlotter.h"
#include "graph_helper.hpp"
#include "Sensor_Connection.hpp"
#include "status_widget.hpp"
#include "output_box.hpp"

extern Sensor_Connection *sensor_connection;
extern GraphHelper *globalGraphHelper;

class main_window : public QMainWindow
{
    Q_OBJECT

public:
    main_window();
    ~main_window();


    QWidget *centralWidget;
    QGridLayout *layout;

    map_widget *MapWidget;
    status_widget *StatusWidget;
    output_box *debug_output;
    GraphHelper *graphHelper;
    GraphPlotter *graph_plotter;

    // Globaly needed

};




#endif // MAIN_WINDOW_H
