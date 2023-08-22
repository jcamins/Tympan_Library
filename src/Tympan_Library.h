
#ifndef Tympan_Library_h
#define Tympan_Library_h

#include "AICShield.h"
#include "AudioStream_F32.h"
#include "BTNRH_WDRC_Types.h"
#include "control_aic3206.h"
#include "AudioCalcEnvelope_F32.h"
#include "AudioCalcGainWDRC_F32.h"
#include "AudioCalcGainDecWDRC_F32.h"
#include "AudioCalcLeq_F32.h"
#include "AudioCalcLevel_F32.h"
#include "AudioConfigFIRFilter_F32.h"
#include "AudioConfigFIRFilterBank_F32.h"
#include "AudioConfigIIRFilterBank_F32.h"
#include "AudioControlTester.h"
#include "AudioConvert_F32.h"
#include "AudioEffectCompWDRC_F32.h"
#include "AudioEffectCompBankWDRC_F32.h"
#include "AudioEffectCompDecWDRC_F32.h"
#include "AudioEffectEmpty_F32.h"
#include "AudioEffectFade_F32.h"
#include "AudioEffectGain_F32.h"
#include "AudioEffectCompressor_F32.h"
#include "AudioEffectDelay_f32.h"
#include "AudioEffectFeedbackCancel_F32.h"
#include "AudioEffectFormantShift_FD_F32.h"
#include "AudioEffectFreqShift_FD_F32.h"
#include "AudioEffectMultiBandWDRC_F32.h"
#include "AudioFilterbank_F32.h"
#include "AudioFilterBiquad_F32.h"
#include "AudioFilterFIR_F32.h"
#include "AudioFilterIIR_F32.h"
#include "AudioFilterFreqWeighting_F32.h"
#include "AudioFilterTimeWeighting_F32.h"
#include "AudioFreqDomainBase_FD_F32.h"
#include "AudioMixer_F32.h"
#include "AudioMathAdd_F32.h"
#include "AudioMathMultiply_F32.h"
#include "AudioMathOffset_F32.h"
#include "AudioMathScale_F32.h"
#include "AudioPlayMemory_F32.h"
#include "AudioRateDecimator_F32.h"
#include "AudioRateInterpolator_F32.h"
#include "AudioSettings_F32.h"
#include "AudioSummer_F32.h"
#include "AudioSwitch_F32.h"
#include "EarpieceMixer_F32.h"
#include "EarpieceMixer_F32_UI.h"
#include "FFT_F32.h"
#include "FFT_Overlapped_F32.h"
#include "play_queue_f32.h"
#include "PresetManager_UI.h"
#include "record_queue_f32.h"
#include "input_i2s_f32.h"
#include "input_i2s_quad_f32.h"
#include "AudioSDPlayer_F32.h"
#include "AudioSDWriter_F32.h"
#include "SDWriter.h"
#include "SerialManagerBase.h"
#include "SerialManager_UI.h"
#include "StereoContainer_UI.h"

#include "synth_pinknoise_f32.h"
#include "synth_waveform_F32.h"
#include "synth_whitenoise_f32.h"
#include "synth_silence_f32.h"
#include "synth_sine_f32.h"
#include "synth_tonesweep_F32.h"
#include "Tympan.h"
#include "TympanRemoteFormatter.h"
#include "TympanStateBase.h"
#include "output_i2s_f32.h"
#include "output_i2s_quad_f32.h"
//include "USB_Audio_F32.h"
#include "BLE/bc127.h"
#include "BLE/ble.h"

#endif

