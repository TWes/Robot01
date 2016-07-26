#include "XMLWriter.hpp"

XMLWriter::XMLWriter()
{
    this->root = NULL;

}

XMLWriter::~XMLWriter()
{
    if( this->root != NULL )
            delete this->root;
}


/**
 * @brief XMLWriter::printTree
 * Prints the tree to console
 */
void XMLWriter::printTree()
{
    if( this->root != NULL )
    {
        std::cout << this->root->getTreeAsString() << std::endl;
    }

}

XMLElement *XMLWriter::addRoot( std::string name)
{
    if( this->root != NULL )
    {
        delete this->root;
    }

    this->root = new XMLElement();
    this->root->setRoot();
    this->root->setName( name );
    this->root->setDepth( 1 );

    this->act_depth = 1;

    return this->root;
}


XMLElement *XMLWriter::getRoot()
{
    return this->root;
}

/**
 * @brief XMLWriter::WriteToFile Write Tree to file
 * @param filepath Filepath to write in.
 */
void XMLWriter::WriteToFile( std::string filepath )
{
    std::fstream file;
    file.open( filepath.c_str(), std::ios_base::out | std::ios_base::trunc );

    file << this->root->getTreeAsString() << std::flush;

    file.close();
}

/**
 * @brief ReadFromFile ready an XML file and set it to root
 * @param filepath The file to load from.
 */
XMLElement *XMLWriter::ReadFromFile( std::string filepath )
{
    if( this->root != NULL )
    {
        delete this->root;
        this->root = NULL;
    }

    std::fstream file;
    file.open( filepath.c_str(), std::ios_base::in );

    if( !file.is_open() ) return NULL;

    this->root = new XMLElement();
    this->root->setRoot();
    this->root->setDepth( 1 );
    this->act_depth = 1;

    this->root->readElement( file, 1 );


    file.close();

    return this->root;
}

std::string XMLWriter::findAttributeInNode(std::string node, std::string Attribute, std::string WayNode )
{
    XMLElement* tmp = this->getRoot();

    if( WayNode.size() > 0)
    {
        tmp = this->root->findNode( WayNode );

        if( tmp == NULL ) return std::string("");
    }

    tmp = tmp->findNode( node );

    if( tmp == NULL ) return std::string("");


    return tmp->getAttribute( Attribute );
}


float  XMLWriter::findAttributeInNodeAsFloat(std::string node, std::string Attribute, std::string WayNode )
{
    std::string value = this->findAttributeInNode( node, Attribute, WayNode );

    float ret = atof( value.c_str() );

    return ret;
}


std::string XMLWriter::findContentOfLeaf( std::string leafName )
{
    XMLElement* tmp = this->root->findNode( leafName );

    if( tmp == NULL ) return std::string("");

    return tmp->getContent();
}


XMLElement* XMLWriter::getNode( std::string node )
{
    if( this->root == NULL ) return NULL;

    return this->root->findNode( node );

}

