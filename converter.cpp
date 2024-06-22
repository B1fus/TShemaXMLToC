#include "converter.h"

namespace ML
{

SchemaBlock::SchemaBlock(size_t id): _id(id), _name("") {
    _inCounter = 0;
}

void SchemaBlock::setIn(size_t port, SchemaBlock *){}

std::vector<SchemaBlock *> SchemaBlock::getOuts() const
{
    return _outs;
}

void SchemaBlock::addOut(SchemaBlock *out)
{
    _outs.push_back(out);
}

size_t SchemaBlock::getId() const
{
    return _id;
}

void SchemaBlock::setName(const std::string name){
    _name = name;
    std::replace(_name.begin(), _name.end(), ' ', '_');
}

std::string SchemaBlock::getName() const{
    return _name;
}

InputBlock::InputBlock(size_t id):SchemaBlock(id){
    _inCounter = 1;
}

std::string InputBlock::getCode(SchemaContext &context){
    return "";
}

GainBlock::GainBlock(size_t id):SchemaBlock(id){
    _inCounter = 1;
    _coef = "1";
}

GainBlock::GainBlock(size_t id, std::string coef, SchemaBlock *in)
    : SchemaBlock(id), _coef(coef), _in(in) {
    _inCounter = 1;
}

std::string GainBlock::getCode(SchemaContext& context){
    std::string ctxName = context.getName() + ".";
    return ctxName + _name + " = " + ctxName + _in->getName() + " * " + _coef + ";";
}

void GainBlock::setCoef(const std::string coef){
    _coef = coef;
}

void GainBlock::setIn(SchemaBlock* in){
    _in = in;
}

void GainBlock::setIn(size_t port, SchemaBlock* block){
    if(port == 1) _in = block;
}

SchemaContext::SchemaContext(std::string name):_name(name){}

std::string SchemaContext::getName() const{
    return _name;
}

void SchemaContext::addBlock(SchemaBlock* block){
    _map[block->getId()] = block;
}

SchemaBlock *SchemaContext::getBlock(size_t id){
    return _map[id];
}

std::map<int, SchemaBlock *> &SchemaContext::getMap(){
    return _map;
}

void SchemaContext::clear(){
    for(auto& i : _map){
        delete i.second;
    }
    _map.clear();
}

SchemaContext::~SchemaContext(){
    clear();
}

AddBlock::AddBlock(size_t id):SchemaBlock(id){
    _inCounter = 2;
    _isIn1Negative = _isIn2Negative = 0;
}

std::string AddBlock::getCode(SchemaContext& context){
    std::string ctxName = context.getName() + ".";
    return ctxName + _name + " = " 
           + (_isIn1Negative ? "-" : "") + ctxName + _in1->getName() +
           + (_isIn2Negative ? " - " : " + ") + ctxName + _in2->getName() + ";";
}

void AddBlock::setIn(SchemaBlock *in1, SchemaBlock *in2){
    _in1 = in1;
    _in2 = in2;
}

void AddBlock::setNegative(bool is1Negative, bool is2Negative){
    _isIn1Negative = is1Negative;
    _isIn2Negative = is2Negative;
}

void AddBlock::setIn(size_t port, SchemaBlock* block){
    if(port == 1) _in1 = block;
    else if(port == 2) _in2 = block;
}

UnitDelayBlock::UnitDelayBlock(size_t id):SchemaBlock(id){
    _inCounter = 1;
}

std::string UnitDelayBlock::getCode(SchemaContext& context){
    std::string ctxName = context.getName() + ".";
    return ctxName + _name + " = " + ctxName + _in->getName() + ";";
}

void UnitDelayBlock::setIn(SchemaBlock *in){
    _in = in;
}

void UnitDelayBlock::setIn(size_t port, SchemaBlock* block){
    if(port == 1) _in = block;
}

OutBlock::OutBlock(size_t id):SchemaBlock(id){
    _inCounter = 1;
}

std::string OutBlock::getCode(SchemaContext &){
    return "";
}

void OutBlock::setIn(size_t port, SchemaBlock* block){
    if(port == 1) _in = block;
}

void OutBlock::setIn(SchemaBlock *in){
    _in = in;
}

SchemaBlock *OutBlock::getIn() const{
    return _in;
}

void SchemaConverter::_stepInsert(SchemaBlock* block){
    if(block->_inCounter == 0) return;

    block->_inCounter--;
    if(block->_inCounter == 0){
        _genStepList.insert(_genStepInsert, block->getCode(*_context));
        for(auto* i : block->_outs){
            _stepInsert(i);
        }
    }
}

std::pair<size_t, size_t> SchemaConverter::_parseLineInOut(const std::string &lineValue) const{
    std::pair<size_t, size_t> result;
    
    std::string tmp{};
    bool isSecond = 0;

    for(size_t i = 0; i<lineValue.size(); i++){
        if(lineValue[i] >= '0' && lineValue[i] <= '9'){
            tmp += lineValue[i];
        }
        else if(!tmp.empty()){
            if(isSecond) result.second = std::stoul(tmp);
            else result.first = std::stoul(tmp);
            isSecond = !isSecond;
            tmp = "";
        }
    }
    if(!tmp.empty()){
        if(isSecond) result.second = std::stoul(tmp);
        else result.first = std::stoul(tmp);
    }
    return result;
}

void SchemaConverter::_initBlocksFromXML(){
    auto currNode = _xmlDoc.FirstChild()->NextSibling()->FirstChild();
    while(currNode != nullptr){
        auto elem = currNode->ToElement();
        if(std::strcmp(elem->Name(), "Block") == 0){
            //init blocks
            std::string type = elem->Attribute("BlockType");
            size_t id = std::stoul(elem->Attribute("SID"));
            ML::SchemaBlock* addedBlock;

            if(!_createBlockFunctions.contains(type)){
                std::cout<<"Error: Can't find function of block by \'"<<type<<"\' type.\n";
            }
            else{
                addedBlock = _createBlockFunctions[type](id, elem, this);
                addedBlock->setName(elem->Attribute("Name"));
                _context->addBlock(addedBlock);
            }
        }
        else if(std::strcmp(elem->Name(), "Line") == 0){
            //connect blocks
            auto src = tinyxml2::findAllValuesByAttribute(elem, "Name", "Src");
            auto dst = tinyxml2::findAllValuesByAttribute(elem, "Name", "Dst");

            auto srcPair = _parseLineInOut(src[0]);
            SchemaBlock* srcBlock = _context->getBlock(srcPair.first);
            for(size_t i = 0; i<dst.size(); i++){
                auto dstPair = _parseLineInOut(dst[i]);
                SchemaBlock* dstblock = _context->getBlock(dstPair.first);

                srcBlock->addOut(dstblock);
                dstblock->setIn(dstPair.second, srcBlock);
            }
        }

        currNode = currNode->NextSibling();
    }
    std::sort(_outBlocks.begin(), _outBlocks.end());
}

void SchemaConverter::_generateStepList(){
    //deferrend blocks are inserting in the end of generate function
    _genStepInsert = _genStepList.begin();
    for(auto i: _deferredBlocks){
        _stepInsert(_context->getBlock(i));
    }

    _genStepInsert = _genStepList.begin();
    for(auto i: _inputBlocks){
        _stepInsert(_context->getBlock(i));
    }
}

void SchemaConverter::_createCFile(const std::string &filename) const{
    std::ofstream outFile(filename);
    std::string ctxName = _context->getName();
    outFile<<"#include \"" << ctxName <<"_run.h\"\n";
    outFile<<"#include <math.h>\n\n";

    outFile<<"static struct{\n";
    for(const auto& i: _context->getMap()){
        //dont write out variable to file
        auto lower = std::lower_bound(_outBlocks.begin(), _outBlocks.end(), i.second->getId());
        if(lower == _outBlocks.end() || *lower != i.second->getId())
        outFile<<"\tdouble "<<i.second->getName()<<";\n";
    }
    outFile<<"} " <<ctxName<<";\n\n";

    outFile<<"void " <<ctxName<<"_generated_init(){\n";
    for(const auto i: _deferredBlocks){
        outFile<< "\t" << ctxName<<"."<<_context->getBlock(i)->getName()<<" = 0;\n";
    }
    outFile<<"}\n\n";

    outFile<<"void " <<ctxName<<"_generated_step(){\n";
    for(const auto& i: _genStepList){
        if(!i.empty()) outFile<< "\t" << i << "\n";
    }
    outFile<<"}\n\n";

    outFile<<"static const "<<ctxName<<"_ExtPort ext_ports[] = {\n";
    for(const auto& i: _outBlocks){
        OutBlock* block= dynamic_cast<OutBlock*>(_context->getBlock(i));
        outFile<<"\t { \""<<block->getName()<<"\", &"
               <<ctxName<<"."<<block->getIn()->getName()<<", 0 },\n";
    }
    for(const auto& i: _inputBlocks){
        SchemaBlock* block= _context->getBlock(i);
        outFile<<"\t { \""<<block->getName()<<"\", &"
               <<ctxName<<"."<<block->getName()<<", 1 },\n";
    }
    outFile<<"};\n\n";

    outFile<<"const "<<ctxName<<"_ExtPort * const "<<ctxName<<"_generated_ext_ports = ext_ports;\n"
           <<"const size_t "<<ctxName<<"_generated_ext_ports_size = sizeof(ext_ports);\n";

    outFile.close();
}

void SchemaConverter::_clear(){
    _genStepList.clear();
    _deferredBlocks.clear();
    _inputBlocks.clear();
    _outBlocks.clear();
}

SchemaConverter::SchemaConverter(const std::string& XMLfilename, const std::string& contextName){
    std::call_once(_createBlockFunctionsFlag, _initCreateBlockFunctions);
    _context = new SchemaContext(contextName);
    if(tinyxml2::XML_SUCCESS != _xmlDoc.LoadFile(XMLfilename.c_str()))
        std::cout<<"Cannot open file\n";
}

void SchemaConverter::convert(const std::string& filename){
    _initBlocksFromXML();
    _generateStepList();
    _createCFile(filename);
}

void SchemaConverter::openXMLFile(const std::string &filename){
    if(tinyxml2::XML_SUCCESS != _xmlDoc.LoadFile(filename.c_str()))
        std::cout<<"Cannot open file\n";
}

void SchemaConverter::setContextName(const std::string &name){
    if(_context != nullptr) delete _context;
    _context = new SchemaContext(name);
}

SchemaConverter::~SchemaConverter(){
    _clear();
}

std::unordered_map<
                std::string,
                std::function<SchemaBlock*(size_t, tinyxml2::XMLElement*, SchemaConverter*)>
> SchemaConverter::_createBlockFunctions{};

void SchemaConverter::_initCreateBlockFunctions(){
    _createBlockFunctions["Inport"] = 
    [](size_t id, tinyxml2::XMLElement* elem, SchemaConverter* converter){
        converter->_inputBlocks.push_back(id);
        return new ML::InputBlock(id);
    };

    _createBlockFunctions["Sum"] =
    [](size_t id, tinyxml2::XMLElement* elem, SchemaConverter* converter){
        auto* sumBlock = new ML::AddBlock(id);
        std::string inputs = tinyxml2::findValueByAttribute(elem, "Name", "Inputs");
        if(!inputs.empty()){
            sumBlock->setNegative(inputs[0] == '-', inputs[1] == '-');
        }
        return sumBlock;
    };

    _createBlockFunctions["Gain"] =
    [](size_t id, tinyxml2::XMLElement* elem, SchemaConverter* converter){
        auto* gainBlock = new ML::GainBlock(id);
        std::string gain = tinyxml2::findValueByAttribute(elem, "Name", "Gain");
        if(!gain.empty()) gainBlock->setCoef(gain);
        return gainBlock;
    };

    _createBlockFunctions["UnitDelay"] =
    [](size_t id, tinyxml2::XMLElement* elem, SchemaConverter* converter){
        converter->_deferredBlocks.push_back(id);
        return new ML::UnitDelayBlock(id);
    };

    _createBlockFunctions["Outport"] =
    [](size_t id, tinyxml2::XMLElement* elem, SchemaConverter* converter){
        converter->_outBlocks.push_back(id);
        return new ML::OutBlock(id);
    };
}

}; // namespace ML