/*! @file db/Pin.cpp
    @brief Net class implementation
    @author Mingjie Liu
    @date 11/24/2018
*/
#include "db/Pin.h"

PROJECT_NAMESPACE_BEGIN

/*! @param type Querry the next search PinType.
    @see #PinType

    The DFS search for symmetry relys on 
    Pin::nextPinType to define the search path 
    direction. For example, if a Mosfet was reached
    through a source then the DFS algorithm would 
    search for connected Inst of the drain.
    Currently supported search paths:
    | Input PinType   | nextPinType   |
    | --------------- | ------------- |
    | SOURCE          | DRAIN         |
    | DRAIN           | SOURCE        |
    | THIS            | THAT          |
    | THAT            | THIS          |
*/
PinType Pin::nextPinType(PinType type) 
{
    if (type == PinType::SOURCE)
        return PinType::DRAIN;
    if (type == PinType::DRAIN)
        return PinType::SOURCE;
    if (type == PinType::THIS)
        return PinType::THAT;
    if (type == PinType::THAT)
        return PinType::THIS;
    return PinType::OTHER;
}


PROJECT_NAMESPACE_END

