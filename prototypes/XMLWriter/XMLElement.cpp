#include "XMLElement.hpp"

XMLElement::XMLElement()
{
    this->noChild = false;
}

XMLElement::~XMLElement()
{
    for( std::vector<XMLElement*>::iterator iter = this->childs.begin(); iter != this->childs.end(); iter++  )
    {
        delete (*iter);
    }

}

///
/// \brief XMLElement::setName Sets the name of the element
/// \param name New name of the element
///
void XMLElement::setName( std::string name )
{
    this->name = name;
}

void XMLElement::setRoot()
{
    this->type = root;
}

void XMLElement::setElement()
{
    this->type = element;
}

void XMLElement::setLeaf()
{
    this->type = leaf;
}

void XMLElement::setNoChild()
{
    this->noChild = true;
}


/**
 * @brief XMLElement::setDepth Set the Depth of the element
 * @param newDepth New Depth of the element.
 */
void XMLElement::setDepth( unsigned int newDepth )
{
    this->depth = newDepth;
}


/**
 * @brief XMLElement::addElement Add a Element
 * @param name Name of the new Element
 * @return Reference to the newly created element
 */
XMLElement* XMLElement::addElement( std::string name )
{
    XMLElement* newElement = new XMLElement();
    newElement->setName( name );
    newElement->setDepth( this->depth+1 );

    this->childs.push_back( newElement );

    return newElement;
}

/**
 * @brief XMLElement::getElement Gets a referenco from the childs
 * @param order The number of the chuld to return.
 * @return A reference to the child
 */
XMLElement* XMLElement::getElement( unsigned int order )
{
    if( (order+1) > this->childs.size() )
    {
        return NULL;
    }

    return this->childs.at( order );
}

/**
 * @brief XMLElement::addAttribute Adds an Attribute to an element
 * @param attribut The attribure name
 * @param value The value assignt to the attribute
 */
void XMLElement::addAttribute( std::string attribut, std::string value )
{
    this->attributes[attribut] = value;
}

/**
 * @brief XMLElement::addComment Adds a comment to an element.
 * @param comment The comment to add
 */
void XMLElement::addComment( std::string comment )
{
    this->comment = comment;
}

/**
 * @brief XMLElement::addLeaf Adds a leaf which can't hold children.
 * @param name NAme of the leaf
 * @param content Content of the leaf
 * @return Pointer to the leaf
 */
XMLElement* XMLElement::addLeaf( std::string name, std::string content )
{
    XMLElement* newElement = new XMLElement();
    newElement->setName( name );
    newElement->setDepth( this->depth+1 );
    newElement->setLeaf();
    newElement->content = content;

    this->childs.push_back( newElement );

    return newElement;
}

XMLElement* XMLElement::findNode( std::string name )
{
    if( name == this->name )
            return this;

    XMLElement* tmp = NULL;
    for (std::vector<XMLElement*>::iterator iter = this->childs.begin();
            iter != this->childs.end(); iter++ )
    {
        tmp = (*iter)->findNode( name );

        if( tmp != NULL )
            return tmp;
    }

    return NULL;
}

std::string XMLElement::getAttribute( std::string key )
{
    return this->attributes[key];
}


std::string XMLElement::getTreeAsString()
{
    std::string ret;

    // print actual element
    std::string tabString;
    for( int i = 1; i < this->depth; i++)
    {
        tabString += "\t";
    }

    std::string attributeString;
    for (std::map<std::string,std::string>::iterator iter = this->attributes.begin();
            iter != this->attributes.end(); iter++ )
    {
        attributeString += " " + iter->first + "=\"" + iter->second + "\"";
    }

    if( this->comment.size() > 0 )
    {
        ret += tabString + "<!-- " + this->comment + " -->\n";
    }

    ret += tabString + "<" + this->name + attributeString + (this->noChild ? "/" :"")  +">\n";

    if( this->type == leaf )
    {
            ret += tabString + "\t" + this->content + "\n";
    }
    else
    {
        for( std::vector<XMLElement*>::iterator iter = this->childs.begin(); iter != this->childs.end(); iter++  )
        {
            ret += (*iter)->getTreeAsString();
        }
    }

    if( !this->noChild )
    {
        ret += tabString + "</" + this->name + ">\n";
    }

    return ret;
}

XMLElement XMLElement::interpreteTag( std::string tag, TagType type )
{
    //std::cout << "Interprete: \"" << tag  << "\"" << std::endl;

    XMLElement ret;

    switch( type )
    {
        case NameTag:
            tag.erase(remove(tag.begin(), tag.end(), '\t'), tag.end());
            ret.name = tag.substr(1, tag.size()-2 );
            break;
        case CommentTag:
            ret.comment = tag.substr(4, tag.size() - 8 );
            break;
        case NameAttributeTag:
            {
            tag.erase(remove(tag.begin(), tag.end(), '\t'), tag.end());
            tag = tag.substr(1, tag.size()-2 );

            if( tag.at( tag.size()-1) == '/')
            {
                ret.noChild = true;
                tag = tag.substr(0, tag.size()-1 );
            }

            // Slit string by space
            bool gotName = false;

            while( tag.size() != 0 )
            {
                int spacePos = tag.find( " ", 0 );

                if( !gotName )
                {
                    std::string tmp = tag;
                    ret.name = tag.substr(0, spacePos );
                    tag = tmp.substr( spacePos +1, tag.size()-1 );

                    gotName = true;
                }
                else
                {
                    std::string attributePair = tag;
                    std::string tmp = tag;

                    if( spacePos != std::string::npos )
                    {
                        attributePair = tag.substr(0, spacePos );
                        tag = tmp.substr( spacePos +1, tag.size()-1 );
                    }
                    else
                            tag = "";

                    int equalSign = attributePair.find( "=", 0 );

                    std::string attrName = attributePair.substr(0, equalSign );
                    std::string valName = attributePair.substr(equalSign+1, attributePair.size() );
                    valName = valName.substr(1, valName.size()-2);

                    ret.attributes[attrName] = valName;
                }
            }
            }

            break;

        case AnyVal:
            tag.erase(0,tag.find_first_not_of("\t")); //führende Leerzeichen entfernen
            tag.erase(0,tag.find_first_not_of(" "));

            ret.content = tag;
            break;
    }



    return ret;
}

