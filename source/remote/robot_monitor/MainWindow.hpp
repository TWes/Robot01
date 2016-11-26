#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QtWidgets>
#include <QObject>

#include <iostream>

#include "DialogWindows.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent );
    ~MainWindow();

private slots:
    void openConnectionWindow();

private:
    QLabel *testLabel = 0;


    // Actions
    void createActions();
    QAction *openConnectionWindowAction;

    // Menues
    void createMenu();
    QMenu *connectionMenu;

};

#endif
