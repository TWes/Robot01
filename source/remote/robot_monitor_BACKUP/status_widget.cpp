#include "status_widget.hpp"

status_widget::status_widget( QWidget *parent ) : QWidget( parent )
{
    this->setMinimumHeight( 30 );
}

void status_widget::paintEvent(QPaintEvent *event)
{

    QPainter painter( this );

    Position_t act_pose = {0.0,0.0,0.0};

    if( sensor_connection != NULL )
    {
        act_pose = sensor_connection->act_pose;
    }

    painter.setPen( Qt::black );

    std::stringstream ss;
    ss << "( " << sensor_connection->act_pose.x << " , " \
       << sensor_connection->act_pose.y << " )";
    painter.drawText( 10, 10, ss.str().c_str() );

    ss.str( std::string() );
    ss << "Orientation: " << sensor_connection->act_pose.theta << " Rad";
    painter.drawText( 10, 25, ss.str().c_str() );
}
