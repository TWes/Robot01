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
    message     << MessageHeadder( SUBSCRIBE_UDP, 3*sizeof(uint32_t),new_entry.id )
                << (uint32_t) _to_subscribe
                << (uint32_t) udp_socket_information.port_nr
                << (uint32_t) sending_interval;

   // std::cout << message.getHexString() << std::endl;


    int ret = tcp::Socket::sendData( message.getData(), message.getLength() );

    if( ret < 0 )
          return -1;

    openRequestMutex.lock();
        this->openRequests.push_back( new_entry );
    openRequestMutex.unlock();

    id = new_entry.id;

    return 0;   
}

/**
 * @brief handle_connection
 * @param message
 * @param message_lenght
 * @param other
 */
void SensorConnection::handle_connection( char* message, int message_lenght,
                                udp::connection_information_t other )
{
    //std::cout << "Rec from: " << std::string(inet_ntoa( other.adress )) << std::endl;

    uint16_t headder[3];

     //std::cout << "UDP receive: " << message << std::endl;
     // get Headders
     if( message_lenght >= sizeof(headder) )
     {
        memcpy( headder, message, sizeof(headder) );

        /*static int test = 0;
        std::cout   << test++ << " ---------------------\n"
                    << "h(0): " << headder[0] << "\n"
                       << "h(1): " << headder[1] << "\n"
                       << "h(2): " << headder[2] << std::endl;*/
       }
       else
       { return; }

       std::vector<request_entry_t> entries = this->getEntryById( headder[2] );

        if( entries.size() <= 0 )
        {
            std::cout << "Got UDP message with no entry" << std::endl;
            return;
        }

        ((request_entry_t) entries.at(0)).action( message + sizeof(headder), headder[1] );
}


int SensorConnection::testTCPConnection()
{
    return 0;

    // send something to test conection
    const char* testSring = "Hallo Welt";

    MessageBuilder builder;
    builder << MessageHeadder( LOOPBACK, strlen(testSring) ,this->actID++ )
            << testSring;

     int ret = tcp::Socket::sendData( builder.getData(), builder.getLength()  );

     // SOmething has to be read

     std::cout << "Return from test send: " << ret << std::endl;

}


/**
 * @brief SensorConnection::getEntryById Returns a std::vector with the corresponding entry.
 * @param id The id to search for.
 * @return A std::vector with the list.
 */
std::vector<request_entry_t> SensorConnection::getEntryById( int id )
{
    std::vector<request_entry_t>  entries;

    for( request_entry_t entry : this->openRequests )
    {
        if( entry.id == id )
        {
            entries.push_back( entry );
        }
    }

    return entries;
}

/**
 * @brief SensorConnection::unsubscribe_UDP Unsubscribes a UDP connection
 * @param id The id to delete
 * @return Error Value
 */
int SensorConnection::unsubscribe_UDP(int id)
{
    //std::cout << "Unsubscribe UDP id: " << id << std::endl;
    MessageBuilder message;
    message << MessageHeadder( UNSUBSCRIBE_UDP, 0, id );
    this->tcp::Socket::sendData( message.getData(), message.getLength()  );

    //std::cout << this->openRequests.size() << std::endl;

    // Erase id
    std::remove_if( this->openRequests.begin(), this->openRequests.end(), [id](request_entry_t x){ return x.id == id;});

    //std::cout << this->openRequests.size() << std::endl;

    return 0;
}
