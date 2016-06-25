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
    // During Debug
    //RegisterMapper::generateTestData(firstRegister, nmbOfRegisters );
    //return;


    // open device
    int fh = open( "/dev/i2c-1", O_RDWR);
    if( fh < 0 )
    {
        std::cerr << "Fehler beim öffnen des i2c Devices" << std::endl;
        std::cerr << "Fehlercode: " << errno << std::endl;

        close( fh );
        return;
    }

    if (ioctl( fh, I2C_SLAVE, device) < 0)
    {
        std::cerr << "Konnt Addresse " << (int) device
            << " nicht öffnen. Fehler: " << errno
            << ": " << strerror(errno) << std::endl;

        close( fh );
        return;
    }

    char register_to_read_from = firstRegister;
    if( write( fh, &register_to_read_from, 1) != 1 )
    {
        std::cerr << "i2c_access: Konnte Zielregister nicht schreiben: " << errno
            << ": " << strerror(errno) << std::endl;

        close( fh );
        return;
    }

    char* buffer = new char[nmbOfRegisters];
    if( read( fh, buffer, nmbOfRegisters) != nmbOfRegisters )
    {
        std::cout << "i2c_access: Konnte nicht lesen: " << errno
                << ": " << strerror(errno) << std::endl;

        delete buffer;
        close( fh );
        return;
    }

    // Read the registers and put it in the list
    RegisterMap MapToInsert( firstRegister, nmbOfRegisters );

    for( int i = 0; i< nmbOfRegisters; i++ )
    {
        unsigned int register_to_add;
        register_to_add = firstRegister + i;

        MapToInsert.addElement( register_to_add, buffer[i] );
    }

    RegisterMapper::getInstance()->maps.push_back( MapToInsert );

    delete buffer;
    close( fh );
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

void RegisterMapper::saveMap( QString filename)
{
    if( !filename.endsWith(".rm") )
    {
        filename += ".rm";
    }

    std::ofstream archive_to_save( filename.toStdString() );
    boost::archive::text_oarchive  archive(archive_to_save);

    archive << RegisterMapper::getInstance()->maps ;

    archive_to_save.close();
}

void RegisterMapper::openMap( QString filename )
{
    std::ifstream archive_to_read( filename.toStdString() );
    boost::archive::text_iarchive archive(archive_to_read);

    archive >> RegisterMapper::getInstance()->maps ;

    archive_to_read.close();
}

void RegisterMapper::saveToCVS( QString filename )
{
    std::cout << "Write csv file: " << filename.toStdString() << std::endl;
    if( !filename.endsWith(".cvs") )
    {
        filename += ".cvs";
    }

    std::ofstream filestream( filename.toStdString() );

    // Make headder
    int maps = RegisterMapper::NmbOfMaps();
    int columns = 2*maps -1;

    int firstRegister = RegisterMapper::getFirstRegister();
    int lastRegister = RegisterMapper::getLastRegister();


    filestream << "\"\",Content";

    for( int i = 1; i < maps; i++)
    {
        filestream << ",Diff";
        filestream << ",Content";
    }
    filestream << "\n";

    // Fill in the values
    for( int r = firstRegister; r <= lastRegister; r++ )
    {
         QString registerName;
         registerName.setNum( (uchar) r, 16);

         if( registerName.size() == 1 )
         {
             registerName = "0" + registerName;
         }

         registerName = "0x" + registerName;

         filestream << registerName.toStdString();


        for( int c = 0; c < columns; c++)
        {
            bool even = !(c % 2);

            if( even )
            {
                char cToInsert = RegisterMapper::getItem(r, c/2);

                QString toInsert;

                toInsert.setNum( (uchar) cToInsert, 16);

                if( toInsert.size() == 1 )
                {
                    toInsert = "0" + toInsert;
                }

                toInsert = ",0x" + toInsert;

                filestream << toInsert.toStdString();
            }
            else
            {
                char cToInsert = RegisterMapper::compareRegister( r, c/2, (c+1)/2);

                QString toInsert;

                toInsert.setNum( (uchar) cToInsert, 2);

                while( toInsert.size() < 8 )
                {
                    toInsert = "0" + toInsert;
                }

                toInsert = ",0b" + toInsert;

                filestream << toInsert.toStdString();
            }
        }

        filestream << "\n";

    }




    filestream.close();
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

char RegisterMapper::compareRegister(unsigned int register_address, int col1, int col2 )
{
    char char1, char2;

    char1 = getItem(register_address, col1 );
    char2 = getItem(register_address, col2 );

    char diff = char1 ^ char2;

    return diff;
}

RegisterMapper::~RegisterMapper()
{
}


