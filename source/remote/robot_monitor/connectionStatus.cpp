#include "connectionStatus.hpp"

ConectionStatus::ConectionStatus( QColor _dotColor, std::string _mainText, QWidget *_parent ) : QWidget(_parent)
{
    this->dotColor = _dotColor;
    this->mainText = QString::fromStdString( _mainText );
    this->additionalText = QString();

    // setup stacked Layout
    this->setupLayout();
}

ConectionStatus::ConectionStatus( QColor _dotColor, std::string _mainText, std::string _additionalText, QWidget *_parent ) : QWidget(_parent)
{
    this->dotColor =  _dotColor ;
    this->mainText = QString::fromStdString( _mainText );
    this->additionalText = QString::fromStdString( _additionalText );

    // setup stacked Layout
    this->setupLayout();
}

ConectionStatus::~ConectionStatus()
{
    delete this->mainLayout;
}

void ConectionStatus::setupLayout()
{
    // First Layout
    this->mainLayout = new QStackedLayout(this);

    this->mainLabelSmall = new QLabel( this->mainText, this );
    this->statusCircleSmall = new dotCircle(this, this->dotColor );

    this->upperLine = new QHBoxLayout();
    this->upperLine->addWidget( this->statusCircleSmall );
    this->upperLine->addWidget( this->mainLabelSmall );
    QWidget *uperLineWidget = new QWidget( this );
    uperLineWidget->setLayout( this->upperLine );


    // Second Layout
    this->mainLayout->addWidget( uperLineWidget );

    this->mainLabelFull = new QLabel( this->mainText, this );
    this->statusCircleFull = new dotCircle(this, this->dotColor );
    this->additionalLabelFull = new QLabel( this->additionalText, this );

    this->fullLayout = new QGridLayout();
    this->fullLayout->addWidget( this->statusCircleFull, 0,0,1,1);
    this->fullLayout->addWidget(  this->mainLabelFull, 0,1,1,1 );
    this->fullLayout->addWidget( this->additionalLabelFull, 1, 1, 1, 2 );
    QWidget *fullLayoutWidget = new QWidget( this );
    fullLayoutWidget->setLayout( this->fullLayout );

    this->mainLayout->addWidget( fullLayoutWidget );

    this->setLayout( this->mainLayout);

    this->updateWidget();
}


void ConectionStatus::setContent( QColor _dotColor, std::string _mainText, std::string _additionalText )
{
    this->dotColor =  _dotColor ;
    this->mainText = QString::fromStdString( _mainText );
    this->additionalText = QString::fromStdString( _additionalText );

    this->updateWidget();
}


void ConectionStatus::updateWidget()
{
    this->mainLabelSmall->setText( this->mainText);
    this->mainLabelFull->setText( this->mainText);

    this->statusCircleSmall->setColor( this->dotColor );
    this->statusCircleFull->setColor( this->dotColor );
    this->additionalLabelFull->setText( this->additionalText);

    if( this->additionalText.length() <= 0 )
    {
        this->mainLayout->setCurrentIndex(0);
    }
    else
    {
        this->mainLayout->setCurrentIndex(1);
    }

    this->update();
    //this->repaint();
}
