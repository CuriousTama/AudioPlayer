#include "AudioHelper.h"
#include <xaudio2.h>
#include <xaudio2fx.h>

// ----------------------------------------------
bool operator==(const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& _lhs, const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& _rhs)
{
    return _lhs.WetDryMix == _rhs.WetDryMix &&
           _lhs.Room == _rhs.Room &&
           _lhs.RoomHF == _rhs.RoomHF &&
           _lhs.RoomRolloffFactor == _rhs.RoomRolloffFactor &&
           _lhs.DecayTime == _rhs.DecayTime &&
           _lhs.DecayHFRatio == _rhs.DecayHFRatio &&
           _lhs.Reflections == _rhs.Reflections &&
           _lhs.ReflectionsDelay == _rhs.ReflectionsDelay &&
           _lhs.Reverb == _rhs.Reverb &&
           _lhs.ReverbDelay == _rhs.ReverbDelay &&
           _lhs.Diffusion == _rhs.Diffusion &&
           _lhs.Density == _rhs.Density &&
           _lhs.HFReference == _rhs.HFReference;
}

// ----------------------------------------------
bool operator!=(const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& _lhs, const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& _rhs)
{
    return !(_lhs == _rhs);
}