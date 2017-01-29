#include <fstream>
#include <map>
#include <string>

class FileWriter
{
public:
    FileWriter();
    ~FileWriter();

    static FileWriter* getInstance();
    static FileWriter *instance;

    void createFile( std::string filename );
    std::fstream *getFile( std::string filename );

    void closeFiles();


private:
    std::map<std::string, std::fstream*> files;





};
