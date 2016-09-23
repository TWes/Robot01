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
    checkBoxes[9] = new QCheckBox("AccLinVelX", this);
    checkBoxes[10] = new QCheckBox("AccLinVelY", this);
    checkBoxes[11] = new QCheckBox("AccLinVelZ", this);
    checkBoxes[12] = new QCheckBox("GyroAngVelX", this);
    checkBoxes[13] = new QCheckBox("GyroAngVelY", this);
    checkBoxes[14] = new QCheckBox("GyroAngVelZ", this);

    checkBoxes[15] = new QCheckBox("BattLow", this);
    checkBoxes[16] = new QCheckBox("BattHigh", this);
    checkBoxes[17] = new QCheckBox("magnAngVelZ", this);

    checkBoxes[18] = new QCheckBox("predicdedAngVelZ", this);
    checkBoxes[19] = new QCheckBox("predictedLineVelX", this);
    checkBoxes[20] = new QCheckBox("predictedLineVelY", this);

    for( int i = 0; i < nmbOfBoxes; i++ )
    {
        layout->addWidget( checkBoxes[i], i % 6, (int) i/6 );

        QObject::connect( checkBoxes[i], SIGNAL(toggled(bool)),
                          this, SLOT(CheckBoxesChanged()));
    }

    this->setLayout( layout );

    scrollOnNewValue = new QCheckBox("Scoll on new Value");
    scrollOnNewValue->setChecked(true);
    layout->addWidget( scrollOnNewValue, 7, 0 );

    plotter->addGraph(1, Qt::red, Point, true); // 1
    plotter->addGraph(1, Qt::blue, Point, true); // 2
    plotter->addGraph(1, Qt::green, Point, true); // 3
    plotter->addGraph(1, Qt::gray, Point, true); // 4
    plotter->addGraph(1, Qt::darkCyan, Point, true); // 5
    plotter->addGraph(1, Qt::darkMagenta, Point, true); // 6
    plotter->addGraph(1, Qt::darkGray, Point, true); // 7
    plotter->addGraph(1, Qt::darkRed, Point, true); // 8
    plotter->addGraph(1, Qt::black, Point, true); // 9
    plotter->addGraph(1, Qt::lightGray, Point, true); // 10
    plotter->addGraph(1, Qt::darkBlue, Point, true); // 11
    plotter->addGraph(1, Qt::darkGreen, Point, true); // 12
    plotter->addGraph(1, Qt::darkYellow, Point, true); // 13
    plotter->addGraph(1, Qt::cyan, Point, true); // 14
    plotter->addGraph(1, Qt::yellow, Point, true); // 15
    plotter->addGraph(1, Qt::red, Point, true); // 16
    plotter->addGraph(1, Qt::blue, Point, true); // 17
    plotter->addGraph(1, Qt::green, Point, true); // 18
    plotter->addGraph(1, Qt::gray, Point, true); // 19
    plotter->addGraph(1, Qt::darkCyan, Point, true); // 20
    plotter->addGraph(1, Qt::darkMagenta, Point, true); // 21

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

    if( scrollOnNewValue != NULL )
    {
        delete scrollOnNewValue;
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
                sensor_connection->init_UDP_Var( GET_RAW_IMU_VALUES, PLOT1_IMU , 100 );
            }
        }
        else
        {
            boxesThatNeedsIMU--;

            if( boxesThatNeedsIMU == 0 )
            {
                // Stop IMU
                sensor_connection->unsubscribe_UDP( GET_RAW_IMU_VALUES );
            }
        }
    }

    // We need to deal with the filtered
    else if( changedBox >= 9 && changedBox < 21 )
    {
        if( newState )
        {
            boxesThatNeedsFilteredValues++;

            if( boxesThatNeedsFilteredValues == 1 )
            {
                // Start IMU
                std::cout << "Start filtered values" << std::endl;
                sensor_connection->init_UDP_Var( GET_FILTERED_IMU_VALUES, PLOT1_FILTERED_VALUES , 100 );
            }
        }
        else
        {
            boxesThatNeedsFilteredValues--;

            if( boxesThatNeedsFilteredValues == 0 )
            {
                // Stop IMU
                sensor_connection->unsubscribe_UDP( GET_FILTERED_IMU_VALUES );
            }
        }
    }
}

