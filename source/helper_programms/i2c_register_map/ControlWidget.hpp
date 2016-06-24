#ifndef CONTROLWIDGET_HPP
#define CONTROLWIDGET_HPP

#include <qt4/Qt/QtGui>
#include <qt4/Qt/QtCore>
#include <qt4/Qt/qstring.h>

#include "RegisterMapper.hpp"

class ControlWidget : public QWidget
{
        Q_OBJECT

public:
    ControlWidget( QWidget *parent );
    ~ControlWidget();

private:
    QGridLayout* layout;

    QLineEdit *deviceAddressInput;
    QLineEdit *firstRegisterInput;
    QLineEdit *sizeInput;

    QPushButton *readButton;
    QPushButton *clearButton;

    int stringToUInt( QString input );

private slots:
    void readRegisters();

signals:
    void clear();
    void redrawTable();


};



#endif // CONTOLWIDGET_HPP
