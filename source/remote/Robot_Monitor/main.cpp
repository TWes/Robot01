#include <QtGui>
#include <iostream>

#include "main_window.hpp"


Sensor_Connection *sensor_connection = NULL;


int main( int argc, char** argv )
{
    if( argc != 2 )
    {
        std::cout << "Wrong number of Parameters. Usage <ip>." << std::endl;
        return -1;
    }

    QApplication app(argc, argv);

    main_window window;

    window.show();

    return app.exec();
}
