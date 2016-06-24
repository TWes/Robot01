#include "MainWindow.hpp"

MainWindow::MainWindow() : QMainWindow()
{
    this->controlWidget = new ControlWidget( this );
    this->tableWidget = new TableWidget( this );

    this->layout = new QVBoxLayout();
    this->layout->addWidget( this->controlWidget );
    this->layout->addWidget( this->tableWidget );

    this->centralWidget = new QWidget();
    this->centralWidget->setLayout( this->layout );
    this->setCentralWidget( this->centralWidget );

    QObject::connect( this->controlWidget, SIGNAL(clear()),
                      this->tableWidget, SLOT(clearTable()) );
    QObject::connect( this->controlWidget, SIGNAL(redrawTable()),
                      this->tableWidget, SLOT(redrawTable()) );

}


MainWindow::~MainWindow()
{
    delete this->controlWidget;
}
