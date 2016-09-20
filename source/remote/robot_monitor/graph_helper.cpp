#include "graph_helper.hpp"

GraphHelper::GraphHelper(QWidget *parent) : QWidget( parent)
{

    layout = new QGridLayout(this);

    testLabel = new QLabel("GraphHelper", this );
    layout->addWidget( testLabel, 0,0 );

    plot1Combo = new QComboBox(this);
    plot1Combo->addItem("none");
    plot1Combo->addItem("GET_RAW_GYROSKOPE");
    plot1Combo->addItem("GET_RAW_ACCELEROMETER");
    plot1Combo->addItem("GET_FILTERED_GYROSCOPE");
    layout->addWidget( plot1Combo, 0,0);

    plot1SubscribeButton = new QPushButton("Subscribe", this);
    layout->addWidget( plot1SubscribeButton, 0,1);

    QObject::connect( plot1SubscribeButton, SIGNAL(clicked()),
                      this, SLOT(emitPlot1Subscribe()) );


    plot1UnsubscribeButton = new QPushButton("Unsubscribe", this);
    layout->addWidget( plot1UnsubscribeButton, 0,2);
    QObject::connect( plot1UnsubscribeButton, SIGNAL(clicked()),
                      this, SLOT(emitPlot1Unsubscribe()) );


    this->setLayout( layout );
}


GraphHelper::~GraphHelper()
{
    if( layout != NULL )
    {
        delete layout;
    }


    if( testLabel != NULL )
    {
        delete testLabel;
    }
}


void GraphHelper::emitPlot1Subscribe()
{
    // Get active combo box
    switch( plot1Combo->currentIndex() )
    {
    case 1:
        sensor_connection->init_UDP_Var( GET_RAW_GYROSKOPE, PLOT1 , 100 );
        break;
    case 2:
        sensor_connection->init_UDP_Var( GET_RAW_ACCELEROMETER, PLOT1   );
        break;
    case 3:
        sensor_connection->init_UDP_Var( GET_FILTERED_GYROSCOPE, PLOT1   );
        break;

    }
}

void GraphHelper::emitPlot1Unsubscribe()
{

}
