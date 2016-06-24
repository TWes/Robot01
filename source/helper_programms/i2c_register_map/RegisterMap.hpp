#ifndef REGISTERMAP_HPP
#define REGISTERMAP_HPP

#include "map"

class RegisterMap
{
public:
    RegisterMap();
    RegisterMap(unsigned int firstRegister, unsigned int size);

    ~RegisterMap();

    void addElement( unsigned int register_address, char content );

    unsigned int getFirstAdress();
    unsigned int getLastAdress();
    char getContent(unsigned int adress );

    bool doesRegisterExist( unsigned int register_address);

private:
    unsigned int first_register;
    unsigned int size;

    std::map<unsigned int, char> registers;
};



#endif // REGISTERMAP_H
