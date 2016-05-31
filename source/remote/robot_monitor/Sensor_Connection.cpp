#include "Sensor_Connection.hpp"

Sensor_Connection::Sensor_Connection(std::string ip_address, int port, QWidget *parent) \
        : socket_inet( ip_address, port), QWidget( NULL )
{
    this->actual_id = 1;

    this->continue_server = false;

    this->parent = parent;
}

Sensor_Connection::~Sensor_Connection()
{
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

    uint32_t data[1];
    data[0] = 1;

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

void Sensor_Connection::receiving_thread_funktion()
{
    while( this->continue_server )
    {
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
            std::cout << "Fehler bei select: " << errno << std::endl;
            continue;
        }
        else if (ret == 0) // timeout
        {
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
            this->act_pose = *((Pose_t*) buffer);
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
        std::cout << "continue server" << std::endl;

        if( state == WAIT_FOR_CONNECTION )
        {

            emit debugOutput( "Wait for connection: " + QString::number(errno) );

            std::cout << "Wair for connection: " << errno << std::endl;

            int ret = this->start_connection();

            std::cout << "ret start connection" << std::endl;

            if( ret < 0 )
            {
                //sleep(1);

                continue;
            }

            state = CONNECTION_ESTABLISHED;
        }

        else if( CONNECTION_ESTABLISHED )
        {
            std::cout << "Connection established" << errno << std::endl;

            // Regulaly update the variables

            int ret = this->get_Pose();

            if( ret < 0 )
            {
                continue;
            }

            //usleep( 1000000 );
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
