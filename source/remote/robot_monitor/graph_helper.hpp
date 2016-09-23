#ifndef GRAPH_HELPER_HPP
#define GRAPH_HELPER_HPP

#include <QtGui>
#include <QtCore>
#include <sys/time.h>

#include "Sensor_Connection.hpp"
#include "sensor_protocol.hpp"
#include "Sensor_structs.hpp"
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
    void GetNewFilteredMeas(  Status_tuple_t _meas);

private:
    GraphPlotter *graphPlotter;
    QGridLayout *layout;

    const float nmbOfBoxes = 21;
    QCheckBox *checkBoxes[21];
    bool boxValues[21] = {false};

    QCheckBox *scrollOnNewValue;

    int boxesThatNeedsIMU = 0;
    int boxesThatNeedsFilteredValues = 0;

    double startTimestamp = 0.0;


private slots:
    void CheckBoxesChanged();

};

#endif // GRAPH_HELPER_HPP
