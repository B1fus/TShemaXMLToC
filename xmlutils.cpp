#include "xmlutils.h"

namespace tinyxml2
{

std::vector<std::string> findAllValuesByAttribute(tinyxml2::XMLElement* elem, const std::string& attribute, const std::string& attributeValue){
    std::vector<std::string> results;
    auto* currNode = elem->FirstChildElement();
    while(currNode != nullptr){
        if(currNode->FindAttribute(attribute.c_str()) != nullptr)
        if(std::strcmp(currNode->Attribute(attribute.c_str()), attributeValue.c_str()) == 0){
            results.push_back(currNode->GetText());
        }
        if(currNode->FirstChildElement() != nullptr)
            currNode = currNode->FirstChildElement();
        else{
            if(currNode->NextSiblingElement() == nullptr){
                if(currNode == elem) break;
                if(currNode->Parent() != nullptr && currNode->Parent() != elem)
                    currNode = currNode->Parent()->ToElement();
                else break;
            }
            currNode = currNode->NextSiblingElement();
        }
    }
    return results;
}

std::string findValueByAttribute(tinyxml2::XMLElement* elem, const std::string& attribute, const std::string& attributeValue){
    auto* currNode = elem->FirstChildElement();
    while(currNode != nullptr){
        if(currNode->FindAttribute(attribute.c_str()) != nullptr)
        if(std::strcmp(currNode->Attribute(attribute.c_str()), attributeValue.c_str()) == 0){
            return currNode->GetText();
        }
        if(currNode->FirstChildElement() != nullptr)
            currNode = currNode->FirstChildElement();
        else{
            if(currNode->NextSiblingElement() == nullptr){
                if(currNode == elem) break;
                if(currNode->Parent() != nullptr && currNode->Parent() != elem)
                    currNode = currNode->Parent()->ToElement();
                else break;
            }
            currNode = currNode->NextSiblingElement();
        }
    }
    return "";
}

} // namespace tinyxml2


