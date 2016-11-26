#include "SensorConnection.hpp"

SensorConnection *SensorConnection::instance = 0;


/**
 * @brief SensorConnection::SensorConnection Standard Constructor
 */
SensorConnection::SensorConnection()
{
    actID = 0;
}


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
int SensorConnection::setupTCPConnection(std::string targetIP, int targetPort , int timeoutMS)
{
    if( !this->tcp::Socket::is_connected )
    {
        return tcp::Socket::start_connection_with_timeout( targetIP, targetPort, timeoutMS );
    }

    return 0;
}


/**
 * @brief SensorConnection::shutdown_connection Disables the connection.
 * @return Error Value
 */
int SensorConnection::shutdownTCPConnection()
{

    if( this->tcp::Socket::is_connected )
    {
        this->tcp::Socket::shutdown_connection();
    }

    return 0;
}

/**
 * @brief SensorConnection::init_UDP_Var Initialized a UDP Subscription
 * @param _to_subscribe The Type to which subscribe
 * @param action The callpack funktion
 * @param sending_interval In which interval should the messages be send
 * @param id Return parameter of the id
 * @return Error Param
 */
int SensorConnection::init_UDP_Var(get_variable_enume_t _to_subscribe, void (*action)(char *, int), int sending_interval, int &id)
{
    // Check if udp socket is active

    /* if( this->udp_socket == NULL )
    {
        udp_connection_information_t socket_info;
        this->udp_socket = new udp_connection( this );

        this->udp_socket->createSocket( 0, socket_info );
        udp_socket_information = socket_info;

        this->udp_socket->start_reveiving();
    }

    uint16_t headder[3];

    request_entry_t new_entry;

    new_entry.id = this->actual_id++;
    new_entry.stream = true;
    gettimeofday( &new_entry.timestamp, NULL );
    new_entry.time_lo_live.tv_sec = 3;
    new_entry.time_lo_live.tv_usec = 0;
    new_entry.todo_action = _todo;

    headder[0] = SUBSCRIBE_UDP;
    headder[1] = 3 * sizeof( uint32_t );
    headder[2] = new_entry.id;

    // receive the pose
    uint32_t data[3];
    data[0] = _to_subscribe;
    data[1] = udp_socket_information.port_nr;
    data[2] = sending_interval; // intervall in ms

    char message[ 3*sizeof(uint16_t) + 3 * sizeof(uint32_t ) ];

    memcpy( message, headder, 3*sizeof(uint16_t) );
    memcpy( (message + 3*sizeof(uint16_t) ), data, 3 * sizeof( uint32_t ) );

    open_requests_mutex.lock();
        this->open_requests.push_back( new_entry );
    open_requests_mutex.unlock();

    int ret = this->sendData( message, sizeof(message) );

    if( ret < 0 )
    {
        // Fehler
        return -1;
    }

    return 0; */
}


/**
 * @brief SensorConnection::unsubscribe_UDP Unsubscribes a UDP connection
 * @param id The id to delete
 * @return Error Value
 */
int SensorConnection::unsubscribe_UDP(int id)
{

}
