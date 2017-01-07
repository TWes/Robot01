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
{
    this->endPollingThread();
}


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

    udp_request_entry_t new_entry;

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

       std::vector<udp_request_entry_t> entries = this->getEntryById( headder[2] );

        if( entries.size() <= 0 )
        {
            std::cout << "Got UDP message with no entry" << std::endl;
            return;
        }

        ((udp_request_entry_t) entries.at(0)).action( message + sizeof(headder), headder[1] );
}


/**
 * @brief SensorConnection::testTCPConnection
 * @param onError
 * @param onSuccess
 * @return
 */
int SensorConnection::testTCPConnection(std::function<void(void)> onError, std::function<void(void)> onSuccess )
{
    // send something to test conection
    const char* testSring = "Hallo Welt";
    int id = this->actID++;

    MessageBuilder builder;
    builder << MessageHeadder( LOOPBACK, strlen(testSring) , id )
            << testSring;

    try
    {
        tcp::Socket::sendData( builder.getData(), builder.getLength());
    }
    catch( tcp::connectionError &exception )
    {
       tcp::Socket::shutdown_connection();
       throw tcp::connectionError(exception.getErrorNumber());
    }

    this->startPollingThread();

     // SOmething has to be read
    // Push request in the request vector
    tcp_request_entry_t newRequest;
    newRequest.id = id;
    newRequest.repeated = false;
    newRequest.onReceive =
            [onSuccess](char* addr, int len)-> void
            {
                // Todo: Check if the received is valid
                onSuccess();
            };
    newRequest.onTimeout =
            [onError](void)-> void
            {
                onError();
            };
    newRequest.timestamp = std::chrono::system_clock::now();
    newRequest.time_lo_live = std::chrono::milliseconds( 2000 );

    this->openTCPRequests.push_back( newRequest );
}

/**
 * @brief SensorConnection::tcp_polling_function
 * This function runs in athread and polls the incoming streams.
 */
void SensorConnection::tcp_polling_function()
{
    while( this->continueTCPpolling )
    {
        //std::cout << "tcp polling" << std::endl;

        // wait for the socket to be readable
        fd_set clientToWatch;
        FD_SET( this->tcp::Socket::socket_fd, &clientToWatch );

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int ret = select( this->tcp::Socket::socket_fd +1, &clientToWatch, NULL, NULL, &timeout );

        if( ret < 0 )
        {
            std::cout << "Fehler bei select: " << errno << "; " << strerror(errno) << std::endl;
        }
        else if (ret == 0) // Timeout
        {
            //Check for timeouts
            checkTCPTimeouts();
            continue;
        }

        // Read and analyse
        int16_t headder[3] = {0};

        try{
            this->tcp::Socket::readData( (char*) headder, 3*sizeof(int16_t) );
        }
        catch( tcp::connectionError &exception )
        {
            switch( exception.getErrorNumber() )
            {
            case EAGAIN:

                break;
            default:
                std::cout << exception.what() << std::endl;
                break;
            }

            continue;
        }

        int lenght = headder[1];
        char* buffer = new char(lenght);
        this->tcp::Socket::readData( buffer, lenght );

        checkTCPTimeouts();
        checkTCPRequest( headder[2], buffer, lenght );
    }
}

/**
 * @brief SensorConnection::checkTCPTimeouts
 * Checks the openTCPRequests for timeouts ans deletes it
 */
void SensorConnection::checkTCPTimeouts()
{    
    //std::cout << "Before Timeout Test: " << this->openTCPRequests.size() << std::endl;

    if( this->openTCPRequests.size() <= 0 ) return;

    std::chrono::system_clock::time_point actTime =  std::chrono::system_clock::now();

    this->openTCPRequests.erase(
    std::remove_if( this->openTCPRequests.begin(), this->openTCPRequests.end(),
            [actTime](tcp_request_entry_t &element){
                std::chrono::system_clock::time_point timeToDie = element.timestamp + element.time_lo_live;
                std::chrono::milliseconds timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(timeToDie - actTime);

                //std::cout << "Check time diff " << timeDiff.count() << std::endl;
                if( timeDiff.count() < 0 )
                {
                    element.onTimeout();
                    return true;
                }
                return false;
    }), this->openTCPRequests.end() );

    //std::cout << "After Timeout Test: " << this->openTCPRequests.size() << std::endl;

}

/**
 * @brief SensorConnection::checkTCPRequest
 * @param id The id to check
 *
 * THis function checkes for the id and calles the request Callback
 */
void SensorConnection::checkTCPRequest(int id , char *msg, int len)
{
    //std::cout << "Before TCP Request: " << this->openTCPRequests.size() << std::endl;

    if( this->openTCPRequests.size() <= 0 ) return;

    this->openTCPRequests.erase(
    std::remove_if( this->openTCPRequests.begin(), this->openTCPRequests.end(),
            [id, msg, len](tcp_request_entry_t &element){
                bool toDelete = false;
                if( id == element.id)
                {
                    std::cout << "Call on receive" << std::endl;
                    element.onReceive(msg, len);

                    if( !element.repeated )
                        toDelete = true;
                }
                return toDelete;
    }), this->openTCPRequests.end() );

    //std::cout << "After TCP Request: " << this->openTCPRequests.size() << std::endl;

}


/**
 * @brief SensorConnection::startPollingThread
 * This function starts the tcp polling thread.
 */
void SensorConnection::startPollingThread()
{
    // Thread already running
    if( continueTCPpolling &&  this->tcpPollingThread != NULL  )
    {
        return;
    }

    continueTCPpolling = true;

    if( this->tcpPollingThread == NULL )
    {
        this->tcpPollingThread = new std::thread(&SensorConnection::tcp_polling_function, this );
    }
}

/**
 * @brief SensorConnection::endPollingThread
 * This function stops the tcp polling thread.
 */
void SensorConnection::endPollingThread()
{
    if( this->tcpPollingThread != NULL )
    {
        continueTCPpolling = false;
        this->tcpPollingThread->join();
        delete this->tcpPollingThread;
        this->tcpPollingThread = NULL;
    }
}

/**
 * @brief SensorConnection::getEntryById Returns a std::vector with the corresponding entry.
 * @param id The id to search for.
 * @return A std::vector with the list.
 */
std::vector<udp_request_entry_t> SensorConnection::getEntryById( int id )
{
    std::vector<udp_request_entry_t>  entries;

    for( udp_request_entry_t entry : this->openRequests )
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
    std::remove_if( this->openRequests.begin(), this->openRequests.end(), [id](udp_request_entry_t x){ return x.id == id;});

    //std::cout << this->openRequests.size() << std::endl;

    return 0;
}
