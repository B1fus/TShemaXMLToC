#include <tinyxml2.h>
#include <vector>
#include <string>

namespace tinyxml2
{

std::vector<std::string> findAllValuesByAttribute(XMLElement* elem, const std::string& attribute, const std::string& attributeValue);
std::string findValueByAttribute(tinyxml2::XMLElement* elem, const std::string& attribute, const std::string& attributeValue);
} // namespace tinyxml2


