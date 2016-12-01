#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow( parent )
{
    QWidget *centralWidget = new QWidget();
    QGridLayout *layout = new QGridLayout();

    testLabel = new QLabel("Hallo Welt!" );
    layout->addWidget( testLabel );

    graphContainer = new GraphContainer(this);
    layout->addWidget( graphContainer );

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
    openConnectionWindowAction->setShortcut( QKeySequence( tr("Ctrl+e")) );

    addAction( openConnectionWindowAction );

    closeWindowAction = new QAction( tr("Close Window"), this );
    closeWindowAction->setShortcut( QKeySequence(tr("Ctrl+q")) );
    QObject::connect( closeWindowAction, SIGNAL(triggered()),
                      this, SLOT(closeWindow()) );

    addAction( closeWindowAction );

}

void MainWindow::createMenu()
{
    mainMenu = menuBar()->addMenu(tr("Menu"));
    mainMenu->addAction( closeWindowAction );

    connectionMenu = menuBar()->addMenu(tr("Connection"));
    connectionMenu->addAction( openConnectionWindowAction );
}

void MainWindow::openConnectionWindow()
{
    OpenConnectionDialog *dialog = new OpenConnectionDialog(this);

    dialog->exec();

    //std::cout << "Open Connection Window" << std::endl;
}



void MainWindow::closeWindow()
{
    SensorConnection::getInstance()->shutdownTCPConnection();

    this->close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    this->closeWindow();
}
