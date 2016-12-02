#ifndef CONNECTION_LIBRARY_HPP
#define CONNECTION_LIBRARY_HPP

#include <iostream>
#include <string>
#include <sstream>

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
    MessageHeadder( int16_t type, int16_t content_size, int16_t _id )
    { data[0] = type; data[1] = content_size; data[2] = _id;}

    int getLength() { return 3 * sizeof(int16_t); }
    char* getData() { return  (char*) data; }

private:
    int16_t data[3];
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

    std::string getHexString();

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
