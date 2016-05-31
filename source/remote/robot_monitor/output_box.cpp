#include "output_box.hpp"

output_box::output_box( QWidget* parent) : QTextEdit( parent )
{
    this->append("Hallo Welt");
    //this->setReadOnly( true );
}

output_box::~output_box()
{

}
