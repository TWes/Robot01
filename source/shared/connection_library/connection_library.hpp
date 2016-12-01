#ifndef CONNECTION_LIBRARY_HPP
#define CONNECTION_LIBRARY_HPP

#include <iostream>

#include <stdint.h>
#include <string.h>

class MessageData
{
public:
    //MessageData();
    //~MessageData();

    virtual int getLength(){}
    virtual char* getData() {}
};


class MessageHeadder : public MessageData
{
public:
    MessageHeadder( uint16_t type, uint16_t _id )
    { data[0] = type; data[1] = 3*sizeof(uint16_t); data[3] = _id; }

    int getLength() { return 3 * sizeof(uint16_t); }
    char* getData() { return  (char*) data; }

private:
    uint16_t data[3];
};



class MessageBuilder
{

public:
    MessageBuilder();
    ~MessageBuilder();

    template<typename T>
    MessageBuilder& operator<<( T data );

    MessageBuilder& operator<<( MessageHeadder data );

    int getLength() { return len; }
    char* getData() { return  message; }

private:
    char* message = 0;
    int len = 0;
};


template<typename T>
MessageBuilder& MessageBuilder::operator<<( T data )
{
    //std::cout << "before: " << this->len <<std::endl;

    //std::cout << "Append: " << data << std::endl;

    int newLength = this->len + sizeof(T);
    char* newPointer = new char[ newLength];

    memcpy( newPointer, this->message, this->len );
    memcpy( (newPointer + this->len ), &data, sizeof(T) );

    delete this->message;

    this->message = newPointer;
    this->len = newLength;

    //std::cout << "After: " << this->len <<std::endl;

    return *this;
}




#endif