XMLElement* XMLElement::readElement( std::fstream &data, int depth, std::string peekString  )
{
    //std::cout << "In Depth: " << depth << std::endl;
    this->depth = depth;

    std::string tag;

    if( peekString.empty() )
    {
        char buffer[255];
        data.getline( buffer, 255 );
        tag = std::string( buffer );
        //data >> tag;
    }
    else
        tag = peekString;


    TagType type = this->getTagType( tag );

    if( type == NameTag )
    {
        this->name = XMLElement::interpreteTag( tag, type ).name;
    }
    else if( type == CommentTag )
    {
        this->comment = XMLElement::interpreteTag( tag, type ).comment;

        char buffer[255];
        data.getline( buffer, 255 );
        tag = std::string( buffer );

        this->name = XMLElement::interpreteTag( tag, NameTag ).name;
    }
    else if( type == NameAttributeTag )
    {
        XMLElement tmp = XMLElement::interpreteTag( tag, NameAttributeTag );
        this->name = tmp.name;
        this->attributes = tmp.attributes;
        this->noChild = tmp.noChild;
    }

    bool leave = false;

    if( this->noChild == true )
    {
        return this;
    }

    // Look if next line is closing this
    do
    {
        char buffer[255];
        data.getline( buffer, 255 );
        tag = std::string( buffer );

        if( getTagType(tag) == EndTag || tag.size() == 0 )
        {
            // return
            leave = true;
        }
        else if( getTagType(tag) == AnyVal )
        {
            this->content = XMLElement::interpreteTag( tag, AnyVal ).content;
            this->setLeaf();
        }
        else
        {
            XMLElement *newElement = new XMLElement();
            newElement->readElement( data, this->depth +1, tag );

            this->childs.push_back( newElement );
        }

    } while( !leave );

    //std::cout << "Leave depth: " << this->depth << std::endl;


    return this;

}

TagType XMLElement::getTagType( std::string tag )
{
    regex_t normalTag;
    regex_t normalAttributeTag;
    regex_t commentTag;
    regex_t endTag;

    /*
     *  Compiling regex
     */
    int comp = regcomp(&normalTag, "^\\s*<[A-Za-z0-9_]+[^/]>$", REG_EXTENDED);
    if( comp != 0 )
           std::cout << "Error compiling regex" << std::endl;

    comp = regcomp(&commentTag, "^\\s*<!--\\s(([A-Za-z0-9_]|\\s)+)\\s-->$", REG_EXTENDED);
    if( comp != 0 )
           std::cout << "Error compiling regex" << std::endl;

    comp = regcomp(&endTag, "^\\s*</[A-Za-z0-9_]+[^/]>$", REG_EXTENDED);
    if( comp != 0 )
           std::cout << "Error compiling regex" << std::endl;

    comp = regcomp(&normalAttributeTag, "^\\s*<[A-Za-z0-9_]+( [A-Za-z0-9_]+=\".+\")/?>$", REG_EXTENDED);
    if( comp != 0 )
           std::cout << "Error compiling regex" << std::endl;

    /*
     * Matching
     */

    int reti = regexec(&normalTag, tag.c_str(), 0, NULL, 0);
    if (!reti) {
        return NameTag;
    }
    else if (reti == REG_NOMATCH) {
    }
    else {
        char msgbuf[100];
        regerror(reti, &normalTag, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
    }

    // Match comment
    reti = regexec(&endTag, tag.c_str(), 0, NULL, 0);
    if (!reti) {
        return EndTag;
    }
    else if (reti == REG_NOMATCH) {
    }
    else {
        char msgbuf[100];
        regerror(reti, &endTag, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
    }

    // Match endTag
    reti = regexec(&commentTag, tag.c_str(), 0, NULL, 0);
    if (!reti) {
        return CommentTag;
    }
    else if (reti == REG_NOMATCH) {
    }
    else {
        char msgbuf[100];
        regerror(reti, &normalTag, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
    }

    // Match attribute
    reti = regexec(&normalAttributeTag, tag.c_str(), 0, NULL, 0);
    if (!reti) {
        return NameAttributeTag;
    }
    else if (reti == REG_NOMATCH) {
    }
    else {
        char msgbuf[100];
        regerror(reti, &normalAttributeTag, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
    }

   return AnyVal;
}

std::string XMLElement::getContent()
{
    return this->content;
}
