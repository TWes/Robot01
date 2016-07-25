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

        std::string findAttributeInNode( std::string node, std::string Attribute );
        std::string findAttributeInNode( std::string node, std::string Attribute, std::string WayNode );
        std::string findContentOfLeaf( std::string leafName );

    private:
        int act_depth;

        XMLElement *root;

        std::fstream file;
};





#endif // XMLWRITER_HPP
