#include <iostream>
#include <fstream>
#include <filesystem>
#include <tinyxml2.h>
#include "converter.h"
#include "paths.h"

int main(){
    std::filesystem::create_directory(std::string(SOURCE_PATH)+"/out");

    ML::SchemaConverter converter(std::string(SOURCE_PATH) + "/data/2.xml", "nwocg");
    converter.convert(std::string(SOURCE_PATH) + "/out/2.c");

    return 0;
}