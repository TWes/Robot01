#include "GPIO_Interface.hpp"

GPIO_Interface *GPIO_Interface::instance = 0;

GPIO_Interface::GPIO_Interface()
{
	//std::cout << "Construkter of Singelton" << std::endl;

	// Open the Export Files
	this->openExportFiles();
}

void GPIO_Interface::openExportFiles()
{
	//std::cout << "openExportFiles" << std::endl;

	this->exportstream[0] = std::make_shared<std::fstream>();

	this->exportstream[0]->open( "/sys/class/gpio/export", 
								std::fstream::out | std::fstream::app );

	this->exportstream[1] = std::make_shared<std::fstream>();
	this->exportstream[1]->open( "/sys/class/gpio/unexport", 
								std::fstream::out | std::fstream::app );
}

void GPIO_Interface::closeExportFiles()
{
	this->exportstream[0]->close();

	this->exportstream[1]->close();
}


GPIO_Interface::~GPIO_Interface()
{
	//std::cout << "Dekonstrukter of Singelton" << std::endl;

	for( int i = 0 ;i < MAX_PINS; i++)
	{
		std::shared_ptr<std::fstream> workstream =  \
				GPIO_Interface::getInstance()->pinstream[ i ];
		
		// Lösche alle Verbindungen
		if( workstream != nullptr )
		{
			std::cout << "deleting: " << (i+1) << std::endl;			

			*this->exportstream[1] << (i+1) << std::endl;
					
			workstream = nullptr;
		}
	}

	this->closeExportFiles();
}

GPIO_Interface* GPIO_Interface::getInstance()
{
	if( GPIO_Interface::instance == 0 )
		GPIO_Interface::instance = new GPIO_Interface();

	return GPIO_Interface::instance;
}


int GPIO_Interface::setPinHigh(int pinnr)
{
	if( pinnr < 0 || pinnr > MAX_PINS )
		return -1;

    //std::cout << "SetPinHigh " << pinnr << std::endl;

	// Look, is pin is initialised as out
	std::shared_ptr<std::fstream> workstream =  \
				GPIO_Interface::getInstance()->pinstream[ pinnr-1 ];
	
	if( workstream == nullptr )
	{
		GPIO_Interface::getInstance()->InitialisePin( pinnr, GPIO_MODE_OUTPUT);
	}

	*(GPIO_Interface::getInstance()->pinstream[ pinnr-1 ]) << "1" << std::endl;

	return 0;
}

int GPIO_Interface::setPinLow(int pinnr)
{
	if( pinnr < 0 || pinnr > MAX_PINS )
		return -1;

    //std::cout << "SetPinLow " << pinnr << std::endl;

	// Look, is pin is initialised as out
	std::shared_ptr<std::fstream> workstream =  \
				GPIO_Interface::getInstance()->pinstream[ pinnr-1 ];
	
	if( workstream == nullptr )
	{
		std::cout << pinnr << " noch nicht initialisiert" << std::endl;
		GPIO_Interface::getInstance()->InitialisePin( pinnr, GPIO_MODE_OUTPUT);
	}

	*(GPIO_Interface::getInstance()->pinstream[ pinnr-1 ]) << "0" << std::endl;

	return 0;
}

void GPIO_Interface::deleteInstance()
{
	std::cout << "Delete instance of GPIO_Interface" << std::endl;

	if( GPIO_Interface::instance == 0 )
		return;

	delete GPIO_Interface::getInstance();
	
	GPIO_Interface::instance = 0;
}

void GPIO_Interface::InitialisePin(int pin, int mode)
{
    std::cout << "Initialise Pin " << pin << std::endl;

	// Exportiere Pin
	*this->exportstream[0] << pin << std::endl;

	// Set pin as output	
	std::stringstream ss;
	ss << "/sys/class/gpio/gpio" << pin;

	std::fstream mode_file;
	mode_file.open( ss.str() + "/direction", 
					std::fstream::out | std::fstream::app );
	
	switch( mode )
	{
	case GPIO_MODE_OUTPUT:
		mode_file << "out" << std::endl;
		break;

	case GPIO_MODE_INPUT:
		mode_file << "in" << std::endl;
		break;

	default:
		break;
	}	

	mode_file.close();

	// create a file for the value stream
	if( this->pinstream[ pin-1 ] != nullptr )
		std::cout << "Pinstream to create already exists." << std::endl;
	
	this->pinstream[ pin-1 ] = std::make_shared<std::fstream>();

	this->pinstream[ pin-1 ]->open( ss.str() + "/value", 
								std::fstream::out | std::fstream::app );

	return;
}





