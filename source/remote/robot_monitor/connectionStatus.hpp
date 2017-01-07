#ifndef CONNECTIONSTATUS_HPP
#define CONNECTIONSTATUS_HPP

#include <QtGui>
#include <QtWidgets>
#include <QStackedLayout>
#include <QGridLayout>
#include <QColor>
#include <qpainter.h>
#include <string>


class dotCircle : public QWidget
{
public:
    dotCircle( QWidget *_parent, QColor _color= Qt::red ) : color(_color), QWidget(_parent)
    { this->setMinimumSize(10,10);}

    void paintEvent(QPaintEvent *event)
    {
        QPainter *painter = new QPainter();
        painter->begin(this);

        // set Pen
        painter->setPen( this->color );

        // set Brush
        painter->setBrush( QBrush( this->color) );

        painter->drawEllipse( event->rect().center(), 5, 5);

        painter->end();
        delete painter;
    }

    QSize sizeHint()
    {
        QSize ret( 100,100 );

        return ret;
    }

    void setColor( QColor color )
    {
        this->color = color;

    }

private:
    QColor color;
};

class ConectionStatus : public QWidget
{
    Q_OBJECT

public:
    ConectionStatus( QColor _dotColor, std::string _mainText, QWidget *_parent  );
    ConectionStatus( QColor _dotColor, std::string _mainText, std::string _additionalText, QWidget *_parent  );
    ~ConectionStatus();

    void setContent( QColor _dotColor, std::string _mainText, std::string _additionalText = "" );

private:
    QColor dotColor;
    QString mainText;
    QString additionalText;

    QLabel *mainLabelSmall = NULL;
    dotCircle *statusCircleSmall = NULL;

    QLabel *mainLabelFull = NULL;
    dotCircle *statusCircleFull = NULL;
    QLabel *additionalLabelFull = NULL;

    QStackedLayout *mainLayout = NULL;
    QHBoxLayout *upperLine = NULL;
    QGridLayout *fullLayout = NULL;

    void setupLayout();

    void updateWidget();
};


#endif // CONNECTIONSTATUS_HPP
