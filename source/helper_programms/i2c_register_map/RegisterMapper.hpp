#ifndef REGISTERMAPPER_HPP
#define REGISTERMAPPER_HPP

#include <iostream>
#include <vector>

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

// used for random number generation
#include <stdlib.h>
#include <time.h>

#include <qt4/Qt/QtCore>
#include "RegisterMap.hpp"

#include <fstream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>

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
    static char compareRegister( unsigned int register_address, int col1, int col2 );

    static void saveMap( QString filename);
    static void openMap( QString filename);

private:
    static RegisterMapper *instance;
    static RegisterMapper *getInstance();

    std::vector<RegisterMap> maps;


};


#endif // REGISTERMAPPER_HPP
