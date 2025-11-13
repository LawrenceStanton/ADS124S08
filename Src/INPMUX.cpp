#include "ADS124S08.hpp"

using Register = ADS124S08::Register;
using INPMUX   = ADS124S08::INPMUX;

Register INPMUX::toRegister(void) const {
	Register regValue = 0u;

	regValue |= (MUXP << 4u);
	regValue |= (MUXN << 0u);

	return regValue;
}

INPMUX::INPMUX(Register val)
	: MUXP{static_cast<Register>((val >> 4u) & 0x0Fu)},
	  MUXN{static_cast<Register>((val >> 0u) & 0x0Fu)} {}

INPMUX::INPMUX(InputSelect posChannel, InputSelect negChannel)
	: MUXP{static_cast<Register>(posChannel)}, //
	  MUXN{static_cast<Register>(negChannel)} {}

INPMUX &INPMUX::setPositiveInputChannel(InputSelect channel) {
	MUXP = static_cast<Register>(channel);
	return *this;
}

INPMUX &INPMUX::setNegativeInputChannel(InputSelect channel) {
	MUXN = static_cast<Register>(channel);
	return *this;
}
