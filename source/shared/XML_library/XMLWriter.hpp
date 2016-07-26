#ifndef XMLWRITER_HPP
#define XMLWRITER_HPP

#include <fstream>

#include "XMLElement.hpp"


class XMLWriter
{
   public:
        XMLWriter();
        ~XMLWriter();


        XMLElement *addRoot( std::string name);
        XMLElement *getRoot();

        void printTree();
        void WriteToFile( std::string filepath );

        XMLElement* ReadFromFile( std::string filepath );


        XMLElement* getNode( std::string node );

        std::string findAttributeInNode(std::string node, std::string Attribute, std::string WayNode = std::string() );

        float findAttributeInNodeAsFloat(std::string node, std::string Attribute, std::string WayNode = std::string() );

        std::string findContentOfLeaf( std::string leafName );

    private:
        int act_depth;

        XMLElement *root;

        std::fstream file;
};



#endif // XMLWRITER_HPP
