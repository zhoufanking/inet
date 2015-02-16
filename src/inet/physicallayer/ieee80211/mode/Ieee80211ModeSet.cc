// Copyright (C) 2012 OpenSim Ltd
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

#include "inet/physicallayer/ieee80211/mode/Ieee80211ModeSet.h"
#include "inet/physicallayer/ieee80211/mode/Ieee80211FHSSMode.h"
#include "inet/physicallayer/ieee80211/mode/Ieee80211IRMode.h"
#include "inet/physicallayer/ieee80211/mode/Ieee80211DSSSMode.h"
#include "inet/physicallayer/ieee80211/mode/Ieee80211HRDSSSMode.h"
#include "inet/physicallayer/ieee80211/mode/Ieee80211OFDMMode.h"

namespace inet {

namespace physicallayer {

const std::vector<Ieee80211ModeSet> Ieee80211ModeSet::modeSets = {
    Ieee80211ModeSet('a', {
//        {true, Ieee80211PhyMode::GetOfdmRate6Mbps()},
//        {false, Ieee80211PhyMode::GetOfdmRate9Mbps()},
//        {true, Ieee80211PhyMode::GetOfdmRate12Mbps()},
//        {false, Ieee80211PhyMode::GetOfdmRate18Mbps()},
//        {true, Ieee80211PhyMode::GetOfdmRate24Mbps()},
//        {false, Ieee80211PhyMode::GetOfdmRate36Mbps()},
//        {false, Ieee80211PhyMode::GetOfdmRate48Mbps()},
//        {false, Ieee80211PhyMode::GetOfdmRate54Mbps()},
    }),
    Ieee80211ModeSet('b', {
        {true, &Ieee80211DsssCompliantModes::dsssMode1Mbps},
        {true, &Ieee80211DsssCompliantModes::dsssMode2Mbps},
        {true, &Ieee80211HrDsssCompliantModes::hrDsssMode5_5MbpsCckShortPreamble},
        {true, &Ieee80211HrDsssCompliantModes::hrDsssMode11MbpsCckShortPreamble},
    }),
    Ieee80211ModeSet('g', {
        {true, &Ieee80211DsssCompliantModes::dsssMode1Mbps},
        {true, &Ieee80211DsssCompliantModes::dsssMode2Mbps},
        {true, &Ieee80211HrDsssCompliantModes::hrDsssMode5_5MbpsCckShortPreamble},
//        {true, Ieee80211PhyMode::GetErpOfdmRate6Mbps()},
//        {false, Ieee80211PhyMode::GetErpOfdmRate9Mbps()},
        {true, &Ieee80211HrDsssCompliantModes::hrDsssMode11MbpsCckShortPreamble},
//        {true, Ieee80211PhyMode::GetErpOfdmRate12Mbps()},
//        {false, Ieee80211PhyMode::GetErpOfdmRate18Mbps()},
//        {true, Ieee80211PhyMode::GetErpOfdmRate24Mbps()},
//        {false, Ieee80211PhyMode::GetErpOfdmRate36Mbps()},
//        {false, Ieee80211PhyMode::GetErpOfdmRate48Mbps()},
//        {false, Ieee80211PhyMode::GetErpOfdmRate54Mbps()},
    }),
    Ieee80211ModeSet('p', {
//        {true, Ieee80211PhyMode::GetOfdmRate3MbpsCS10MHz()},
//        {false, Ieee80211PhyMode::GetOfdmRate4_5MbpsCS10MHz()},
//        {true, Ieee80211PhyMode::GetOfdmRate6MbpsCS10MHz()},
//        {false, Ieee80211PhyMode::GetOfdmRate9MbpsCS10MHz()},
//        {true, Ieee80211PhyMode::GetOfdmRate12MbpsCS10MHz()},
//        {false, Ieee80211PhyMode::GetOfdmRate18MbpsCS10MHz()},
//        {false, Ieee80211PhyMode::GetOfdmRate24MbpsCS10MHz()},
//        {false, Ieee80211PhyMode::GetOfdmRate27MbpsCS10MHz()},
    }),
};

Ieee80211ModeSet::Ieee80211ModeSet(char name, const std::vector<Entry> entries) :
    name(name),
    entries(entries)
{
}

int Ieee80211ModeSet::getModeIndex(const IIeee80211Mode *mode) const
{
    for (int index = 0; index < (int)entries.size(); index++) {
        if (entries[index].mode == mode)
            return index;
    }
    return -1;
}

const IIeee80211Mode *Ieee80211ModeSet::getMode(bps bitrate) const
{
    for (int index = 0; index < (int)entries.size(); index++) {
        if (entries[index].mode->getDataMode()->getGrossBitrate() == bitrate)
            return entries[index].mode;
    }
    return nullptr;
}

const IIeee80211Mode *Ieee80211ModeSet::getSlowestMode() const
{
    return entries.front().mode;
}

const IIeee80211Mode *Ieee80211ModeSet::getFastestMode() const
{
    return entries.back().mode;
}

const IIeee80211Mode *Ieee80211ModeSet::getSlowerMode(const IIeee80211Mode *mode) const
{
    int index = getModeIndex(mode);
    if (index > 0)
        return entries[index - 1].mode;
    else
        return nullptr;
}

const IIeee80211Mode *Ieee80211ModeSet::getFasterMode(const IIeee80211Mode *mode) const
{
    int index = getModeIndex(mode);
    if (index < (int)entries.size() - 1)
        return entries[index + 1].mode;
    else
        return nullptr;
}

const Ieee80211ModeSet *Ieee80211ModeSet::getModeSet(char mode) {
    for (int index = 0; index < (int)Ieee80211ModeSet::modeSets.size(); index++) {
        const Ieee80211ModeSet *modeSet = &Ieee80211ModeSet::modeSets[index];
        if (modeSet->getName() == mode)
            return modeSet;
    }
    return nullptr;
}

} // namespace physicallayer

} // namespace inet

