#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QtWidgets>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent );
    ~MainWindow();


private:
    QLabel *testLabel = 0;

};

#endif
