#include "GraphContainer.hpp"


GraphContainer::GraphContainer(QWidget *parent) : QWidget( parent )
{
    QVBoxLayout *layout = new QVBoxLayout();

    comboBox = createComboBox();
    initialicedItemsChecked();
    QObject::connect( comboBox, SIGNAL(activated(int)),
                      this, SLOT(comboBoxItemChanged(int)) );

    layout->addWidget( comboBox );


    this->plotter = new GraphPlotter(this);
    layout->addWidget( this->plotter);
    createGraphs();


    setLayout( layout );

    programm_start = std::chrono::system_clock::now();

    FileWriter::getInstance()->createFile("magnValues");

}

QComboBox* GraphContainer::createComboBox()
{
    QComboBox *tmp = new QComboBox(this);
    tmp->addItem(QString::fromUtf8("\u2610")+ " GyroRawX");
    tmp->addItem(QString::fromUtf8("\u2610")+ " GyroRawY");
    tmp->addItem(QString::fromUtf8("\u2610")+ " GyroRawZ");

    this->itemService["GyroRawX"] = GET_RAW_IMU_VALUES;
    this->itemService["GyroRawY"] = GET_RAW_IMU_VALUES;
    this->itemService["GyroRawZ"] = GET_RAW_IMU_VALUES;

    tmp->addItem(QString::fromUtf8("\u2610")+ " AccRawX");
    tmp->addItem(QString::fromUtf8("\u2610")+ " AccRawY");
    tmp->addItem(QString::fromUtf8("\u2610")+ " AccRawZ");

    this->itemService["AccRawX"] = GET_RAW_IMU_VALUES;
    this->itemService["AccRawY"] = GET_RAW_IMU_VALUES;
    this->itemService["AccRawZ"] = GET_RAW_IMU_VALUES;

    tmp->addItem(QString::fromUtf8("\u2610")+ " MagnRawX");
    tmp->addItem(QString::fromUtf8("\u2610")+ " MagnRawY");
    tmp->addItem(QString::fromUtf8("\u2610")+ " MagnRawZ");

    this->itemService["MagnRawX"] = GET_RAW_IMU_VALUES;
    this->itemService["MagnRawY"] = GET_RAW_IMU_VALUES;
    this->itemService["MagnRawZ"] = GET_RAW_IMU_VALUES;

    tmp->addItem(QString::fromUtf8("\u2610")+ " AccLinVelX");
    tmp->addItem(QString::fromUtf8("\u2610")+ " AccLinVelY");
    tmp->addItem(QString::fromUtf8("\u2610")+ " AccLinVelZ");

    this->itemService["AccLinVelX"] = GET_FILTERED_IMU_VALUES;
    this->itemService["AccLinVelY"] = GET_FILTERED_IMU_VALUES;
    this->itemService["AccLinVelZ"] = GET_FILTERED_IMU_VALUES;

    tmp->addItem(QString::fromUtf8("\u2610")+ " GyroAngVelX");
    tmp->addItem(QString::fromUtf8("\u2610")+ " GyroAngVelY");
    tmp->addItem(QString::fromUtf8("\u2610")+ " GyroAngVelZ");

    this->itemService["GyroAngVelX"] = GET_FILTERED_IMU_VALUES;
    this->itemService["GyroAngVelY"] = GET_FILTERED_IMU_VALUES;
    this->itemService["GyroAngVelZ"] = GET_FILTERED_IMU_VALUES;

    tmp->addItem(QString::fromUtf8("\u2610")+ " BattLow");
    tmp->addItem(QString::fromUtf8("\u2610")+ " BattHigh");
    tmp->addItem(QString::fromUtf8("\u2610")+ " magnAngVelZ");

    this->itemService["BattLow"] = GET_FILTERED_IMU_VALUES;
    this->itemService["BattHigh"] = GET_FILTERED_IMU_VALUES;
    this->itemService["magnAngVelZ"] = GET_FILTERED_IMU_VALUES;

    tmp->addItem(QString::fromUtf8("\u2610")+ " predicdedAngVelZ");
    tmp->addItem(QString::fromUtf8("\u2610")+ " predictedLineVelX");
    tmp->addItem(QString::fromUtf8("\u2610")+ " predictedLineVelY");

    this->itemService["predicdedAngVelZ"] = GET_FILTERED_IMU_VALUES;
    this->itemService["predictedLineVelX"] = GET_FILTERED_IMU_VALUES;
    this->itemService["predictedLineVelY"] = GET_FILTERED_IMU_VALUES;

    this->functionService[GET_FILTERED_IMU_VALUES] =
            [this](char* addr, int len)-> void
    {
        if( sizeof( Status_tuple_t) != len )
        {
            std::cout << "Received message for status tuple does not match correct size." << std::endl;
            return;
        }

        Status_tuple_t status;
        memcpy( &status, addr, len);

        this->getStatusValues(status);
    };
    this->functionService[GET_RAW_IMU_VALUES] = [this](char* addr, int len)-> void
    {
        if( sizeof( IMU_Measurement) != len )
        {
            std::cout << "Get Raw Values: " << len << ";" << sizeof(IMU_Measurement) << std::endl;
        }

        IMU_Measurement measurement;
        memcpy( &measurement, addr, len );

        this->getRawIMUValues(measurement);
    };

    subscriptionCounter[GET_RAW_IMU_VALUES] = 0;
    subscriptionCounter[GET_FILTERED_IMU_VALUES] = 0;

    return tmp;
}

