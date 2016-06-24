#include "ControlWidget.hpp"


ControlWidget::ControlWidget(QWidget *parent) : QWidget( parent )
{
    this->deviceAddressInput = new QLineEdit( "device Address", this );
    this->firstRegisterInput = new QLineEdit( "first Register", this );
    this->sizeInput = new QLineEdit( "register count", this );

    this->readButton = new QPushButton( "read", this );
    this->clearButton = new QPushButton ( "clear", this );

    this->layout = new QGridLayout( this );
    this->layout->addWidget( this->deviceAddressInput, 0, 0, 1, 1);
    this->layout->addWidget( this->firstRegisterInput, 0, 1, 1, 1);
    this->layout->addWidget( this->sizeInput, 0, 2, 1, 1);
    this->layout->addWidget( this->readButton, 1, 0, 1, 1);
    this->layout->addWidget( this->clearButton, 1, 1, 1, 1);

    this->setLayout( this->layout );

    QObject::connect( this->clearButton, SIGNAL(clicked()),
                      this, SIGNAL(clear()) );

    QObject::connect( this->readButton, SIGNAL(clicked()),
                       this, SLOT( readRegisters() ) );

}

void ControlWidget::readRegisters()
{
    // check the input of the LineEdits
    QMessageBox errorBox(QMessageBox::Critical, "Wrong Input Format", "" );
    //errorBox.set

    int registerAdddress = this->stringToUInt( this->deviceAddressInput->text() );
    if( registerAdddress < 0 )
    {
        errorBox.setText( "Register address has wrong format.");
        errorBox.exec();
        return;
    }

    int startRegister = this->stringToUInt( this->firstRegisterInput->text() );
    if( startRegister < 0 )
    {
        errorBox.setText( "Start register has wrong format.");
        errorBox.exec();
        return;
    }

    int registersToRead = this->stringToUInt( this->sizeInput->text() );
    if( registersToRead < 0 )
    {
        errorBox.setText( "Register size has wrong format.");
        errorBox.exec();
        return;
    }

    // Read Registers
    RegisterMapper::readRegisters(registerAdddress, startRegister, registersToRead );

    RegisterMapper::generateTestData( startRegister, registersToRead );


    // emit Redraw
    emit redrawTable();

}

/**
 * @brief ControlWidget::stringToUInt converts a QString
 * containing a hex number or a 4 digit decimal into a unsigned int.
 * @param input The QString to be converted.
 * @return -1 on mistake, >= 0 on success
 */
int ControlWidget::stringToUInt(QString input)
{
    int ret = -1;

    QRegExp regularNumber( "^[0-9]{1,4}$");
    QRegExp regularHexNumper("^0[xX]([0-9]|[a-f]|[A-F]){1,2}$");

    // Check if the string has correct form
    if( input.contains(regularNumber) )
    {
        bool success;

        ret = input.toUInt( &success, 10 );

        if( !success ) ret = -1;
    }
    else if( input.contains( regularHexNumper) )
    {
        bool success;

        QString chopedString = input.remove(0 , 2);

        ret =  chopedString.toUInt( &success, 16 );

        if( !success ) ret = -1;
    }

    return ret;
}

ControlWidget::~ControlWidget()
{
    delete this->clearButton;
    delete this->readButton;
    delete this->deviceAddressInput;
    delete this->firstRegisterInput;
    delete this->sizeInput;

    delete this->layout;
}
