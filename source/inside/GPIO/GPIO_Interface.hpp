#ifndef GPIO_INTERFACE_HPP
#define GPIO_INTERFACE_HPP

#include <iostream>
#include <fstream>
#include <sstream>

#include <memory>

#define MAX_PINS 26
#define GPIO_MODE_OUTPUT 1
#define GPIO_MODE_INPUT 0

#define SetDigital(pin, state) {SETDIGITAL( pin, state )}

#define SETDIGITAL( pin, state )  { if( state == 0) \
				GPIO_Interface::setPinLow(pin); \
				else if (state ==1) \
				GPIO_Interface::setPinHigh(pin); }
			


class GPIO_Interface
{
	public:
		static GPIO_Interface* getInstance();

		static int setPinHigh(int pinnr);
		static int setPinLow(int pinnr);

		static void deleteInstance();

		GPIO_Interface();

		~GPIO_Interface();

	private:
		static GPIO_Interface* instance;

		std::shared_ptr<std::fstream> pinstream[ MAX_PINS ];
		std::shared_ptr<std::fstream> exportstream[2];

		void openExportFiles();
		void closeExportFiles();

		void InitialisePin(int pin, int mode);
};	


#endif
