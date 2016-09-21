#include "Sensor_Connection.hpp"

Sensor_Connection::Sensor_Connection(std::string ip_address, int port, QWidget *parent) \
        : socket_inet( ip_address, port), QWidget( NULL )
{
    this->udp_socket = NULL;

    this->actual_id = 1;

    this->continue_server = false;

    this->parent = parent;
}

Sensor_Connection::~Sensor_Connection()
{
    if( this->udp_socket != NULL )
    {
        delete this->udp_socket;
    }
}

int Sensor_Connection::get_Pose()
{
    uint16_t headder[3];

    request_entry_t new_entry;

    new_entry.id = this->actual_id++;
    new_entry.stream = false;
    gettimeofday( &new_entry.timestamp, NULL );
    new_entry.time_lo_live.tv_sec = 3;
    new_entry.time_lo_live.tv_usec = 0;
    new_entry.todo_action = WRITE_POSE;

    headder[0] = GET_VARIABLE;
    headder[1] = sizeof( uint32_t );
    headder[2] = new_entry.id;

    // receive the pose
    uint32_t data[1];
    data[0] = GET_POSE;

    char message[ 3*sizeof(uint16_t) + 1 * sizeof(uint32_t ) ];

    memcpy( message, headder, 3*sizeof(uint16_t) );
    memcpy( (message + 3*sizeof(uint16_t) ), data, 1 * sizeof(uint32_t ) );    

    open_requests_mutex.lock();
        this->open_requests.push_back( new_entry );
    open_requests_mutex.unlock();

    int ret = this->sendData( message, 3*sizeof(uint16_t) + 1 * sizeof(uint32_t ) );

    if( ret < 0 )
    {
        // Fehler
        return -1;
    }

    return 0;
}

int Sensor_Connection::init_UDP_Pose()
{
    emit debugOutput( "Init UDP Pose" );

    // Erstelle den udp socket
    if( this->udp_socket == NULL )
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
    new_entry.todo_action = WRITE_POSE;

    headder[0] = SUBSCRIBE_UDP;
    headder[1] = 3 * sizeof( uint32_t );
    headder[2] = new_entry.id;

    // receive the pose
    uint32_t data[3];
    data[0] = GET_POSE;
    data[1] = udp_socket_information.port_nr;
    data[2] = 1000; // intervall in ms

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

    return 0;
}

request_entry_t* Sensor_Connection::getEntryById( int id )
{
    request_entry_t* ret = NULL;

    std::vector<request_entry_t>::iterator iter;
    for( iter = this->open_requests.begin(); iter != this->open_requests.end(); iter++ )
    {
        if( iter->id == id )
        {
            ret = new request_entry_t;
            *ret = *iter;

            if( !ret->stream )
                this->open_requests.erase( iter++ );

            break;
        }
    }

    return ret;
}

void Sensor_Connection::receiving_thread_funktion()
{
    while( this->continue_server )
    {
        if( this->state == WAIT_FOR_CONNECTION )
        {
            sleep(1);
            continue;
        }

        fd_set clients_to_watch;
        FD_ZERO( &clients_to_watch );
        FD_SET( this->socket_fd, &clients_to_watch  );

        struct timeval timeout;
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;

        //std::cout << "Waiting for stuff" << std::endl;

        int ret = select( this->socket_fd+1, &clients_to_watch, NULL, NULL, &timeout );

        if( ret < 0 )
        {
            if( errno == 9 && !this->is_connected ) // Server just not connected
            {
                continue;
            }

            emit debugOutput( "Fehler bei select: " + QString(errno) );
            continue;
        }
        else if (ret == 0) // timeout
        {
            emit debugOutput( "Timeout bei select" );

            continue;
        }

        // Read the headder
        uint16_t headder[3];
        ret = this->readData( (char*) headder, 3* sizeof(uint16_t ));

        if( ret < 0 )
        {
            emit debugOutput( "Fehler beim lesen eines headders" );
            std::cout << "Fehler beim lesen eines headders" << std::endl;
            continue;
        }
        // If the socket is readable, but could not read
        // anithing, the server is down
        else if( ret == 0 )
        {
            emit debugOutput( "Server is down" );

            this->shutdown_connection();
            this->state = WAIT_FOR_CONNECTION;
        }

        char* buffer = (char*) malloc( headder[1] );

        ret = this->readData( (char*) buffer, headder[1] );

        if( ret < 0 )
        {
            emit debugOutput( "Fehler beim lesen der daten." );
            std::cout << "Fehler beim lesen der daten." << std::endl;
            continue;
        }

        // TODO: Check for Timeouts and streams
        //Find entry in open request list
        request_entry_t *act_entry = NULL;

        this->open_requests_mutex.lock();
        std::vector<request_entry_t>::iterator iter;

        for( iter = this->open_requests.begin(); iter != this->open_requests.end(); iter++ )
        {
            if( iter->id == headder[2] )
            {
                act_entry = new request_entry_t;
                *act_entry = *iter;

                this->open_requests.erase( iter++ );

                break;
            }
        }
        this->open_requests_mutex.unlock();

        if( act_entry == NULL ) // Nothing found
        {
            std::cout << "Have got request with no open request." << std::endl;
            std::cout << headder[0] << " | " << headder[1] << " | " << headder[2] << std::endl;

            delete buffer;
            continue;
        }

        switch( act_entry->todo_action )
        {
        case WRITE_POSE:
            this->act_pose = *((Position_t*) buffer);
            emit debugOutput( "GotPose: " + QString::number( this->act_pose.theta ) );
            this->parent->update();
            break;
        default:
            break;
        }

        delete buffer;

    } // End of main while loop
}

