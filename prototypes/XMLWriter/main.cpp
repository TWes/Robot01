#include <iostream>

#include "XMLWriter.hpp"

int main( int argc, char** argv )
{
    XMLWriter writer;
    XMLElement *iterator;
    XMLElement *iterator2;

    iterator = writer.addRoot( "SensorConfig" );

    // iterator is root now
    iterator2 = iterator->addElement( "Magnetometer");
    iterator2->addComment("The Magnetometer config file");

    XMLElement *iter = iterator2->addElement( "Scale" );
    iter->setNoChild();
    iter->addAttribute( "ScaleX", "0.81");
    iter->addAttribute( "ScaleY", "1.0");
    iter->addAttribute( "ScaleZ", "1.0");

    iterator = iterator->addElement( "Accelerometer");
    iterator = iterator->addElement( "Scale");
    iterator->addAttribute( "ScaleX", "0.99");
    iterator->addAttribute( "ScaleY", "1.0");
    iterator->addAttribute( "ScaleZ", "1.0");

    writer.WriteToFile( "SensorConfig.xml" );

    std::cout << "--------------------------" << std::endl;

    writer.ReadFromFile( "SensorConfig.xml" );

    writer.printTree();


    return 0;
}
