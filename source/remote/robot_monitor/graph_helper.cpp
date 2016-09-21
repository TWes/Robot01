#include "graph_helper.hpp"

GraphHelper::GraphHelper(GraphPlotter *plotter, QWidget *parent) : QWidget( parent)
{
    graphPlotter = plotter;

    layout = new QGridLayout(this);

    checkBoxes[0] = new QCheckBox("GyroRawX", this);
    checkBoxes[1] = new QCheckBox("GyroRawY", this);
    checkBoxes[2] = new QCheckBox("GyroRawZ", this);
    checkBoxes[3] = new QCheckBox("AccRawX", this);
    checkBoxes[4] = new QCheckBox("AccRawY", this);
    checkBoxes[5] = new QCheckBox("AccRawZ", this);
    checkBoxes[6] = new QCheckBox("MagnRawX", this);
    checkBoxes[7] = new QCheckBox("MagnRawY", this);
    checkBoxes[8] = new QCheckBox("MagnRawZ", this);

    for( int i = 0; i < nmbOfBoxes; i++ )
    {
        layout->addWidget( checkBoxes[i], i % 3, (int) i/3 );

        QObject::connect( checkBoxes[i], SIGNAL(toggled(bool)),
                          this, SLOT(CheckBoxesChanged()));
    }

    this->setLayout( layout );


    plotter->addGraph(1, Qt::red, Point, true); // 1
    plotter->addGraph(1, Qt::blue, Point, true); // 2
    plotter->addGraph(1, Qt::green, Point, true); // 3
    plotter->addGraph(1, Qt::gray, Point, true); // 4
    plotter->addGraph(1, Qt::darkCyan, Point, true); // 5
    plotter->addGraph(1, Qt::darkMagenta, Point, true); // 6
    plotter->addGraph(1, Qt::darkGray, Point, true); // 7
    plotter->addGraph(1, Qt::darkRed, Point, true); // 8
    plotter->addGraph(1, Qt::black, Point, true); // 9

    timeval time;
    gettimeofday( &time, NULL );

    startTimestamp = (time.tv_sec * (double)1000) + (time.tv_usec / (double)1000);
}


GraphHelper::~GraphHelper()
{
    if( layout != NULL )
    {
        delete layout;
    }

    for( int i = 0; i <= nmbOfBoxes; i++ )
    {
        if( checkBoxes[i] != NULL )
        {
            delete checkBoxes[i];
        }
    }

}


void GraphHelper::CheckBoxesChanged()
{
    int changedBox = -1;
    bool newState = false;
    for(int i = 0 ; i < nmbOfBoxes; i++ )
    {
        if( (checkBoxes[i]->checkState() >= 1) != boxValues[i] )
        {
            boxValues[i] = checkBoxes[i]->checkState();

            newState = boxValues[i];
            changedBox = i;
        }
    }

    if( changedBox < 0 ) return;

    // We need to deal with the IMU Values
    if( changedBox >= 0 && changedBox < 9 )
    {
        if( newState )
        {
            boxesThatNeedsIMU++;

            if( boxesThatNeedsIMU == 1 )
            {
                // Start IMU
                sensor_connection->init_UDP_Var( GET_RAW_IMU_VALUES, PLOT1 , 100 );
            }
        }
        else
        {
            boxesThatNeedsIMU--;

            if( boxesThatNeedsIMU == 0 )
            {
                // Stop IMU
                std::cout << "Stop IMU" << std::endl;
            }
        }
    }
}

void GraphHelper::GetNewIMUMeas( IMU_Measurement _meas)
{
    timeval time;
    gettimeofday( &time, NULL );

    double timestamb = (time.tv_sec * (double)1000) + (time.tv_usec / (double)1000);

    timestamb -= startTimestamp;

    if( boxValues[0] )
    {
        graphPlotter->addPoint(1, QPointF( timestamb, _meas.gyro[0]), true);
    }
    if( boxValues[1] )
    {
        graphPlotter->addPoint(2, QPointF( timestamb, _meas.gyro[1]), true);
    }
    if( boxValues[2] )
    {
        graphPlotter->addPoint(3, QPointF( timestamb, _meas.gyro[2]), true);
    }
    if( boxValues[3] )
    {
        graphPlotter->addPoint(4, QPointF( timestamb, _meas.acc[0]), true);
    }
    if( boxValues[4] )
    {
        graphPlotter->addPoint(5, QPointF( timestamb, _meas.acc[1]), true);
    }
    if( boxValues[5] )
    {
        graphPlotter->addPoint(6, QPointF( timestamb, _meas.acc[2]), true);
    }
    if( boxValues[6] )
    {
        graphPlotter->addPoint(7, QPointF( timestamb, _meas.mag[0]), true);
    }
    if( boxValues[7] )
    {
        graphPlotter->addPoint(8, QPointF( timestamb, _meas.mag[1]), true);
    }
    if( boxValues[8] )
    {
        graphPlotter->addPoint(9, QPointF( timestamb, _meas.mag[2]), true);
    }

}

