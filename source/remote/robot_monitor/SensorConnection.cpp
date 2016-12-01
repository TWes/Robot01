#include "SensorConnection.hpp"

SensorConnection *SensorConnection::instance = 0;


/**
 * @brief SensorConnection::SensorConnection Standard Constructor
 */
SensorConnection::SensorConnection() : udp::Socket(), tcp::Socket()
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
int SensorConnection::init_UDP_Var(get_variable_enume_t _to_subscribe, std::function<void(char*,int)> _action, int sending_interval, int &id)
{

    std::cout << "Init UDP: " << _to_subscribe << std::endl;

    // Check if udp socketis active
     if( !this->udp::Socket::isSocketActive() )
    {
        udp::connection_information_t socket_info;
        udp::Socket::createSocket( socket_info );
        udp_socket_information = socket_info;

        udp::Socket::start_reveiving();
     }

    request_entry_t new_entry;

    new_entry.id = this->actID++;
    new_entry.repeated = true;
    new_entry.action = _action;
    new_entry.timestamp = std::chrono::steady_clock::now();
    new_entry.time_lo_live = std::chrono::seconds(1);


    MessageBuilder message;
    message     << MessageHeadder( SUBSCRIBE_UDP, new_entry.id )
                << (uint32_t) _to_subscribe
                << (uint32_t) udp_socket_information.port_nr
                << (uint32_t) sending_interval;

    int ret = tcp::Socket::sendData( message.getData(), message.getLength() );

    if( ret < 0 )
          return -1;

    openRequestMutex.lock();
        this->openRequests.push_back( new_entry );
    openRequestMutex.unlock();

    return 0;

    /*
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
    std::cout << "Unsubscribe UDP id: " << id << std::endl;
    return -1;
}
