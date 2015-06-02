/*
 * Copyright (C) 2004 Andras Varga
 * Copyright (C) 2014 OpenSim Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __INET_IARP_H
#define __INET_IARP_H

#include "inet/common/INETDefs.h"

#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/contract/ipv4/IPv4Address.h"
#include "inet/linklayer/common/MACAddress.h"
#include "inet/common/ModuleAccess.h"

namespace inet {

class InterfaceEntry;

/**
 * Represents an IPv4 ARP module.
 */
class INET_API IARP
{
  public:
    /**
     * Sent in ARP cache change notification signals
     */
    class Notification : public cObject
    {
      public:
        L3Address l3Address;
        MACAddress macAddress;
        const InterfaceEntry *ie;

      public:
        Notification(L3Address l3Address, MACAddress macAddress, const InterfaceEntry *ie)
            : l3Address(l3Address), macAddress(macAddress), ie(ie) {}
    };

    class CallbackInterface
    {
      public:
        virtual void arpResolutionInitiated(L3Address l3Address) = 0;
        virtual void arpResolutionCompleted(L3Address l3Address, MACAddress macAddress, const InterfaceEntry *ie) = 0;
        virtual void arpResolutionFailed(L3Address l3Address, const InterfaceEntry *ie) = 0;
    };

    /** @brief Signals used to publish ARP state changes. */
    static const simsignal_t initiatedARPResolutionSignal;
    static const simsignal_t completedARPResolutionSignal;
    static const simsignal_t failedARPResolutionSignal;

  protected:
    CallbackInterface *cb = nullptr;

  public:
    virtual ~IARP() {}

    /**
     * Returns the Layer 3 address for the given MAC address. If it is not available
     * (not in the cache, pending resolution, or already expired), UNSPECIFIED_ADDRESS
     * is returned.
     */
    virtual L3Address getL3AddressFor(const MACAddress&) const = 0;

    /**
     * Tries to resolve the given network address to a MAC address. If the MAC
     * address is not yet resolved it returns an unspecified address and starts
     * an address resolution procedure. A signal is emitted when the address
     * resolution procedure terminates.
     */
    virtual MACAddress resolveL3Address(const L3Address& address, const InterfaceEntry *ie) = 0;

    /**
     * Sets a callback object, to be used when ARP state changes.
     * This callback object may be your simple module itself (if it multiply inherits
     * from CallbackInterface too, that is you declared it as
     * <pre>
     * class MyModule : public cSimpleModule, public IARP::CallbackInterface
     * </pre>
     * and redefined the necessary virtual functions; or you may use
     * dedicated class (and objects) for this purpose.
     *
     * ARP doesn't delete the callback object in the destructor
     * or on any other occasion.
     */
    void setCallbackObject(CallbackInterface *cb);
};

} // namespace inet

#endif // ifndef __INET_IARP_H

