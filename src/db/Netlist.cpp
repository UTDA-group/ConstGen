#include "db/Netlist.h"
#include <vector>
#include <algorithm>

PROJECT_NAMESPACE_BEGIN

static const PinType MOS_PIN_TYPE[4] = {PinType::DRAIN, PinType::GATE, PinType::SOURCE, PinType::BULK};
static const PinType RES_PIN_TYPE[3] = {PinType::THIS, PinType::THAT, PinType::OTHER};

bool Netlist::isMos(InstType instType) const
{
    return instType == InstType::NMOS || instType == InstType::PMOS;  
}

bool Netlist::isPasvDev(InstType instType) const
{
    return instType == InstType::NMOS || instType == InstType::PMOS;  
}

void Netlist::init(InitDataObj & obj)
{
    for (InitNet & net : obj.netArray)
        _netArray.emplace_back(net.name, net.id);
    for (InitInst & inInst : obj.instArray)
    {
        if (isMos(inInst.type))
        {
            Inst inst(inInst.name, inInst.type, _instArray.size(), inInst.wid, inInst.len);
            for (IndexType i = 0; i < 4; i++)
            {
                inst.addPinId(_pinArray.size());
                _netArray[inInst.netIdArray.at(i)].addPinId(_pinArray.size());
                _pinArray.emplace_back(_pinArray.size(), inst.id(), inInst.netIdArray.at(i), MOS_PIN_TYPE[i]);    
            }
            _instArray.push_back(inst);
        }
        else if (isPasvDev(inInst.type))
        {
            Inst inst(inInst.name, inInst.type, _instArray.size(), inInst.wid, inInst.len);
            for (IndexType i = 0; i < 3; i++)
            {
                inst.addPinId(_pinArray.size());
                _netArray[inInst.netIdArray.at(i)].addPinId(_pinArray.size());
                _pinArray.emplace_back(_pinArray.size(), inst.id(), inInst.netIdArray.at(i), RES_PIN_TYPE[i]);
            }
            _instArray.push_back(inst);
        }
        else
        {
            Inst inst(inInst.name, inInst.type, _instArray.size());
            for (IndexType netId : inInst.netIdArray)
            {
                inst.addPinId(_pinArray.size());
                _netArray[netId].addPinId(_pinArray.size());
                _pinArray.emplace_back(_pinArray.size(), inst.id(), netId, PinType::OTHER);                
            }    
            _instArray.push_back(inst);
        } 
    }
}

void Netlist::print_all() const
{
    for (const Net & net : _netArray)
    {
        std::printf("Net %d, %s \n", net.id(), net.name().c_str());
    }
    for (const Inst & inst : _instArray)
    {
        std::printf("Instance %d, %s \n", inst.id(), inst.name().c_str()); 
        for (IndexType pinId : inst.pinIdArray())
            std::printf("Pin %d, from  Instance %s to net %d:%s \n",
                pinId, _instArray[_pinArray[pinId].instId()].name().c_str(),
                _pinArray[pinId].netId(), _netArray[_pinArray[pinId].netId()].name().c_str()); 
    }   
}

void Netlist::getInstNetConn(std::vector<IndexType> & instArray, IndexType netId) const
{
    instArray.clear();
    for(IndexType tempPinId : _netArray[netId].pinIdArray())
    {
        IndexType instId = _pinArray[tempPinId].instId();
        if(std::find(instArray.begin(), instArray.end(), instId) == instArray.end())
        {
            instArray.push_back(instId); 
        }
    }
}

void Netlist::rmvInstHasPin(std::vector<IndexType> & instArray, IndexType pinId) const
{
    auto it = instArray.begin();
    while (it != instArray.end())
    {
        IndexType instId = *it;
        if (_pinArray[pinId].instId() == instId)
            it = instArray.erase(it);
        else
            ++it;
    }
}

void Netlist::getInstPinConn(std::vector<IndexType> & instArray, IndexType pinId) const
{
    IndexType netId = _pinArray[pinId].netId();
    getInstNetConn(instArray, netId);
    rmvInstHasPin(instArray, pinId);
}

void Netlist::fltrInstNetConnPinType(std::vector<IndexType> & instArray, IndexType netId, PinType connPinType) const
{
    auto it = instArray.begin();
    while (it != instArray.end())
    {
        IndexType instId = *it;
        if (instNetId(instId, connPinType) != netId)
            it = instArray.erase(it);
        else
            ++it;
    }
}

void Netlist::fltrInstPinConnPinType(std::vector<IndexType> & instArray, IndexType pinId, PinType connPinType) const
{
    fltrInstNetConnPinType(instArray, _pinArray[pinId].netId(), connPinType);
}

void Netlist::fltrInstMosType(std::vector<IndexType> & instArray, MosType mosType) const
{
    auto it = instArray.begin();
    while (it != instArray.end())
    {
        IndexType instId = *it; 
        if (Netlist::mosType(instId) != mosType)
            it = instArray.erase(it);
        else
            ++it;
    }
}

PinType Netlist::getPinTypeInstNetConn(IndexType instId, IndexType netId) const
{
    for (IndexType instPinId : _instArray[instId].pinIdArray())
        if (_pinArray[instPinId].netId() == netId)
            return _pinArray[instPinId].type();
    return PinType::OTHER;
}

PinType Netlist::getPinTypeInstPinConn(IndexType instId, IndexType pinId) const
{
    return getPinTypeInstNetConn(instId, _pinArray[pinId].netId());
}

IndexType Netlist::instNetId(IndexType instId, PinType type) const
{
    for (IndexType pinId : _instArray[instId].pinIdArray())
        if (_pinArray[pinId].type() == type)
            return _pinArray[pinId].netId();
    return INDEX_TYPE_MAX;
}    

MosType Netlist::mosType(IndexType mosId) const
{
    if (instNetId(mosId, PinType::SOURCE) == instNetId(mosId, PinType::DRAIN))
        return MosType::DUMMY;
    else if (instNetId(mosId, PinType::GATE) == instNetId(mosId, PinType::DRAIN))
        return MosType::DIODE;
    else if (instNetId(mosId, PinType::GATE) == instNetId(mosId, PinType::SOURCE))
        return MosType::CAP;
    return MosType::DIFF;
}

IndexType Netlist::instPinId(IndexType instId, PinType pinType) const
{
    for (IndexType pinId : _instArray[instId].pinIdArray())
        if (_pinArray[pinId].type() == pinType)
            return pinId;
    return INDEX_TYPE_MAX;
}   


PROJECT_NAMESPACE_END


