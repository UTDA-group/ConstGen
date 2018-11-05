#include "db/Netlist.h"
#include <cstdio>
#include <vector>
#include <algorithm>

PROJECT_NAMESPACE_BEGIN

void Netlist::init(InitDataObject &obj)
{
    for (Netlist::InitNet & net : obj.netArray )
        _netArray.push_back( Net(net.name, net.id) );
    for (InitInstance instance : obj.instanceArray )
    {
        if (instance.type == InstanceType::PMOS || instance.type == InstanceType::NMOS)
        {
            Instance inst(instance.name, instance.type, _instanceArray.size());
            inst.addPinId(_pinArray.size());
            _netArray[instance.netIdArray.at(0)].addPinId(_pinArray.size());
            _pinArray.push_back(Pin(_pinArray.size(), inst.id(), instance.netIdArray.at(0), PinType::DRAIN));    
            inst.addPinId(_pinArray.size());
            _netArray[instance.netIdArray.at(1)].addPinId(_pinArray.size());
            _pinArray.push_back(Pin(_pinArray.size(), inst.id(), instance.netIdArray.at(1), PinType::GATE));    
            inst.addPinId(_pinArray.size());
            _netArray[instance.netIdArray.at(2)].addPinId(_pinArray.size());
            _pinArray.push_back(Pin(_pinArray.size(), inst.id(), instance.netIdArray.at(2), PinType::SOURCE));    
            inst.addPinId(_pinArray.size());
            _netArray[instance.netIdArray.at(3)].addPinId(_pinArray.size());
            _pinArray.push_back(Pin(_pinArray.size(), inst.id(), instance.netIdArray.at(3), PinType::BULK));    
            _instanceArray.push_back(inst);
        }
       else if (instance.type == InstanceType::RES || instance.type == InstanceType::CAP)
        {
            Instance inst(instance.name, instance.type, _instanceArray.size());
            inst.addPinId(_pinArray.size());
            _netArray[instance.netIdArray.at(0)].addPinId(_pinArray.size());
            _pinArray.push_back(Pin(_pinArray.size(), inst.id(), instance.netIdArray.at(0), PinType::THIS));
            inst.addPinId(_pinArray.size());
            _netArray[instance.netIdArray.at(1)].addPinId(_pinArray.size());
            _pinArray.push_back(Pin(_pinArray.size(), inst.id(), instance.netIdArray.at(1), PinType::THAT));
            inst.addPinId(_pinArray.size());
            _netArray[instance.netIdArray.at(2)].addPinId(_pinArray.size());
            _pinArray.push_back(Pin(_pinArray.size(), inst.id(), instance.netIdArray.at(2), PinType::OTHER));
            _instanceArray.push_back(inst);
        }
       else
        {
            Instance inst(instance.name, instance.type, _instanceArray.size());
            for (IndexType netId : instance.netIdArray)
            {
                inst.addPinId(_pinArray.size());
                _netArray[netId].addPinId(_pinArray.size());
                _pinArray.push_back(Pin(_pinArray.size(), inst.id(), netId, PinType::OTHER));                
            }    
            _instanceArray.push_back(inst);
        } 
    }
}

void Netlist::print_all()
{
    for (Net & net : _netArray)
    {
        std::printf("Net %d, %s \n", net.id(), net.name().c_str());
    }
    for (Instance & inst : _instanceArray)
    {
        std::printf("Instance %d, %s \n", inst.id(), inst.name().c_str()); 
        for (IndexType pinId : inst.pinIdArray())
            std::printf("Pin %d, from  Instance %s to net %d:%s \n", pinId, _instanceArray[_pinArray[pinId].instanceId()].name().c_str(), _pinArray[pinId].netId(), _netArray[_pinArray[pinId].netId()].name().c_str()); 
    }   
}

std::vector<IndexType> Netlist::pinInstanceId(IndexType pinId)
{
    std::vector<IndexType> inst;
    Net net = _netArray[_pinArray[pinId].netId()];
    for(IndexType tempPinId : net.pinIdArray())
    {
        IndexType instId = _pinArray[tempPinId].instanceId();
        if(instId == _pinArray[pinId].instanceId())
            continue;
        if(std::find(inst.begin(), inst.end(), instId) != inst.end())
            continue;
        inst.push_back(instId); 
    }
    return inst;
}

std::vector<IndexType> Netlist::pinMosfetId(IndexType pinId)
{
    std::vector<IndexType> inst = pinInstanceId(pinId);
    std::vector<IndexType> mos;
    for(IndexType instance : inst)
        if(_instanceArray[instance].type() == InstanceType::NMOS || _instanceArray[instance].type() == InstanceType::PMOS)
            mos.push_back(instance);
    return mos;
}

std::vector<IndexType> Netlist::pinInstanceId(IndexType pinId, InstanceType type)
{
    std::vector<IndexType> inst = pinInstanceId(pinId);
    std::vector<IndexType> result;
    for(IndexType instance : inst)
        if(_instanceArray[instance].type() == type)
            result.push_back(instance);
    return result;
}

std::vector<IndexType> Netlist::netMosfetId(IndexType netId, PinType pinType, MosType mosType)
{
    std::vector<IndexType> inst;
    Net net = _netArray[netId];
    for(IndexType tempPinId : net.pinIdArray())
        if(_pinArray[tempPinId].type() == pinType)
        {
        IndexType instId = _pinArray[tempPinId].instanceId();
        if(std::find(inst.begin(), inst.end(), instId) != inst.end())
            continue;
        if(Netlist::mosType(instId) == mosType)
            inst.push_back(instId); 
        }
    return inst;
}

IndexType Netlist::instanceNetId(IndexType instId, PinType type)
{
    for (IndexType pinId : _instanceArray[instId].pinIdArray())
        if (_pinArray[pinId].type() == type)
            return _pinArray[pinId].netId();
    return INDEX_TYPE_MAX;
}    

MosType Netlist::mosType(IndexType mosId)
{
    if (instanceNetId(mosId, PinType::SOURCE) == instanceNetId(mosId, PinType::DRAIN))
        return MosType::DUMMY;
    else if (instanceNetId(mosId, PinType::GATE) == instanceNetId(mosId, PinType::DRAIN))
        return MosType::DIODE;
    else if (instanceNetId(mosId, PinType::GATE) == instanceNetId(mosId, PinType::SOURCE))
        return MosType::CAP;
    return MosType::DIFF;
}

PROJECT_NAMESPACE_END


