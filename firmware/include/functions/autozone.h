/*
 * Copyright (C) 2021
 * Joseph Stephen VK7JS
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. Use of this source code or binary releases for commercial purposes is strictly forbidden. This includes, without limitation,
 *    incorporation in a commercial product or incorporation into a product or project which allows commercial use.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef _OPENGD77_AUTOZONE_H_
#define _OPENGD77_AUTOZONE_H_

#include "codeplug.h"


/*
An AutoZone is a virtual zone whose channels are autogenerated on the fly and do not take up any memory.
E.g. The Australian UHF CB band has 80 channels.
*/

typedef enum
{
	AutoZone_NONE=0,
	AutoZone_MRN=1, // International Marine.
	AutoZone_AU_UHFCB,
	AutoZone_GMRS,
	AutoZone_FRS,
	AutoZone_MURS,
	AutoZone_NOAA,
	AutoZone_PMR446,
	AutoZone_US_RAILWAY,
	AutoZone_TYPE_MAX
} AutoZoneType_t;

typedef enum
{
	AutoZoneEnabled = 0x01,
	AutoZoneModeDigital = 0x02, // 0 FM, 1 DMR.
	AutoZoneInterleaveChannels = 0x04, // 0 off, 1 on, bank 2 is interleaved with bank 1 
	AutoZoneDuplexEnabled = 0x08,
	AutoZoneOffsetDirectionPlus = 0x10, // unset is minus, set is plus. For simplex, unset prior bit.
	AutoZoneDuplexAvailable = 0x20,
	AutoZoneNarrow = 0x40, // if not set, assumed wide.
	AutoZoneHasBaseIndex = 0x80, // channel numbers do not begin at 1, see 	baseChannelNumberStart and interleaveChannelNumberStart;
	AutoZoneInterleavingStartsPrior = 0x100, // Start the interleaving  prior rather than after start frequency.
	AutoZoneSimplexUsesTXFrequency = 0x200, // usually a duplex channel when  set to simplex uses the rx freq for both rx and tx, this allows the use of the tx freq instead.
	AutoZoneHasBankAtOffset = 0x400, // GMRS has second bank of channels starting at base freq plus repeater offset which can be used for simplex. So, allowing for 4 banks, bank at base, interleaved bank at base, bank at offset and interleaved bank at offset.
	AutoZonePowerMask=0xf000, // 0 all rx, or max wattage allowed in increments of 0.5 w (up to 5 w).
} AutoZoneFlags_t;

typedef void (*AutoZoneFunctionPointer_t)(uint16_t index, struct_codeplugChannel_t *channelBuf); // Typedef for AutoZone function pointer for applying restrictions to an autoZone channel.

typedef struct
{
	AutoZoneType_t type;
	const char* name;
	uint16_t flags; // See AutoZoneFlags_t enum.
	uint32_t startFrequency; // mHz of first channel of base bank
	uint32_t endFrequency; // mHz of last non-interstitial/interleaved channel of base bank
	uint16_t channelSpacing; // kHz channel step x 100 between channels in base bank.
	uint16_t repeaterOffset; // kHz.
	uint8_t priorityChannelIndex;
	uint8_t totalChannelsInBaseBank;
	// The next four are used for display purposes, e.g. to give a particular physical channel an appropriate display channel number. 
	uint8_t baseChannelNumberStart;
	uint8_t interleaveChannelNumberStart;
	uint8_t offsetBankChannelNumberStart;
	uint8_t offsetBankInterleavedChannelNumberStart;
	const AutoZoneFunctionPointer_t ApplyChannelRestrictionsFunc;
} struct_AutoZoneParams_t;

bool AutoZoneIsValid();
bool AutoZoneIsCurrentZone(int zoneIndex);
void AutoZoneInitialize(AutoZoneType_t type);
bool AutoZoneGetZoneDataForIndex(int zoneNum, struct_codeplugZone_t *returnBuf);
// indices are 0-based.
bool AutoZoneGetFrequenciesForIndex(uint16_t index, uint32_t* rxFreq, uint32_t* txFreq);
bool AutoZoneGetChannelData(uint16_t index, struct_codeplugChannel_t *channelBuf);
void AutoZoneApplyChannelRestrictions(uint16_t index, struct_codeplugChannel_t *channelBuf);
bool AutoZoneGetData(AutoZoneType_t type, struct_AutoZoneParams_t* autoZone);
uint8_t AutoZoneGetEnabledCount(); // Count the number of autoZones actually enabled.
#endif //_OPENGD77_AUTOZONE_H_