#include "GraphContainer.hpp"


GraphContainer::GraphContainer(QWidget *parent) : QWidget( parent )
{
    QVBoxLayout *layout = new QVBoxLayout();

    comboBox = createComboBox();
    initialicedItemsChecked();
    QObject::connect( comboBox, SIGNAL(activated(int)),
                      this, SLOT(comboBoxItemChanged(int)) );

    layout->addWidget( comboBox );

    setLayout( layout );
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

    this->functionService[GET_FILTERED_IMU_VALUES] = [this](char* addr, int len)-> void { this->getStatusValues(addr, len);};
    this->functionService[GET_RAW_IMU_VALUES] = [this](char* addr, int len)-> void { this->getRawIMUValues(addr, len);};

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

void GraphContainer::getRawIMUValues(char* message, int length )
{
    std::cout << "Get Raw Values: " << length << ";" << sizeof(IMU_Measurement) << std::endl;
}

void GraphContainer::getStatusValues(char* message, int length )
{
    //std::cout << "Get Status Values: " << length << "; " << sizeof(Status_tuple_t) << std::endl;
}

GraphContainer::~GraphContainer()
{
    if( testLabel != NULL )
        delete testLabel;
}
