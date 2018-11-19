#include "sym_detect/Pattern.h"

PROJECT_NAMESPACE_BEGIN

bool Pattern::matchedType(IndexType mosId1, IndexType mosId2)
{
    return _netlist.inst(mosId1).type() == _netlist.inst(mosId2).type();
}
    
bool Pattern::matchedSize(IndexType mosId1, IndexType mosId2)
{
    if (_netlist.inst(mosId1).wid() != _netlist.inst(mosId2).wid())
        return false;
    if (_netlist.inst(mosId1).len() != _netlist.inst(mosId2).len())
        return false;
    return true;
}

bool Pattern::diffPairInput(IndexType mosId1, IndexType mosId2)
{
    if (_netlist.srcNetId(mosId1) == _netlist.srcNetId(mosId2) && 
        _netlist.gateNetId(mosId1) != _netlist.gateNetId(mosId2) && 
        _netlist.drainNetId(mosId1) != _netlist.drainNetId(mosId2) && 
        _netlist.mosType(mosId1) == MosType::DIFF && 
        _netlist.mosType(mosId2) == MosType::DIFF)
        return true;
    return false;
}

bool Pattern::diffPairCascode(IndexType mosId1, IndexType mosId2)
{
    if (_netlist.srcNetId(mosId1) != _netlist.srcNetId(mosId2) && 
        _netlist.gateNetId(mosId1) != _netlist.gateNetId(mosId2) && 
        _netlist.drainNetId(mosId1) != _netlist.drainNetId(mosId2) && 
        _netlist.mosType(mosId1) == MosType::DIFF && 
        _netlist.mosType(mosId2) == MosType::DIFF)
        return true;
    return false;
}

bool Pattern::validPairCascode(IndexType mosId1, IndexType mosId2)
{
    if (_netlist.mosType(mosId1) != MosType::DIFF && 
        _netlist.mosType(mosId1) != MosType::DIODE )
        return false;
    if (_netlist.mosType(mosId2) != MosType::DIFF && 
        _netlist.mosType(mosId2) != MosType::DIODE )
        return false;
    if (_netlist.srcNetId(mosId1) != _netlist.srcNetId(mosId2) && 
        _netlist.gateNetId(mosId1) == _netlist.gateNetId(mosId2) && 
        _netlist.drainNetId(mosId1) != _netlist.drainNetId(mosId2))
        return true;
    return false;
}

bool Pattern::validPairLoad(IndexType mosId1, IndexType mosId2)
{
    if (_netlist.mosType(mosId1) != MosType::DIFF && 
        _netlist.mosType(mosId1) != MosType::DIODE )
        return false;
    if (_netlist.mosType(mosId2) != MosType::DIFF && 
        _netlist.mosType(mosId2) != MosType::DIODE )
        return false;
    if (_netlist.srcNetId(mosId1) == _netlist.srcNetId(mosId2) && 
        _netlist.gateNetId(mosId1) == _netlist.gateNetId(mosId2) && 
        _netlist.drainNetId(mosId1) != _netlist.drainNetId(mosId2))
        return true;
    if (_netlist.mosType(mosId1) == MosType::DIODE &&
        _netlist.mosType(mosId2) == MosType::DIODE &&
        _netlist.srcNetId(mosId1) == _netlist.srcNetId(mosId2) &&
        _netlist.drainNetId(mosId1) != _netlist.drainNetId(mosId2))
        return true;
    return false;
}  

bool Pattern::crossPairCascode(IndexType mosId1, IndexType mosId2)
{
    if (_netlist.srcNetId(mosId1) != _netlist.srcNetId(mosId2) && 
        _netlist.gateNetId(mosId1) == _netlist.drainNetId(mosId2) && 
        _netlist.drainNetId(mosId1) == _netlist.gateNetId(mosId2) && 
        _netlist.mosType(mosId1) == MosType::DIFF && 
        _netlist.mosType(mosId2) == MosType::DIFF)
        return true;
    return false;
}

bool Pattern::crossPairLoad(IndexType mosId1, IndexType mosId2)
{
    if (_netlist.srcNetId(mosId1) == _netlist.srcNetId(mosId2) && 
        _netlist.gateNetId(mosId1) == _netlist.drainNetId(mosId2) && 
        _netlist.drainNetId(mosId1) == _netlist.gateNetId(mosId2) && 
        _netlist.mosType(mosId1) == MosType::DIFF && 
        _netlist.mosType(mosId2) == MosType::DIFF)
        return true;
    return false;
}

MosPattern Pattern::pattern(IndexType mosId1, IndexType mosId2)
{
    if (!matchedSize(mosId1, mosId2) || !matchedType(mosId1, mosId2))
        return MosPattern::INVALID;
    if (crossPairCascode(mosId1, mosId2))
        return MosPattern::CROSS_CASCODE;
    if (crossPairLoad(mosId1, mosId2))
        return MosPattern::CROSS_LOAD;
    if (validPairCascode(mosId1, mosId2))
        return MosPattern::CASCODE;
    if (validPairLoad(mosId1, mosId2))
        return MosPattern::LOAD;
    if (diffPairInput(mosId1, mosId2))
        return MosPattern::DIFF_SOURCE;
    if (diffPairCascode(mosId1, mosId2))
        return MosPattern::DIFF_CASCODE;
    return MosPattern::INVALID; 
}

PROJECT_NAMESPACE_END
