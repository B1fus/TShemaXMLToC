#include <iostream>
#include <map>
#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <fstream>
#include <tinyxml2.h>
#include "xmlutils.h"

namespace ML
{

class SchemaContext;
class SchemaConverter;

class SchemaBlock{
    friend SchemaConverter;
protected:
    size_t _id;
    std::string _name;
    std::vector<SchemaBlock*> _outs;
    size_t _inCounter;
public:
    SchemaBlock(size_t id);
    virtual ~SchemaBlock() = default;
    virtual std::string getCode(SchemaContext&) = 0;
    virtual void setIn(size_t port, SchemaBlock*);
    std::vector<SchemaBlock*> getOuts() const;
    void addOut(SchemaBlock* out);
    size_t getId() const;
    void setName(const std::string name);
    std::string getName() const;
};

class InputBlock : public SchemaBlock{
public:
    InputBlock(size_t id);
    std::string getCode(SchemaContext&) override;
};

class GainBlock : public SchemaBlock{
    std::string _coef;
    SchemaBlock* _in;
public:
    GainBlock(size_t id);
    GainBlock(size_t id, std::string coef, SchemaBlock* in);
    std::string getCode(SchemaContext&) override;
    void setCoef(const std::string coef);
    void setIn(SchemaBlock* in);
    void setIn(size_t port, SchemaBlock*) override;
};

class AddBlock : public SchemaBlock{
    SchemaBlock* _in1, *_in2;
    bool _isIn1Negative, _isIn2Negative;
public:
    AddBlock(size_t id);
    std::string getCode(SchemaContext&) override;
    void setIn(SchemaBlock* in1, SchemaBlock* in2);
    void setNegative(bool is1Negative, bool is2Negative);
    void setIn(size_t port, SchemaBlock*) override;
};

class UnitDelayBlock : public SchemaBlock{
    SchemaBlock* _in;
public:
    UnitDelayBlock(size_t id);
    std::string getCode(SchemaContext&) override;
    void setIn(SchemaBlock* in);
    void setIn(size_t port, SchemaBlock*) override;
};

class OutBlock : public SchemaBlock{
    SchemaBlock* _in;
public:
    OutBlock(size_t id);
    std::string getCode(SchemaContext&) override;
    void setIn(size_t port, SchemaBlock* block) override;
    void setIn(SchemaBlock* in);
    SchemaBlock* getIn() const;
};

class SchemaContext{
    std::map<int, SchemaBlock*> _map;
    std::string _name;
public:
    SchemaContext(std::string name);
    std::string getName() const;
    void addBlock(SchemaBlock* block);
    SchemaBlock* getBlock(size_t id);
    std::map<int, SchemaBlock*>& getMap();
};

class SchemaConverter{
    SchemaContext* _context;
    std::list<std::string> _genStepList;
    std::list<std::string>::iterator _genStepInsert;
    std::vector<size_t> _deferredBlocks;
    std::vector<size_t> _inputBlocks;
    std::vector<size_t> _outBlocks;
    tinyxml2::XMLDocument _xmlDoc;
    void _stepInsert(SchemaBlock* block);
    std::pair<size_t, size_t> _parseLineInOut(const std::string& lineValue) const;
public:
    SchemaConverter(std::string XMLfilename, std::string contextName);
    void convert(std::string filename);
};

} // namespace ML
