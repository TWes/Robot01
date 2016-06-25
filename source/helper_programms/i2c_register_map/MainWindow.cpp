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


    // Add Actions
    this->saveFile = new QAction( "Save", this);

    QObject::connect( this->saveFile, SIGNAL(triggered()),
                     this, SLOT(saveFileFunktion()) );


    this->openFile = new QAction( "Open", this);
    QObject::connect( this->openFile, SIGNAL(triggered()),
                     this, SLOT(openFileFunktion()) );


    // Add Menu
    QMenu *dataMenu = this->menuBar()->addMenu( "File" );
    dataMenu->addAction( this->openFile );
    dataMenu->addAction( this->saveFile );
}

void MainWindow::openFileFunktion()
{
    QString filepath = QFileDialog::getOpenFileName( this, "Chose register map", "~/", "Register map (*.rm)");

    if( filepath.size() == 0 ) return;

    RegisterMapper::openMap( filepath );

    this->tableWidget->redrawTable();
}

void MainWindow::saveFileFunktion()
{
    QString filepath = QFileDialog::getSaveFileName( this, "Chose file to save", "~/", "Register map (*.rm)");

    if( filepath.size() == 0 ) return;

    RegisterMapper::saveMap( filepath );
}

MainWindow::~MainWindow()
{
    delete this->controlWidget;
    delete this->tableWidget;

    delete this->openFile;
    delete this->saveFile;
}
