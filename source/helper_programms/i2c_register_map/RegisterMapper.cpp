#include "RegisterMapper.hpp"

RegisterMapper::RegisterMapper()
{
}

RegisterMapper *RegisterMapper::instance = NULL;

RegisterMapper* RegisterMapper::getInstance()
{
    if( !instance )
    {
        instance = new RegisterMapper();
    }

    return instance;
}


void RegisterMapper::readRegisters( unsigned int device, unsigned int firstRegister,
                    unsigned int nmbOfRegisters )
{
    // get instance here


    // Read the registers and put it in the list
}

void RegisterMapper::generateTestData( unsigned int firstRegister,
                                       unsigned int nmbOfRegisters )
{
    RegisterMap mapToInsert( firstRegister, nmbOfRegisters );

    srand (time(NULL));

    for( int i = firstRegister; i < (firstRegister+nmbOfRegisters); i++ )
    {
        char randomNumber = rand();

        mapToInsert.addElement(i, randomNumber);
    }


    RegisterMapper::getInstance()->maps.push_back( mapToInsert );
}

int RegisterMapper::getFirstRegister()
{
    unsigned int min = 0xffffffff;

    for( auto iter = RegisterMapper::getInstance()->maps.begin();
         iter != RegisterMapper::getInstance()->maps.end();
         iter++ )
    {
        RegisterMap actMap = *iter;

        if( min > actMap.getFirstAdress() )
        {
            min = actMap.getFirstAdress();
        }
    }

    if( min == 0xffffffff ) return -1;

    return min;
}

int RegisterMapper::getLastRegister()
{
    int max = -1;

    for( auto iter = RegisterMapper::getInstance()->maps.begin();
         iter != RegisterMapper::getInstance()->maps.end();
         iter++ )
    {
        RegisterMap actMap = *iter;

        if( max < (int) actMap.getLastAdress() )
        {
            max = actMap.getLastAdress();
        }
    }

    return max;
}

void RegisterMapper::clearAll()
{
    RegisterMapper::getInstance()->maps.erase( RegisterMapper::getInstance()->maps.begin(),
                                               RegisterMapper::getInstance()->maps.end() );
}

int RegisterMapper::NmbOfMaps()
{
   return RegisterMapper::getInstance()->maps.size();
}

char RegisterMapper::getItem( int row, int column)
{

    if( column >= RegisterMapper::NmbOfMaps() )
    {
        return 0x00;
    }

    RegisterMap act_map = RegisterMapper::getInstance()->maps.at( column );

    if( !act_map.doesRegisterExist( row ) )
    {
        return 0x00;
    }

    return act_map.getContent( row );
}


RegisterMapper::~RegisterMapper()
{
}


