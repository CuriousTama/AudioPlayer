#pragma once
#include <type_traits>

struct XAUDIO2FX_REVERB_I3DL2_PARAMETERS;

namespace AudioHelper
{
    template<typename T, class Base>
    concept DerivedOrSameType = std::is_base_of_v<Base, T>;

    template<typename T>
    concept VectorConstructible = std::is_constructible_v<T, float, float, float>;
} // namespace AudioHelper

bool operator==(const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& _lhs, const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& _rhs);
bool operator!=(const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& _lhs, const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& _rhs);