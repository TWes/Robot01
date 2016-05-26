#include "main_window.hpp"

main_window::main_window()
{
    sensor_connection = new Sensor_Connection( QApplication::arguments().at(1).toStdString() , 2553, this);

    sensor_connection->start_server();

    this->centralWidget = new QWidget( this );

    QGridLayout *layout = new QGridLayout( this );

    this->MapWidget = new map_widget( this );
    layout->addWidget( this->MapWidget, 1, 0, 1, 1 );

    this->StatusWidget = new status_widget( this );
    layout->addWidget( this->StatusWidget, 0, 0, 1, 1 );


    this->centralWidget->setLayout( layout );
    this->setCentralWidget( this->centralWidget );
}

main_window::~main_window()
{
    sensor_connection->end_server();

    std::cout << "Server heruntergefahren" << std::endl;
}
