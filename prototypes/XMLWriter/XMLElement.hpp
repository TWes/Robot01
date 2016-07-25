#ifndef XMLELEMENT_HPP
#define XMLELEMENT_HPP

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <regex.h>
#include <fstream>
#include <algorithm>

/**
 * @brief The ElementType enum
 * Defines of which type the Element is.
 */
enum ElementType{
    root,
    element,
    leaf
};

enum TagType
{
    NameTag,
    NameAttributeTag,
    CommentTag,
    EndTag,
    AnyVal
};

class NoMatchException: public std::exception
{
public:
    NoMatchException( std::string tag)
    { this->tag = tag; }

  virtual const char* what() const throw()
  {
        std::string errormsg;
        errormsg += "Tag \"" + this->tag + "\" doesnt match anything";

    return errormsg.c_str(); // my error message
  }
private:
    std::string tag;
};

/**
 * @brief The XMLElement class
 * The generic XML Element, which represens a part of the
 * XML tree
 */
class XMLElement
{
public:
    XMLElement();
    ~XMLElement();


    std::string getTreeAsString();


    void setName( std::string name );

    void setRoot();
    void setElement();
    void setLeaf();

    void setNoChild();

    void setDepth( unsigned int newDepth );

    XMLElement* addElement( std::string name );
    XMLElement* addLeaf( std::string name, std::string content );
    XMLElement* getElement( unsigned int order );

    XMLElement* findNode( std::string name );
    std::string getAttribute( std::string key );

    void addAttribute( std::string attribut, std::string value );
    void addComment( std::string comment );

    TagType getTagType( std::string tag );
    static XMLElement interpreteTag(std::string tag, TagType type);
    XMLElement* readElement(std::fstream &data, int depth , std::string peekString = "");

    std::string getContent();

private:
    std::string name; /// The name of the subtree

    std::vector<XMLElement*> childs;   /// The ordered childs

    ElementType type;   /// Type of the Element

    std::map<std::string,std::string> attributes; /// Attributes of the node

    std::string comment; /// Comment before the element

    bool no_content;
    std::string content;

    unsigned int depth;
    bool noChild;

};

#endif // XMLELEMENT_HPP
