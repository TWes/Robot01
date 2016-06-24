#ifndef TABLEWIDGET_HPP
#define TABLEWIDGET_HPP

#include <qt4/Qt/QtGui>
#include <qt4/Qt/QtCore>
#include <iostream>

#include "RegisterMapper.hpp"

class TableWidget : public QTableWidget
{
    Q_OBJECT

public:
    TableWidget( QWidget *parent );
    ~TableWidget();

private:
    void initEmptyTable();
    void drawTable();

    void createLabels();
    void fillTabel();

public slots:
    void clearTable();
    void redrawTable();

};


#endif // TABLEWIDGET_HPP
