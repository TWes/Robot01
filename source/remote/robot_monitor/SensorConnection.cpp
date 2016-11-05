#include "SensorConnection.hpp"

SensorConnection *SensorConnection::instance = 0;


/**
 * @brief SensorConnection::SensorConnection Standard Constructor
 */
SensorConnection::SensorConnection()
{}


/**
 * @brief SensorConnection::~SensorConnection Standard Deconstructor
 */
SensorConnection::~SensorConnection()
{}


/**
 * @brief SensorConnection::getInstance
 * @return The Singelton Instance
 */
SensorConnection *SensorConnection::getInstance()
{
    if( SensorConnection::instance == 0 )
    {
        SensorConnection::instance = new SensorConnection();
    }

    return SensorConnection::instance;
}


/**
 * @brief SensorConnection::setupTCPConnection Connects to the given target
 * @param targetIP The IP
 * @param targetPort The port
 * @return Error Value
 */
int SensorConnection::setupTCPConnection( std::string targetIP, int targetPort )
{
    if( !this->tcp::Socket::is_connected )
    {
        return tcp::Socket::start_connection( targetIP, targetPort );
    }

    return 0;
}
