#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "server_inet.hpp"

class cam_Server : public Server_inet
{
	public:
		cam_Server(int portnr);
		~cam_Server();


		cv::VideoCapture  webcam;

		int state;

		void setup();
		void handle_connection( int client_handle);
		void cleanup();
};

cam_Server::cam_Server(int portnr) : Server_inet( portnr )
{
	this->state = 0;
}

cam_Server::~cam_Server()
{
}

void cam_Server::setup()
{
	std::cout << "starting video capturing" << std::endl;	
}

void cam_Server::handle_connection(int client_handle)
{
	// No Camera connected
	if( state == 0 )
	{
		static int last_cam_opened = 0;

		webcam = cv::VideoCapture(  last_cam_opened );

		if( !webcam.isOpened() )
		{
		 	std::cout << "No camera detected under nmb: " << last_cam_opened << std::endl;

			last_cam_opened = ((last_cam_opened+1)%10);
			if( last_cam_opened == 0 ) // If we're trying the first video again, sleep a little bit
			{
				sleep(1);
			}

		}	
		else
		{
			state = 1;
		}

		return;
	}

	else if( state == 1) 
	{


		cv::Mat image;

		webcam >> image;

		if( image.data == NULL )
		{
			std::cout << "No image available" << std::endl;
			return;
		}

		cvtColor( image, image, CV_RGB2GRAY);

		// original Seitenverhältniss beibehalten
		cv::resize( image, image, cv::Size( 250,250) );
	
		int buffer[3];
		buffer[0] = image.type();
		buffer[1] = image.cols;
		buffer[2] = image.rows;
	
		int ret = write( client_handle, buffer, sizeof(int)*3 );
	
		if( ret <= 0 )
		{
			std::cout << "Konnte nicht schreiben" << std::endl;
			return;
		}

		int bytes_to_write = buffer[1]*buffer[2];
		int bytes_written = 0;

		char* image_buffer = new char[bytes_to_write]; 	
	
		for( int i = 0; i <bytes_to_write; i++ )
		{
			int x = i % buffer[1];
			int y = i / buffer[2];
	
			 image_buffer[ i ] = image.at<uchar>(x, y) ;
		}


		do{
			//std::cout 	<< "bytes to write: " << bytes_to_write
			//				<< "bytes written: " <<  bytes_written << std::endl;

				ret =  write( client_handle, 
						&image_buffer[bytes_written] , 
						bytes_to_write - bytes_written );

				if( ret < 0 )
				{
					bytes_written =  bytes_written;
				}
				else 
				{
					 bytes_written += ret;
				}

			} while( bytes_to_write != bytes_written );

		delete image_buffer;

		cv::waitKey( 100 );
	}
}

void cam_Server::cleanup()
{}

cam_Server *Server = NULL;

void sighandler(int signum) { if( Server!= NULL && 
							signum == SIGINT ) Server->end(); }

int main(int argc, char** argv)
{
	signal(SIGINT, sighandler); 

	

	//std::cout << image << std::endl;

	Server = new cam_Server(2553);
	
	Server->run();
	
	return 0;
}



