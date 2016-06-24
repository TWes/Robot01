#ifndef REGISTERMAPPER_HPP
#define REGISTERMAPPER_HPP

#include <iostream>
#include <vector>

// used for random number generation
#include <stdlib.h>
#include <time.h>

#include "RegisterMap.h"

class RegisterMapper
{
public:
    RegisterMapper();
    ~RegisterMapper();

    static void clearAll();
    static int NmbOfMaps();

    static void readRegisters( unsigned int device, unsigned int firstRegister,
                        unsigned int nmbOfRegisters );

    static void generateTestData(unsigned int firstRegister,
                                 unsigned int nmbOfRegisters);

    static int getFirstRegister();
    static int getLastRegister();
    static char getItem( int row, int column);


private:
    static RegisterMapper *instance;
    static RegisterMapper *getInstance();

    std::vector<RegisterMap> maps;

};


#endif // REGISTERMAPPER_HPP
