#include "FileWriter.hpp"

FileWriter *FileWriter::instance = NULL;

FileWriter::FileWriter()
{

}

FileWriter::~FileWriter()
{
    closeFiles();
}

FileWriter* FileWriter::getInstance()
{
    if( FileWriter::instance == NULL )
    {
        FileWriter::instance = new FileWriter();
    }

    return FileWriter::instance;
}


void FileWriter::createFile(std::string filename)
{
    // Create File
    std::fstream *newFile = new std::fstream();
    newFile->open( filename.c_str(), std::ios_base::out );

    this->files.insert( std::pair<std::string, std::fstream*>(filename, newFile));
    //this->files[filename] = newFile;
}

std::fstream* FileWriter::getFile(std::string filename)
{
    return this->files[filename];
}

void FileWriter::closeFiles()
{
    for( std::pair<std::string, std::fstream*> iter : this->files )
    {
        iter.second->close();
        iter.second = NULL;
    }
}
