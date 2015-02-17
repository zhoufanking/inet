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

#ifndef __INET_IEEE80211DSSSMODE_H
#define __INET_IEEE80211DSSSMODE_H

#include "inet/physicallayer/modulation/DBPSKModulation.h"
#include "inet/physicallayer/modulation/DQPSKModulation.h"
#include "inet/physicallayer/ieee80211/mode/IIeee80211Mode.h"

namespace inet {

namespace physicallayer {

class INET_API Ieee80211DsssPreambleMode
{
  public:
    inline int getSYNCBitLength() const { return 128; }
    inline int getSFDBitLength() const { return 16; }
    inline int getBitLength() const { return getSYNCBitLength() + getSFDBitLength(); }
    inline bps getBitrate() const { return Mbps(1); }
    inline const simtime_t getDuration() const { return getBitLength() / getBitrate().get(); }

    const DBPSKModulation *getModulation() const { return &DBPSKModulation::singleton; }
};

class INET_API Ieee80211DsssHeaderMode
{
  public:
    inline int getSignalBitLength() const { return 8; }
    inline int getServiceBitLength() const { return 8; }
    inline int getLengthBitLength() const { return 16; }
    inline int getCRCBitLength() const { return 16; }
    inline int getBitLength() const { return getSignalBitLength() + getServiceBitLength() + getLengthBitLength() + getCRCBitLength(); }
    inline bps getBitrate() const { return Mbps(1); }
    inline const simtime_t getDuration() const { return getBitLength() / getBitrate().get(); }

    const DBPSKModulation *getModulation() const { return &DBPSKModulation::singleton; }
};

class INET_API Ieee80211DsssDataMode : public IIeee80211DataMode
{
  protected:
    const DPSKModulationBase *modulation;

  public:
    Ieee80211DsssDataMode(const DPSKModulationBase *modulation);

    virtual inline bps getNetBitrate() const { return Mbps(1) * modulation->getConstellationSize() / 2; }
    virtual inline bps getGrossBitrate() const { return getNetBitrate(); }
    const simtime_t getDuration(int bitLength) const;

    const DPSKModulationBase *getModulation() const { return modulation; }
};

/**
 * Represents a Direct Sequence Spread Spectrum PHY mode as described in IEEE
 * 802.11-2012 specification clause 16.
 */
class INET_API Ieee80211DsssMode : public IIeee80211Mode
{
  protected:
    const Ieee80211DsssPreambleMode *preambleMode;
    const Ieee80211DsssHeaderMode *headerMode;
    const Ieee80211DsssDataMode *dataMode;

  public:
    Ieee80211DsssMode(const Ieee80211DsssPreambleMode *preambleMode, const Ieee80211DsssHeaderMode *headerMode, const Ieee80211DsssDataMode *dataMode);

    const IIeee80211DataMode *getDataMode() const { return dataMode; }

    inline Hz getChannelSpacing() const { return MHz(5); }
    inline Hz getBandwidth() const { return MHz(22); }

    inline const simtime_t getSlotTime() const { return 20E-6; }
    inline const simtime_t getSIFSTime() const { return 10E-6; }
    inline const simtime_t getDuration(int dataBitLength) const { return preambleMode->getDuration() + headerMode->getDuration() + dataMode->getDuration(dataBitLength); }
};

/**
 * Provides the compliant Direct Sequence Spread Spectrum PHY modes as described
 * in the IEEE 802.11-2012 specification clause 16.
 */
class INET_API Ieee80211DsssCompliantModes
{
  public:
    // preamble modes
    static const Ieee80211DsssPreambleMode dsssPreambleMode1Mbps;

    // header modes
    static const Ieee80211DsssHeaderMode dsssHeaderMode1Mbps;

    // data modes
    static const Ieee80211DsssDataMode dsssDataMode1Mbps;
    static const Ieee80211DsssDataMode dsssDataMode2Mbps;

    // modes
    static const Ieee80211DsssMode dsssMode1Mbps;
    static const Ieee80211DsssMode dsssMode2Mbps;
};

} // namespace physicallayer

} // namespace inet

#endif // ifndef __INET_IEEE80211DSSSMODE_H

