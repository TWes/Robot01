#ifndef DIALOGWINDOWS_HPP
#define DIALOGWINDOWS_HPP

#include <QDialog>
#include <QtWidgets>
#include <QObject>
 #include <QRegExp>

#include <iostream>

#include "SensorConnection.hpp"
#include "connectionStatus.hpp"

class OpenConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    OpenConnectionDialog(QWidget *parent);
    ~OpenConnectionDialog();

private:
    QLineEdit *input;    
    ConectionStatus *statusBar;
    QPushButton *acceptButton;
    QPushButton *cancelButton;

    static bool checkIfValidIP( QString toCheck );

private slots:
    void cancelButtonClicked();
    void acceptButtonClicked();

};



#endif
