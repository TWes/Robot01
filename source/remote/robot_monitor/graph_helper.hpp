#ifndef GRAPH_HELPER_HPP
#define GRAPH_HELPER_HPP

#include <QtGui>
#include <QtCore>

#include "Sensor_Connection.hpp"
#include "sensor_protocol.hpp"

extern Sensor_Connection *sensor_connection;

class GraphHelper : public QWidget
{
    Q_OBJECT

public:
    GraphHelper(QWidget *parent = 0);
    ~GraphHelper();

private:

    QGridLayout *layout;

    QLabel *testLabel;
    QComboBox *plot1Combo;
    QPushButton *plot1SubscribeButton;

    QPushButton *plot1UnsubscribeButton;

private slots:
    void emitPlot1Subscribe();
    void emitPlot1Unsubscribe();


};

#endif // GRAPH_HELPER_HPP
