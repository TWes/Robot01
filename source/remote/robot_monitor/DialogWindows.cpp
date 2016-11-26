#include "DialogWindows.hpp"


OpenConnectionDialog::OpenConnectionDialog(QWidget *parent) : QDialog(parent)
{
    input = new QLineEdit("192.123.2.13:200", this);
    cancelButton = new QPushButton( "Cancel", this);
    acceptButton = new QPushButton( "Connect", this);

    QObject::connect(acceptButton, SIGNAL(clicked()),
                     this, SLOT(acceptButtonClicked()) );

    QObject::connect(cancelButton, SIGNAL(clicked()),
                     this, SLOT(cancelButtonClicked()) );

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(acceptButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(input);
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

        try{
            SensorConnection::getInstance()->setupTCPConnection( targetIP, targetPort, 5000 );
        }
        catch( tcp::Timeout &exc )
        {
            std::cout << exc.what() << std::endl;
        }

        std::cout << "Connected to : " << ipString.toStdString() << std::endl;
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
