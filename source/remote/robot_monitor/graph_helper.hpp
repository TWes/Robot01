#ifndef GRAPH_HELPER_HPP
#define GRAPH_HELPER_HPP

#include <QtGui>
#include <QtCore>
#include <sys/time.h>

#include "Sensor_Connection.hpp"
#include "sensor_protocol.hpp"
#include "GraphPlotter.h"

class Sensor_Connection;

extern Sensor_Connection *sensor_connection;

class GraphHelper : public QWidget
{
    Q_OBJECT

public:
    GraphHelper( GraphPlotter *plotter, QWidget *parent = 0);
    ~GraphHelper();

    void GetNewIMUMeas( IMU_Measurement _meas);

private:
    GraphPlotter *graphPlotter;
    QGridLayout *layout;

    const float nmbOfBoxes = 9;
    QCheckBox *checkBoxes[9];
    bool boxValues[9] = {false};

    int boxesThatNeedsIMU = 0;
    double startTimestamp = 0.0;


private slots:
    void CheckBoxesChanged();

};

#endif // GRAPH_HELPER_HPP