void GraphContainer::comboBoxItemChanged(int changedIndex )
{
    QString changed = this->comboBox->itemText( changedIndex );

    // Erase first elemetns
    QString chopedQString = changed.right( changed.size() -2 );
    std::string chopedString = chopedQString.toStdString() ;

    toogleItemsChecked( chopedString );

    // Sets the new index
    QString newLabel = (this->itemsChecked[chopedString]? QString::fromUtf8("\u2611") : QString::fromUtf8("\u2610") );
    newLabel += " " + chopedQString;

    comboBox->setItemText(changedIndex, newLabel);
}

void GraphContainer::initialicedItemsChecked()
{
    if( comboBox == NULL ) return;

    for( int i = 0; i < comboBox->count(); i++ )
    {
        QString changed = this->comboBox->itemText( i );

        // Erase first elemetns
        QString chopedQString = changed.right( changed.size() -2 );
        std::string chopedString = chopedQString.toStdString() ;

        this->itemsChecked[chopedString] = false;
    }
}

void GraphContainer::toogleItemsChecked(std::string toToggle)
{
    this->itemsChecked[toToggle] = !this->itemsChecked[toToggle];

    get_variable_enume_t service = itemService[toToggle];

    // Is now true
    if(this->itemsChecked[toToggle] )
    {
        subscriptionCounter[service] += 1;

        if( subscriptionCounter[service] == 1 )
        {
            int id = -1;
            //std::cout << "Subscribe UDP: " << service << std::endl;
            SensorConnection::getInstance()->init_UDP_Var( service, functionService[service], 1000, id );
            subscriptionId[service] = id;
            //std::cout << "Subscribe: " << service << "; id: " << id << std::endl;
        }
    }
    else
    {
        subscriptionCounter[service] -= 1;

        if( subscriptionCounter[service] <= 0 )
        {
            //std::cout << "Unsubscribe UDP: " << service << std::endl;
            SensorConnection::getInstance()->unsubscribe_UDP( subscriptionId[service] );
        }
    }
}

void GraphContainer::createGraphs()
{
    plotter->addGraph(1, Qt::red, PointTypeEnum::Point, true);
    graphMap["GyroRawX"] = 1;
    plotter->addGraph(1, Qt::red, PointTypeEnum::Point, true);
    graphMap["GyroRawY"] = 2;
    plotter->addGraph(1, Qt::red, PointTypeEnum::Point, true);
    graphMap["GyroRawZ"] = 3;

    plotter->addGraph(1, Qt::red, PointTypeEnum::Point, true);
    graphMap["AccRawX"] = 4;
    plotter->addGraph(1, Qt::red, PointTypeEnum::Point, true);
    graphMap["AccRawY"] = 5;
    plotter->addGraph(1, Qt::red, PointTypeEnum::Point, true);
    graphMap["AccRawZ"] = 6;

    plotter->addGraph(1, Qt::red, PointTypeEnum::Point, true);
    graphMap["MagnRawX"] = 7;
    plotter->addGraph(1, Qt::red, PointTypeEnum::Point, true);
    graphMap["MagnRawY"] = 8;
    plotter->addGraph(1, Qt::red, PointTypeEnum::Point, true);
    graphMap["MagnRawZ"] = 9;

    plotter->addGraph(1, Qt::blue, PointTypeEnum::Point, true);
    graphMap["AccLinVelX"] = 10;
    plotter->addGraph(1, Qt::blue, PointTypeEnum::Point, true);
    graphMap["AccLinVelY"] = 11;
    plotter->addGraph(1, Qt::blue, PointTypeEnum::Point, true);
    graphMap["AccLinVelZ"] = 12;

    plotter->addGraph(1, Qt::blue, PointTypeEnum::Point, true);
    graphMap["GyroAngVelX"] = 13;
    plotter->addGraph(1, Qt::blue, PointTypeEnum::Point, true);
    graphMap["GyroAngVelY"] = 14;
    plotter->addGraph(1, Qt::blue, PointTypeEnum::Point, true);
    graphMap["GyroAngVelZ"] = 15;

    plotter->addGraph(1, Qt::blue, PointTypeEnum::Point, true);
    graphMap["BattLow"] = 16;
    plotter->addGraph(1, Qt::blue, PointTypeEnum::Point, true);
    graphMap["BattHigh"] = 17;
    plotter->addGraph(1, Qt::blue, PointTypeEnum::Point, true);
    graphMap["magnAngVelZ"] = 18;

    plotter->addGraph(1, Qt::blue, PointTypeEnum::Point, true);
    graphMap["predicdedAngVelZ"] = 19;
    plotter->addGraph(1, Qt::blue, PointTypeEnum::Point, true);
    graphMap["predictedLineVelX"] = 20;
    plotter->addGraph(1, Qt::blue, PointTypeEnum::Point, true);
    graphMap["predictedLineVelY"] = 21;
}

