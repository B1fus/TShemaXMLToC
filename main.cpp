#include <iostream>
#include <fstream>
#include <tinyxml2.h>
#include "converter.h"
#include "paths.h"

int main(){
    // tinyxml2::XMLDocument x;
    // tinyxml2::XMLPrinter printer;
    // //std::fstream("data/1.xml", "rb");
    // if(tinyxml2::XML_SUCCESS != x.LoadFile("data/1.xml"))
    //     std::cout<<"Cannot open file\n";
    // auto node = x.FirstChild()->NextSibling()->FirstChild()
    //             ->NextSibling()->NextSibling()->NextSibling()
    //             ->NextSibling()->NextSibling()->NextSibling()
    //             ->NextSibling()->FirstChild()->NextSibling();
    
    // //std::cout<<node->ToElement()->Attribute("Name");
    // std::cout<<node->ToElement()->GetText();
    
    // node->Accept(&printer);
    // std::cout<<printer.CStr();


    // std::cout<<"\n\n\n\n";

    // ML::InputBlock* input = new ML::InputBlock(1);
    // input->setName("input1");

    // ML::GainBlock* gain = new ML::GainBlock(2, "0.03", input);
    // gain->setName("gain1");

    // ML::AddBlock* add = new ML::AddBlock(3);
    // ML::UnitDelayBlock* delay = new ML::UnitDelayBlock(4);
    // delay->setName("delay1");
    // delay->setIn(add);

    // add->setName("add1");
    // add->setIn(delay, input);
    // add->setNegative(0, 1);

    // ML::SchemaContext context("nwocg");

    // std::cout<<input->getCode(context)<<"\n";
    // std::cout<<gain->getCode(context)<<"\n";
    // std::cout<<delay->getCode(context)<<"\n";
    // std::cout<<add->getCode(context)<<"\n";
    // std::cout<<(typeid((input))==typeid(ML::InputBlock*))<<"\n";
    // std::cout<<typeid(input).name() << " " <<typeid(ML::InputBlock*).name()<<"\n";

    ML::SchemaConverter converter(std::string(SOURCE_PATH) + "/data/1.xml", "nwocg");
    converter.convert(std::string(SOURCE_PATH) + "1.c");


    return 0;
}