#include "TableWidget.hpp"

TableWidget::TableWidget(QWidget *parent) : QTableWidget( parent )
{
    this->initEmptyTable();
}

void TableWidget::initEmptyTable()
{
    removeRow(this->rowCount()-1);

    this->setRowCount(1);
    this->setColumnCount(1);

    QStringList column_headder;
    column_headder << "Register";

    this->setHorizontalHeaderLabels(column_headder);

    QStringList row_headder;
    row_headder << "0x00";

    this->setVerticalHeaderLabels( row_headder );
}

void TableWidget::drawTable()
{
    this->createLabels();

    this->fillTabel();
}

void TableWidget::createLabels()
{
    // Setup row labels
    int firstRegister = RegisterMapper::getFirstRegister();
    int lastRegister = RegisterMapper::getLastRegister();

    this->setRowCount(lastRegister - firstRegister +1);

    QStringList row_headder;
    for( unsigned int i = firstRegister; i <= lastRegister; i++ )
    {
        QString label;
        label.setNum(i, 16);

        if( label.size() == 1 )
        {
            label = "0" + label;
        }

        label = "0x" + label;

        row_headder << label;
    }

    this->setVerticalHeaderLabels( row_headder );

    // MAke horizontal label
    int maps = RegisterMapper::NmbOfMaps();
    int columns = 2*maps -1;

    this->setColumnCount( columns );

    QStringList column_headder;
    column_headder << "content";

    for( int i = 1; i < maps; i++)
    {
        column_headder << "diff";
        column_headder << "content";
    }

    this->setHorizontalHeaderLabels( column_headder );
}

void TableWidget::fillTabel()
{
    int columns = 2*RegisterMapper::NmbOfMaps() -1;
    int firstRegister = RegisterMapper::getFirstRegister();
    int lastRegister = RegisterMapper::getLastRegister();

    for( int c = 0; c < columns; c++)
    {
        bool even = !(c % 2);

        if( even )
        {
            for( int r = firstRegister; r <= lastRegister; r++ )
            {
                int row = r - firstRegister;

                char cToInsert = RegisterMapper::getItem(r, c/2);

                QString toInsert;

                toInsert.setNum( (uchar) cToInsert, 16);

                if( toInsert.size() == 1 )
                {
                    toInsert = "0" + toInsert;
                }

                toInsert = "0x" + toInsert;

                this->setItem(row, c, new QTableWidgetItem(toInsert)) ;

            }
        }
        else
        {
            for( int r = firstRegister; r <= lastRegister; r++ )
            {
                int row = r - firstRegister;

                char cToInsert = RegisterMapper::compareRegister( r, c/2, (c+1)/2);

                QString toInsert;

                toInsert.setNum( (uchar) cToInsert, 2);

                while( toInsert.size() < 8 )
                {
                    toInsert = "0" + toInsert;
                }

                toInsert = "0b" + toInsert;

                this->setItem(row, c, new QTableWidgetItem(toInsert)) ;
            }


        }
    }

}


void TableWidget::clearTable()
{
    RegisterMapper::clearAll();

    this->initEmptyTable();
}

void TableWidget::redrawTable()
{
    this->drawTable();
}



TableWidget::~TableWidget()
{

}