void GraphContainer::getRawIMUValues(IMU_Measurement meas )
{
    float yVal = std::chrono::duration_cast<std::chrono::milliseconds>( meas.timestamp - this->programm_start).count();

    // Suche alle für RAW IMU Values relevanten strings, welcha aktiv sind
    for( std::pair<std::string, get_variable_enume_t> iter : itemService)
    {
        if( iter.second == GET_RAW_IMU_VALUES && itemsChecked[iter.first] )
        {
            // Hänge die Punke an die Graphen
            int graphID = graphMap[iter.first];

            // Get Point
            float xVal = getValueFromMeasurement( iter.first, meas );

            QPointF pointToAdd = QPointF(yVal, xVal );
            plotter->addPoint( graphID, pointToAdd, true );
        }
    }

    *FileWriter::getInstance()->getFile("magnValues") << meas.mag.x_val << " "
                                                      << meas.mag.y_val << " "
                                                      << meas.mag.z_val << std::endl;
}

void GraphContainer::getStatusValues( Status_tuple_t status )
{
    //std::cout << "Get Status Values: " << length << "; " << sizeof(Status_tuple_t) << std::endl;
    float yVal = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now() - this->programm_start).count();

    // Suche alle für RAW IMU Values relevanten strings, welcha aktiv sind
    for( std::pair<std::string, get_variable_enume_t> iter : itemService)
    {
        if( iter.second == GET_FILTERED_IMU_VALUES && itemsChecked[iter.first] )
        {
            // Hänge die Punke an die Graphen
            int graphID = graphMap[iter.first];

            // Get Point
            float xVal = getValueFromStatus( iter.first, status );

            QPointF pointToAdd = QPointF(yVal, xVal );
            plotter->addPoint( graphID, pointToAdd, true );
        }
    }
}

float GraphContainer::getValueFromMeasurement( std::string item, IMU_Measurement data )
{
    float value = 0;

    if( item == "GyroRawX" )
    {
        value = data.gyro[0];
    }
    else if( item == "GyroRawY" )
    {
        value = data.gyro[1];
    }
    else if( item == "GyroRawZ" )
    {
        value  = data.gyro[2];
    }
    else if( item == "AccRawX" )
    {
        value = data.acc[0];
    }
    else if( item == "AccRawY" )
    {
        value = data.acc[1];
    }
    else if( item == "AccRawZ" )
    {
        value = data.acc[2];
    }
    else if( item == "MagnRawX" )
    {
        value = data.mag.x_val;
    }
    else if( item == "MagnRawY" )
    {
        value = data.mag.y_val;
    }
    else if( item == "MagnRawZ" )
    {
        value = data.mag.z_val;
    }

    return value;
}

float GraphContainer::getValueFromStatus( std::string item, Status_tuple_t data )
{
    float value = 0.0;

    if( item == "AccLinVelX" )
    {
        value = data.accLineVel[0];
    }
    else if( item == "AccLinVelY")
    {
        value = data.accLineVel[1];
    }
    else if( item == "AccLinVelZ")
    {
        value = data.accLineVel[2];
    }
    else if( item == "GyroAngVelX")
    {
        value = data.gyroAngVel[0];
    }
    else if( item == "GyroAngVelY")
    {
        value = data.gyroAngVel[1];
    }
    else if( item == "GyroAngVelZ")
    {
        value = data.gyroAngVel[2];
    }
    else if( item == "BattLow")
    {
        value = data.BatteryLow;
    }
    else if( item == "BattHigh")
    {
        value = data.BatteryHigh;
    }
    else if( item == "magnAngVelZ")
    {
        value = data.magnAngVelZ;
    }
    else if( item == "predicdedAngVelZ")
    {
        value = data.predicdedAngVelZ;
    }
    else if( item == "predictedLineVelX")
    {
        value = data.predictedLineVelX;
    }
    else if( item == "predictedLineVelY")
    {
        value = data.predictedLineVelY;
    }
    else
    {
        std::cout << "Asked for :\"" << item << "\" which is not known" << std::endl;
    }

    return value;
}

GraphContainer::~GraphContainer()
{
    if( testLabel != NULL )
        delete testLabel;
}
