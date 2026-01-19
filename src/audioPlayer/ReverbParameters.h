#pragma once
#include <xaudio2fx.h>

namespace ReverbParameters
{
    // List of presets reverbs from xaudio2fx
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Default{ 100, -10000, 0, 0.0f, 1.00f, 0.50f, -10000, 0.020f, -10000, 0.040f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Generic{ 100, -1000, -100, 0.0f, 1.49f, 0.83f, -2602, 0.007f, 200, 0.011f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Paddedcell{ 100, -1000, -6000, 0.0f, 0.17f, 0.10f, -1204, 0.001f, 207, 0.002f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Room{ 100, -1000, -454, 0.0f, 0.40f, 0.83f, -1646, 0.002f, 53, 0.003f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Bathroom{ 100, -1000, -1200, 0.0f, 1.49f, 0.54f, -370, 0.007f, 1030, 0.011f, 100.0f, 60.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Livingroom{ 100, -1000, -6000, 0.0f, 0.50f, 0.10f, -1376, 0.003f, -1104, 0.004f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Stoneroom{ 100, -1000, -300, 0.0f, 2.31f, 0.64f, -711, 0.012f, 83, 0.017f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Auditorium{ 100, -1000, -476, 0.0f, 4.32f, 0.59f, -789, 0.020f, -289, 0.030f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Concerthall{ 100, -1000, -500, 0.0f, 3.92f, 0.70f, -1230, 0.020f, -2, 0.029f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Cave{ 100, -1000, 0, 0.0f, 2.91f, 1.30f, -602, 0.015f, -302, 0.022f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Arena{ 100, -1000, -698, 0.0f, 7.24f, 0.33f, -1166, 0.020f, 16, 0.030f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Hangar{ 100, -1000, -1000, 0.0f, 10.05f, 0.23f, -602, 0.020f, 198, 0.030f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Carpetedhallway{ 100, -1000, -4000, 0.0f, 0.30f, 0.10f, -1831, 0.002f, -1630, 0.030f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Hallway{ 100, -1000, -300, 0.0f, 1.49f, 0.59f, -1219, 0.007f, 441, 0.011f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Stonecorridor{ 100, -1000, -237, 0.0f, 2.70f, 0.79f, -1214, 0.013f, 395, 0.020f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Alley{ 100, -1000, -270, 0.0f, 1.49f, 0.86f, -1204, 0.007f, -4, 0.011f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Forest{ 100, -1000, -3300, 0.0f, 1.49f, 0.54f, -2560, 0.162f, -613, 0.088f, 79.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS City{ 100, -1000, -800, 0.0f, 1.49f, 0.67f, -2273, 0.007f, -2217, 0.011f, 50.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Mountains{ 100, -1000, -2500, 0.0f, 1.49f, 0.21f, -2780, 0.300f, -2014, 0.100f, 27.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Quarry{ 100, -1000, -1000, 0.0f, 1.49f, 0.83f, -10000, 0.061f, 500, 0.025f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Plain{ 100, -1000, -2000, 0.0f, 1.49f, 0.50f, -2466, 0.179f, -2514, 0.100f, 21.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Parkinglot{ 100, -1000, 0, 0.0f, 1.65f, 1.50f, -1363, 0.008f, -1153, 0.012f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Sewerpipe{ 100, -1000, -1000, 0.0f, 2.81f, 0.14f, 429, 0.014f, 648, 0.021f, 80.0f, 60.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Underwater{ 100, -1000, -4000, 0.0f, 1.49f, 0.10f, -449, 0.007f, 1700, 0.011f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Smallroom{ 100, -1000, -600, 0.0f, 1.10f, 0.83f, -400, 0.005f, 500, 0.010f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Mediumroom{ 100, -1000, -600, 0.0f, 1.30f, 0.83f, -1000, 0.010f, -200, 0.020f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Largeroom{ 100, -1000, -600, 0.0f, 1.50f, 0.83f, -1600, 0.020f, -1000, 0.040f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Mediumhall{ 100, -1000, -600, 0.0f, 1.80f, 0.70f, -1300, 0.015f, -800, 0.030f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Largehall{ 100, -1000, -600, 0.0f, 1.80f, 0.70f, -2000, 0.030f, -1400, 0.060f, 100.0f, 100.0f, 5000.0f };
    static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Plate{ 100, -1000, -200, 0.0f, 1.30f, 0.90f, 0, 0.002f, 0, 0.010f, 100.0f, 75.0f, 5000.0f };
}; // namespace ReverbParameters