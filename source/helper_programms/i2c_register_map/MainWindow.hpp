#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <qt4/Qt/QtGui>

#include "ControlWidget.hpp"
#include "TableWidget.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private:
    QWidget *centralWidget;
    QVBoxLayout *layout;


    ControlWidget *controlWidget;
    TableWidget *tableWidget;


};


#endif // MAINWINDOW_HPP
