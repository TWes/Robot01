#include "RegisterMap.hpp"

RegisterMap::RegisterMap()
{

}

RegisterMap::RegisterMap(unsigned int firstRegister, unsigned int size)
{
    this->first_register = firstRegister;
    this->size = size;
}

void RegisterMap::addElement(unsigned int register_address, char content)
{
    this->registers[register_address] = content;
}

unsigned int RegisterMap::getFirstAdress()
{
    return this->first_register;
}

unsigned int RegisterMap::getLastAdress()
{
    int ret = (this->first_register + this->size - 1);

    if( ret < 0 ) ret = 0;

    return ret;
}

char RegisterMap::getContent( unsigned int adress )
{
    return this->registers[adress];
}

bool RegisterMap::doesRegisterExist( unsigned int register_address)
{
    return (this->registers.find( register_address ) != this->registers.end() );
}



RegisterMap::~RegisterMap()
{

}
