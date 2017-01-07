#include "DialogWindows.hpp"


OpenConnectionDialog::OpenConnectionDialog(QWidget *parent) : QDialog(parent)
{
    input = new QLineEdit("192.168.2.102:2553", this);
    cancelButton = new QPushButton( "Cancel", this);
    acceptButton = new QPushButton( "Connect", this);

    statusBar = new ConectionStatus( Qt::red, "Not Connected", this );

    QObject::connect(acceptButton, SIGNAL(clicked()),
                     this, SLOT(acceptButtonClicked()) );

    QObject::connect(cancelButton, SIGNAL(clicked()),
                     this, SLOT(cancelButtonClicked()) );

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(acceptButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(input);
    layout->addWidget( this->statusBar );
    layout->addLayout(buttonLayout);

    setLayout( layout );
}


OpenConnectionDialog::~OpenConnectionDialog()
{
}

void OpenConnectionDialog::cancelButtonClicked()
{
    this->close();
}

void OpenConnectionDialog::acceptButtonClicked()
{
    acceptButton->setEnabled(false);

    QString ipString = input->text();

    // Check if input is valid
    if( OpenConnectionDialog::checkIfValidIP(ipString) )
    {
        // setup connection
        //std::cout << "Input: \"" << ipString.toStdString() << "\" is valid." << std::endl;

        // split the input
        QStringList splitetInput = ipString.split( ":", QString::SkipEmptyParts );

        std::string targetIP = splitetInput.at(0).toStdString();
        int targetPort = splitetInput.at(1).toInt();

        this->statusBar->setContent( Qt::yellow, "Connecting..." );

        try{
            SensorConnection::getInstance()->setupTCPConnection( targetIP, targetPort, 5000 );

            auto helperPointer = this->statusBar;

            // Send nothing
            SensorConnection::getInstance()->testTCPConnection(
                        [helperPointer]()
                            {
                                // Error -> timeout
                                helperPointer->setContent( Qt::red, "Not Connected", "TImeout while trying to comunicate");
                                //std::cout << "Timeout" << std::endl;
                            },
                        [helperPointer,this]()
                            {
                                 // Success
                                helperPointer->setContent( Qt::green, "Connected" );
                                //std::cout << "Connected" << std::endl;
                            }
                        );
        }
        catch( tcp::Timeout &exc )
        {
            std::cout << "In dialouge: " << exc.what() << std::endl;
            this->statusBar->setContent( Qt::red, "No Connection", exc.what() );
        }
        catch( tcp::connectionError &exc )
        {
            std::cout << "In dialouge: " << exc.what() << std::endl;
            this->statusBar->setContent( Qt::red, "No Connection", exc.what() );
        }
    }
    else
    {
        // Give out error message
        QMessageBox *msg = new QMessageBox(this);
        msg->setText("Input has wrong Format");
        msg->exec();
    }

    acceptButton->setEnabled(true);
}

/**
 * @brief OpenConnectionDialog::checkIfValidIP Checks if the input has the Format
 *  <3digits>.<3digits>.<3digits>.<3digits>:<integer>
 * @param toCheck The string to check
 * @return true if input has valif Format, false - otherwise
 */
bool OpenConnectionDialog::checkIfValidIP(QString toCheck)
{
    QRegExp expression( "[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}:[0-9]{1,6}" );

    return expression.exactMatch( toCheck );
}
