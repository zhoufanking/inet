cplusplus {{
#include "inet/common/Tag_m.h"
#include "inet/linklayer/common/MACAddress.h"
}}

namespace inet;

class noncobject MACAddress;
class Tag extends cObject;

//class SimpleLinkLayerControlInfo
//{
//    MACAddress src;  // can be left empty when sending
//    MACAddress dest;
//    int interfaceId;
//}

////////////////

class LinkLayerAddressTag extends Tag
{
    MACAddress src;  // can be left empty when sending
    MACAddress dest;
}

class LinkLayerAddressRequestTag extends LinkLayerAddressTag {}

class LinkLayerAddressIndicationTag extends LinkLayerAddressTag {}

////////////////

class InterfaceIdTag extends Tag
{
    int interfaceId;
}

class InterfaceIdRequestTag extends InterfaceIdTag {}

class InterfaceIdIndicationTag extends InterfaceIdTag {}

