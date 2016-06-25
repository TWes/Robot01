#ifndef REGISTERMAP_HPP
#define REGISTERMAP_HPP

#include "map"
#include "boost/serialization/map.hpp"

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

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & first_register;
        ar & size;
        ar & registers;
    }


private:
    u_int32_t first_register;
    u_int32_t size;

    std::map<u_int32_t, u_char> registers;
};



#endif // REGISTERMAP_H