void GraphHelper::GetNewIMUMeas( IMU_Measurement _meas)
{
    bool renewOnValue = (scrollOnNewValue->checkState() >= 1);

    timeval time;
    gettimeofday( &time, NULL );

    double timestamb = (time.tv_sec * (double)1000) + (time.tv_usec / (double)1000);

    timestamb -= startTimestamp;

    if( boxValues[0] )
    {
        graphPlotter->addPoint(1, QPointF( timestamb, _meas.gyro[0]), renewOnValue);
    }
    if( boxValues[1] )
    {
        graphPlotter->addPoint(2, QPointF( timestamb, _meas.gyro[1]), renewOnValue);
    }
    if( boxValues[2] )
    {
        graphPlotter->addPoint(3, QPointF( timestamb, _meas.gyro[2]), renewOnValue);
    }
    if( boxValues[3] )
    {
        graphPlotter->addPoint(4, QPointF( timestamb, _meas.acc[0]), renewOnValue);
    }
    if( boxValues[4] )
    {
        graphPlotter->addPoint(5, QPointF( timestamb, _meas.acc[1]), renewOnValue);
    }
    if( boxValues[5] )
    {
        graphPlotter->addPoint(6, QPointF( timestamb, _meas.acc[2]), renewOnValue);
    }
    if( boxValues[6] )
    {
        graphPlotter->addPoint(7, QPointF( timestamb, _meas.mag.x_val), renewOnValue);
    }
    if( boxValues[7] )
    {
        graphPlotter->addPoint(8, QPointF( timestamb, _meas.mag.y_val ), renewOnValue);
    }
    if( boxValues[8] )
    {
        graphPlotter->addPoint(9, QPointF( timestamb, _meas.mag.z_val ), renewOnValue);
    }

}

void GraphHelper::GetNewFilteredMeas(  Status_tuple_t _meas)
{
    bool renewOnValue = (scrollOnNewValue->checkState() >= 1);

    timeval time;
    gettimeofday( &time, NULL );

    double timestamb = (time.tv_sec * (double)1000) + (time.tv_usec / (double)1000);

    timestamb -= startTimestamp;

    if( boxValues[9] )
    {
        graphPlotter->addPoint(10, QPointF( timestamb, _meas.accLineVel[0]), renewOnValue);
    }
    if( boxValues[10] )
    {
        graphPlotter->addPoint(11, QPointF( timestamb, _meas.accLineVel[1]), renewOnValue);
    }
    if( boxValues[11] )
    {
        graphPlotter->addPoint(12, QPointF( timestamb, _meas.accLineVel[2]), renewOnValue);
    }
    if( boxValues[12] )
    {
        graphPlotter->addPoint(13, QPointF( timestamb, _meas.gyroAngVel[0]), renewOnValue);
    }
    if( boxValues[13] )
    {
        graphPlotter->addPoint(14, QPointF( timestamb, _meas.gyroAngVel[1]), renewOnValue);
    }
    if( boxValues[14] )
    {
        graphPlotter->addPoint(15, QPointF( timestamb, _meas.gyroAngVel[2]), renewOnValue);
    }
    if( boxValues[15] )
    {
        graphPlotter->addPoint(16, QPointF( timestamb, _meas.BatteryLow), renewOnValue);
    }
    if( boxValues[16] )
    {
        graphPlotter->addPoint(17, QPointF( timestamb, _meas.BatteryHigh), renewOnValue);
    }
    if( boxValues[17] )
    {
        graphPlotter->addPoint(18, QPointF( timestamb, _meas.magnAngVelZ), renewOnValue);
    }
    if( boxValues[18] )
    {
        graphPlotter->addPoint(19, QPointF( timestamb, _meas.predicdedAngVelZ), renewOnValue);
    }
    if( boxValues[19] )
    {
        graphPlotter->addPoint(20, QPointF( timestamb, _meas.predictedLineVelX), renewOnValue);
    }
    if( boxValues[20] )
    {
        graphPlotter->addPoint(21, QPointF( timestamb, _meas.predictedLineVelY), renewOnValue);
    }
}
