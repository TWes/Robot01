#include "MainWindow.hpp"


MainWindow::MainWindow(QWidget *parent) : QMainWindow( parent )
{
    QWidget *centralWidget = new QWidget();
    QGridLayout *layout = new QGridLayout();

    testLabel = new QLabel("Hallo Welt!" );
    layout->addWidget( testLabel );

    centralWidget->setLayout( layout );
    setCentralWidget( centralWidget );
}


MainWindow::~MainWindow()
{
    delete testLabel;

}
