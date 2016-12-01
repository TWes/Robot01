#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QtWidgets>
#include <QObject>

#include <iostream>

#include "DialogWindows.hpp"
#include "GraphContainer.hpp"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent );
    ~MainWindow();

private slots:
    void openConnectionWindow();
    void closeWindow();

private:
    QLabel *testLabel = 0;
    GraphContainer *graphContainer;

    // Actions
    void createActions();
    QAction *openConnectionWindowAction;
    QAction *closeWindowAction;

    // Menues
    void createMenu();
    QMenu *connectionMenu;
    QMenu *mainMenu;


    void closeEvent(QCloseEvent *event);

};

#endif
