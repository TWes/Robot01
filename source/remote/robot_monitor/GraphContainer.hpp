#ifndef GRAPHCONTAINER_HPP
#define GRAPHCONTAINER_HPP

#include <iostream>
#include <map>
#include <functional>

#include <QString>
#include <QtWidgets>

#include "SensorConnection.hpp"
#include "Sensor_structs.hpp"

class GraphContainer : public QWidget
{
    Q_OBJECT


public:
    GraphContainer( QWidget *parent );
    ~GraphContainer();

private slots:
    void comboBoxItemChanged(int changedIndex );

private:
    QLabel *testLabel = NULL;

    QComboBox *comboBox = NULL;
    QComboBox *createComboBox();
    void initialicedItemsChecked();
    void toogleItemsChecked( std::string toToggle );
    std::map<std::string, bool> itemsChecked;
    std::map<std::string,get_variable_enume_t> itemService;
    std::map<get_variable_enume_t,std::function<void(char*,int)>> functionService;

    std::map<get_variable_enume_t, int> subscriptionCounter;
    std::map<get_variable_enume_t, int> subscriptionId;
    //std::map<get_variable_enume_t, void (*)(char*, int)> subscriptionFunction;
    void getRawIMUValues(char* message, int length );
    void getStatusValues(char* message, int length );


};


#endif // GRAPHCONTAINER_HPP
