#include "ADS124S08.hpp"

using PGA	   = ADS124S08::PGA;
using Register = ADS124S08::Register;

PGA::PGA(Register val)				//
	: DELAY((val >> 5U) & 0b111u),	//
	  PGA_EN((val >> 3U) & 0b11u),	//
	  GAIN((val >> 0U) & 0b111u) {} //

PGA &PGA::setDelay(PGA::CONVERSION_DELAY delay) {
	DELAY = static_cast<Register>(delay);
	return *this;
}

PGA &PGA::setEnable(PGA::ENABLE enable, bool setUnityGainIfBypassed) {
	PGA_EN = static_cast<Register>(enable);
	if (enable == ENABLE::BYPASSED && setUnityGainIfBypassed)
		GAIN = static_cast<Register>(GAIN_SELECT::GAIN_1);
	return *this;
}

PGA &PGA::setGain(PGA::GAIN_SELECT gain, bool setPGAEnabledIfGainNotUnity) {
	GAIN = static_cast<Register>(gain);
	if (gain != GAIN_SELECT::GAIN_1 && setPGAEnabledIfGainNotUnity)
		PGA_EN = static_cast<Register>(ENABLE::ENABLED);
	return *this;
}

Register PGA::toRegister(void) const {
	Register reg = (DELAY << 5U)  //
				 | (PGA_EN << 3U) //
				 | (GAIN << 0U);
	return reg;
}
