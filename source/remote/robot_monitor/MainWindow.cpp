#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow( parent )
{
    QWidget *centralWidget = new QWidget();
    QGridLayout *layout = new QGridLayout();

    testLabel = new QLabel("Hallo Welt!" );
    layout->addWidget( testLabel );

    createActions();
    createMenu();

    centralWidget->setLayout( layout );
    setCentralWidget( centralWidget );
}


MainWindow::~MainWindow()
{
    delete testLabel;

}

void MainWindow::createActions()
{
    openConnectionWindowAction = new QAction(tr("Open Connection"), this );
    QObject::connect( openConnectionWindowAction, SIGNAL(triggered()),
                      this, SLOT(openConnectionWindow() ) );

}

void MainWindow::createMenu()
{
    connectionMenu = menuBar()->addMenu(tr("Connection"));
    connectionMenu->addAction( openConnectionWindowAction );
}

void MainWindow::openConnectionWindow()
{
    OpenConnectionDialog *dialog = new OpenConnectionDialog(this);

    dialog->exec();

    std::cout << "Open Connection Window" << std::endl;
}
