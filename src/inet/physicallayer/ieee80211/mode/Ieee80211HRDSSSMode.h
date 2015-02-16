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

#ifndef __INET_IEEE80211HRDSSSMODE_H
#define __INET_IEEE80211HRDSSSMODE_H

#include "inet/physicallayer/modulation/DBPSKModulation.h"
#include "inet/physicallayer/modulation/DQPSKModulation.h"
#include "inet/physicallayer/ieee80211/mode/IIeee80211Mode.h"

namespace inet {

namespace physicallayer {

enum Ieee80211HrDsssPreambleType
{
    IEEE80211_HRDSSS_PREAMBLE_TYPE_SHORT,
    IEEE80211_HRDSSS_PREAMBLE_TYPE_LONG,
};

class INET_API Ieee80211HrDsssPreambleMode
{
  protected:
    const Ieee80211HrDsssPreambleType preambleType;

  public:
    Ieee80211HrDsssPreambleMode(const Ieee80211HrDsssPreambleType preambleType);

    inline int getSyncBitLength() const { return preambleType == IEEE80211_HRDSSS_PREAMBLE_TYPE_SHORT ? 72 : 128; }
    inline int getSFDBitLength() const { return 16; }
    inline int getBitLength() const { return getSyncBitLength() + getSFDBitLength(); }
    inline bps getBitrate() const { return Mbps(1); }
    inline const simtime_t getDuration() const { return getBitLength() / getBitrate().get(); }

    const DBPSKModulation *getModulation() const { return &DBPSKModulation::singleton; }
};

class INET_API Ieee80211HrDsssHeaderMode
{
  protected:
    const Ieee80211HrDsssPreambleType preambleType;

  public:
    Ieee80211HrDsssHeaderMode(const Ieee80211HrDsssPreambleType preambleType);

    inline int getSignalBitLength() const { return 8; }
    inline int getServiceBitLength() const { return 8; }
    inline int getLengthBitLength() const { return 16; }
    inline int getCRCBitLength() const { return 16; }
    inline int getBitLength() const { return getSignalBitLength() + getServiceBitLength() + getLengthBitLength() + getCRCBitLength(); }
    inline bps getBitrate() const { return Mbps(2); }
    inline int getDuration() const { return getBitLength() / getBitrate().get(); }

    const DPSKModulationBase *getModulation() const { return preambleType == IEEE80211_HRDSSS_PREAMBLE_TYPE_SHORT ? static_cast<const DPSKModulationBase *>(&DQPSKModulation::singleton) : static_cast<const DPSKModulationBase *>(&DBPSKModulation::singleton); }
};

class INET_API Ieee80211HrDsssDataMode
{
  protected:
    const bps bitrate;

  public:
    Ieee80211HrDsssDataMode(bps bitrate);

    inline bps getBitrate() const { return bitrate; }
    inline const simtime_t getDuration(int bitLength) const { return bitLength / getBitrate().get(); }
};

/**
 * Represents a High Rate Direct Sequence Spread Spectrum PHY mode as described
 * in the IEEE 802.11-2012 specification clause 17.
 */
class INET_API Ieee80211HrDsssMode : public IIeee80211Mode
{
  protected:
    const Ieee80211HrDsssPreambleMode *preambleMode;
    const Ieee80211HrDsssHeaderMode *headerMode;
    const Ieee80211HrDsssDataMode *dataMode;

  public:
    Ieee80211HrDsssMode(const Ieee80211HrDsssPreambleMode *preambleMode, const Ieee80211HrDsssHeaderMode *headerMode, const Ieee80211HrDsssDataMode *dataMode);

    inline Hz getChannelSpacing() const { return MHz(5); }
    inline Hz getBandwidth() const { return MHz(22); }

    inline const simtime_t getSlotTime() const { return 20E-6; }
    inline const simtime_t getSIFSTime() const { return 10E-6; }

    inline const simtime_t getDuration(int dataBitLength) const { return preambleMode->getDuration() + headerMode->getDuration() + dataMode->getDuration(dataBitLength); }
};

/**
 * Provides the compliant High Rate Direct Sequence Spread Spectrum PHY modes as
 * described in the IEEE 802.11-2012 specification clause 17.
 */
class INET_API Ieee80211HrDsssCompliantModes
{
  public:
    // preamble modes
    static const Ieee80211HrDsssPreambleMode hrDsssPreambleMode1MbpsLongPreamble;
    static const Ieee80211HrDsssPreambleMode hrDsssPreambleMode1MbpsShortPreamble;

    // header modes
    static const Ieee80211HrDsssHeaderMode hrDsssHeaderMode1MbpsLongPreamble;
    static const Ieee80211HrDsssHeaderMode hrDsssHeaderMode2MbpsShortPreamble;

    // data modes
    static const Ieee80211HrDsssDataMode hrDsssDataMode1MbpsLongPreamble;

    static const Ieee80211HrDsssDataMode hrDsssDataMode2MbpsLongPreamble;
    static const Ieee80211HrDsssDataMode hrDsssDataMode2MbpsShortPreamble;

    static const Ieee80211HrDsssDataMode hrDsssDataMode5_5MbpsCckLongPreamble;
    static const Ieee80211HrDsssDataMode hrDsssDataMode5_5MbpsPbccLongPreamble;
    static const Ieee80211HrDsssDataMode hrDsssDataMode5_5MbpsCckShortPreamble;

    static const Ieee80211HrDsssDataMode hrDsssDataMode11MbpsCckLongPreamble;
    static const Ieee80211HrDsssDataMode hrDsssDataMode11MbpsPbccLongPreamble;
    static const Ieee80211HrDsssDataMode hrDsssDataMode11MbpsCckShortPreamble;
    static const Ieee80211HrDsssDataMode hrDsssDataMode11MbpsPbccShortPreamble;

    // modes
    static const Ieee80211HrDsssMode hrDsssMode1MbpsLongPreamble;

    static const Ieee80211HrDsssMode hrDsssMode2MbpsLongPreamble;
    static const Ieee80211HrDsssMode hrDsssMode2MbpsShortPreamble;

    static const Ieee80211HrDsssMode hrDsssMode5_5MbpsCckLongPreamble;
    static const Ieee80211HrDsssMode hrDsssMode5_5MbpsPbccLongPreamble;
    static const Ieee80211HrDsssMode hrDsssMode5_5MbpsCckShortPreamble;

    static const Ieee80211HrDsssMode hrDsssMode11MbpsCckLongPreamble;
    static const Ieee80211HrDsssMode hrDsssMode11MbpsPbccLongPreamble;
    static const Ieee80211HrDsssMode hrDsssMode11MbpsCckShortPreamble;
    static const Ieee80211HrDsssMode hrDsssMode11MbpsPbccShortPreamble;
};

} // namespace physicallayer

} // namespace inet

#endif // ifndef __INET_IEEE80211HRDSSSMODE_H

