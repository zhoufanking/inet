cplusplus {{
    #include "inet/linklayer/common/MACAddress.h"
}}

namespace inet;

class noncobject MACAddress;

//class SimpleLinkLayerControlInfo
//{
//    MACAddress src;  // can be left empty when sending
//    MACAddress dest;
//    int interfaceId;
//}

////////////////

class LinkLayerAddressTag
{
    MACAddress src;  // can be left empty when sending
    MACAddress dest;
}

class LinkLayerAddressRequestTag extends LinkLayerAddressTag {}

class LinkLayerAddressIndicationTag extends LinkLayerAddressTag {}

////////////////

class InterfaceIdTag
{
    int interfaceId;
}

class InterfaceIdRequestTag extends InterfaceIdTag {}

class InterfaceIdIndicationTag extends InterfaceIdTag {}

