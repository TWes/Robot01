#include "map_widget.hpp"

map_widget::map_widget(QWidget *parent) : QWidget( parent )
{


}

map_widget::~map_widget()
{


}

void map_widget::paintEvent(QPaintEvent *event)
{
    this->setMinimumSize( 100, 100 );

    QPainter painter( this );

    this->visible_rect = QRect( -10, 10, 20, 20 );

    painter.setBrush( Qt::white );
    painter.drawRect(event->rect().x(), event->rect().y(), event->rect().width()-1, event->rect().height()-1 );

    this->drawKoodinateFrame( &painter, event );

    this->drawRobotPose( &painter, event );
}

void map_widget::drawKoodinateFrame( QPainter *painter, QPaintEvent *event )
{
    QLine line[2];

    double x_res = (double) event->rect().width() / (double) this->visible_rect.width() ;
    double y_res = (double) event->rect().height() / (double) this->visible_rect.height();

    double rel_x_pos = 0.0  - ( (double) this->visible_rect.x());
    double rel_y_pos = -1.0 * ( 0.0 - (double) this->visible_rect.y() );

    // Now scale
    rel_x_pos *= x_res;
    rel_y_pos *= y_res;

    // X- Achse
    line[0].setP1( QPoint( 0, rel_y_pos ) );
    line[0].setP2( QPoint( event->rect().width() , rel_y_pos ) );

    line[1].setP1( QPoint( rel_x_pos, 0 ) );
    line[1].setP2( QPoint( rel_x_pos, event->rect().height() ) );

    //std::cout << rel_x_pos << " | " << rel_y_pos << std::endl;

    painter->drawLines( line, 2);
}

QPoint map_widget::convertToActFrame(double x_val, double y_val, QPaintEvent *event)
{
    double x_res = (double) event->rect().width() / (double) this->visible_rect.width() ;
    double y_res = (double) event->rect().height() / (double) this->visible_rect.height();

    double rel_x_pos = x_val - this->visible_rect.x();
    double rel_y_pos = -1.0 * ( y_val - this->visible_rect.y() );

    // Now scale
    rel_x_pos *= x_res;
    rel_y_pos *= y_res;

    QPoint ret;
    ret.setX( rel_x_pos );
    ret.setY( rel_y_pos );

    return ret;
}

void map_widget::drawRobotPose(QPainter *painter , QPaintEvent *event)
{
    QPoint Robot_Pose = this->convertToActFrame( sensor_connection->act_pose.x,
                                                 sensor_connection->act_pose.y,
                                                 event );

    painter->setPen( QPen( Qt::red, 5, Qt::SolidLine, Qt::RoundCap) );
    painter->drawPoint( Robot_Pose );

    // Draw the orientatian;
    // Berechne Steigung
    const double lenght_of_orientation_line = 2.0;

    double theta = sensor_connection->act_pose.theta;
    while( theta >= (2* M_PI) )
    {
        theta -= (2* M_PI);
    }

    double skew_y = tan( theta );
    double skew_x = 1.0;

    if( theta == 0.5 * M_PI )
    {
        skew_y = 1.0;
        skew_x = 0.0;
    }

    else if( theta == (3.0*M_PI)/2.0 )
    {
        skew_y = -1.0;
        skew_x = 0.0;
    }

    else if( theta >= 0.5 * M_PI && theta < (3.0*M_PI)/2.0)
    {
        skew_y *= -1.0;
        skew_x *= -1.0;
    }

    double distance = sqrt( (skew_x*skew_x) + (skew_y*skew_y) );
    skew_x = (skew_x * lenght_of_orientation_line) / distance;
    skew_y = (skew_y * lenght_of_orientation_line) / distance;

    skew_x *= ((double) event->rect().width() / (double) this->visible_rect.width() );
    skew_y *= ((double) event->rect().height() / (double) this->visible_rect.height() );

    QLine line;
    line.setP1( Robot_Pose );
    line.setP2( QPoint(Robot_Pose.x() + skew_x, Robot_Pose.y() - skew_y) );

    painter->setPen( QPen( Qt::red, 3, Qt::SolidLine, Qt::RoundCap) );
    painter->drawLine( line );
}
