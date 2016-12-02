#include "connection_library.hpp"


MessageBuilder::MessageBuilder()
{
    this->message = 0;
    this->len = 0;
}

MessageBuilder::~MessageBuilder()
{
    if( this->message != 0 )
    {
        delete this->message;
    }
}

std::string MessageBuilder::getHexString()
{

    std::stringstream ss;

    for( int i = 0; i < this->len; i++ )
    {
       ss << "[" << "0x" << std::hex << +this->message[i] << "]";
    }

    return ss.str();
}

MessageBuilder& MessageBuilder::operator<<( MessageHeadder data )
{
    int newLength = this->len + data.getLength();
    char* newPointer = new char[ newLength];

    memcpy( newPointer, this->message, this->len );
    memcpy( (newPointer + this->len ), data.getData(), data.getLength() );

    delete this->message;

    this->message = newPointer;
    this->len = newLength;

    return *this;
}
