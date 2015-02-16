//
// Copyright (C) 2014 OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifndef __INET_IEEE80211FHSSMODE_H
#define __INET_IEEE80211FHSSMODE_H

#include "inet/physicallayer/base/GFSKModulationBase.h"
#include "inet/physicallayer/ieee80211/mode/IIeee80211Mode.h"

namespace inet {

namespace physicallayer {

class INET_API Ieee80211FhssPreambleMode
{
  public:
    inline int getSYNCBitLength() const { return 80; }
    inline int getSFDBitLength() const { return 16; }
    inline int getBitLength() const { return getSYNCBitLength() + getSFDBitLength(); }
    inline bps getBitrate() const { return Mbps(1); }
    inline const simtime_t getDuration() const { return getBitLength() / getBitrate().get(); }
};

class INET_API Ieee80211FhssHeaderMode
{
  public:
    inline int getPLWBitLength() const { return 12; }
    inline int getPSFBitLength() const { return 4; }
    inline int getHECBitLength() const { return 16; }
    inline int getBitLength() const { return getPLWBitLength() + getPSFBitLength() + getHECBitLength(); }
    inline bps getBitrate() const { return Mbps(1); }
    inline const simtime_t getDuration() const { return getBitLength() / getBitrate().get(); }
};

class INET_API Ieee80211FhssDataMode
{
  protected:
    const GFSKModulationBase *modulation;

  public:
    Ieee80211FhssDataMode(const GFSKModulationBase *modulation);

    inline bps getBitrate() const { return Mbps(1) * modulation->getConstellationSize(); }
    inline const simtime_t getDuration(int bitLength) const { return bitLength / getBitrate().get(); }

    const GFSKModulationBase *getModulation() const { return modulation; }
};

/**
 * Represents a Frequency-Hopping Spread Spectrum PHY mode as described in IEEE
 * 802.11-2012 specification clause 14.
 */
class INET_API Ieee80211FhssMode : public IIeee80211Mode
{
  protected:
    const Ieee80211FhssPreambleMode *preambleMode;
    const Ieee80211FhssHeaderMode *headerMode;
    const Ieee80211FhssDataMode *dataMode;

  public:
    Ieee80211FhssMode(const Ieee80211FhssPreambleMode *preambleMode, const Ieee80211FhssHeaderMode *headerMode, const Ieee80211FhssDataMode *dataMode);

    inline const simtime_t getDuration(int dataBitLength) const { return preambleMode->getDuration() + headerMode->getDuration() + dataMode->getDuration(dataBitLength); }
};

/**
 * Provides the compliant Frequency-Hopping Spread Spectrum PHY modes as described
 * in the IEEE 802.11-2012 specification clause 14.
 */
class INET_API Ieee80211FhssCompliantModes
{
  public:
    // preamble modes
    static const Ieee80211FhssPreambleMode fhssPreambleMode1Mbps;

    // header modes
    static const Ieee80211FhssHeaderMode fhssHeaderMode1Mbps;

    // data modes
    static const Ieee80211FhssDataMode fhssDataMode1Mbps;
    static const Ieee80211FhssDataMode fhssDataMode2Mbps;

    // modes
    static const Ieee80211FhssMode fhssMode1Mbps;
    static const Ieee80211FhssMode fhssMode2Mbps;
};

} // namespace physicallayer

} // namespace inet

#endif // ifndef __INET_IEEE80211FHSSMODE_H