void Sensor_Connection::polling_thread_funktion()
{
    this->state = WAIT_FOR_CONNECTION;

    while ( this->continue_server )
    {
        if( state == WAIT_FOR_CONNECTION )
        {
            emit debugOutput( "Wait for connection: " + QString::number(errno) );

            int ret = this->start_connection();

            if( ret < 0 )
            {
                sleep(1);

                continue;
            }

            emit debugOutput( "Connection established" );

            this->init_UDP_Pose();

            state = CONNECTION_ESTABLISHED;
        }

        else if( CONNECTION_ESTABLISHED )
        {

            sleep( 1 );
        }
    }

    std::cout << "End of polling thread" << std::endl;

    this->shutdown_connection();
}

void Sensor_Connection::end()
{
    this->continue_server = false;
}

void Sensor_Connection::start_server()
{
    this->continue_server = true;

    this->polling_thread = std::thread( &Sensor_Connection::polling_thread_funktion, this );
    this->receiving_thread = std::thread( &Sensor_Connection::receiving_thread_funktion, this );
}

void Sensor_Connection::end_server()
{
    this->continue_server = false;

    this->polling_thread.join();
    this->receiving_thread.join();
}

udp_connection::udp_connection( Sensor_Connection *sensor ) : udp_connection_inet()
{
    this->connection = sensor;
}

void udp_connection::handle_connection(char *message, int message_lenght, udp_connection_information_t other)
{
    uint16_t headder[3];

    //std::cout << "UDP receive: " << message << std::endl;
    // get Headders
    if( message_lenght >= sizeof(headder) )
    {
        memcpy( headder, message, sizeof(headder) );

        /*std::cout << "h(0): " << headder[0] << "\n"
                  << "h(1): " << headder[1] << "\n"
                  << "h(2): " << headder[2] << std::endl; */
    }
    else
    { return; }    

    request_entry_t *act = this->connection->getEntryById( headder[2] );

    switch(  act->todo_action )
    {
    case WRITE_POSE:
        Position_t recv_pose;

        memcpy( &recv_pose, (message + sizeof(headder)), sizeof( Position_t ) );

        this->connection->act_pose = recv_pose;
        break;
    case PLOT1_IMU:
        IMU_Measurement imuMeas;

        memcpy( &imuMeas, (message + sizeof(headder)), sizeof( IMU_Measurement ) );

        // Tell the Graph Helper the Values
        globalGraphHelper->GetNewIMUMeas( imuMeas );

        break;
    case PLOT1_FILTERED_VALUES:
        Status_tuple_t tuple;
        memcpy( &tuple, (message + sizeof(headder)), sizeof( Status_tuple_t ) );
        globalGraphHelper->GetNewFilteredMeas( tuple );
        break;

    default:
        std::cout << "Got Action: " << act->todo_action << std::endl;
    }

    delete act;

    return;
}

int Sensor_Connection::init_UDP_Var( get_variable_enume_t _to_subscribe, action_t _todo, int sending_interval)
{
    // Erstelle den udp socket
    if( this->udp_socket == NULL )
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

    return 0;


}

int Sensor_Connection::unsubscribe_UDP( get_variable_enume_t _to_subscribe  )
{
    std::cout << "Unsubscribe UDP: " << _to_subscribe << std::endl;

    // Send the server to stop


    // delete the request

}
