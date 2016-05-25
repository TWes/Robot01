#include "Sensor_Connection.hpp"

Sensor_Connection::Sensor_Connection(std::string ip_address, int port) \
        : socket_inet( ip_address, port)
{
    this->actual_id = 1;

    this->continue_server = false;
}

Sensor_Connection::~Sensor_Connection()
{
}


int Sensor_Connection::get_Pose(Pose_t *new_pose)
{
    uint16_t headder[3];

    headder[0] = GET_VARIABLE;
    headder[1] = sizeof( uint32_t );
    headder[2] = this->actual_id++;

    uint32_t data[1];
    data[0] = 1;

    char message[ 3*sizeof(uint16_t) + 1 * sizeof(uint32_t ) ];

    memcpy( message, headder, 3*sizeof(uint16_t) );
    memcpy( (message + 3*sizeof(uint16_t) ), data, 1 * sizeof(uint32_t ) );

    this->sendData( message, 3*sizeof(uint16_t) + 1 * sizeof(uint32_t ) );

    int read_ret =this->readData( (char*) headder, 3*sizeof(uint16_t) );

    if( read_ret < 0 )
    {
        if( errno == 104 ) // Connection reseted by peer
        {
            this->state = WAIT_FOR_CONNECTION;

            return -1;
        }

        std::cout << "Konnte Pose nicht lesen: " << errno << std::endl;
    }


    if( headder[0] != (GET_VARIABLE | ANSWER) )
    {
        std::cout << "Got headder " << headder[0] << " | Size : "
                  << headder[1] << " | id: " << headder[2] << std::endl;
    }

    Pose_t ret;

    this->readData( (char*) &ret, sizeof( Pose_t ) );

    *new_pose = ret;

    return 0;
}


void Sensor_Connection::polling_thread_funktion()
{

    this->state = WAIT_FOR_CONNECTION;


    while ( this->continue_server )
    {
        if( state == WAIT_FOR_CONNECTION )
        {
            int ret = this->start_connection();

            if( ret < 0 )
            {
                std::cout << "Fehler beim Aufbau einer Verbindung: " << errno << std::endl;

                sleep(1);

                continue;
            }

            state = CONNECTION_ESTABLISHED;
        }

        else if( CONNECTION_ESTABLISHED )
        {
            // Regulaly update the variables
            Pose_t new_pose;

            int ret = 0;
            ret = this->get_Pose( &new_pose );

            if( ret == 0 ) this->act_pose = new_pose;

            usleep( 1000000 );
        }
    }

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
}

void Sensor_Connection::end_server()
{
    this->continue_server = false;

    this->polling_thread.join();
}
