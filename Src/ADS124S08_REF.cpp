#include "ADS124S08.hpp"

using Register = ADS124S08::Register;
using REF	   = ADS124S08::REF;

Register REF::toRegister(void) const {
	Register regValue = 0u;

	regValue |= (FL_REF_EN << 6u);
	regValue |= (REFP_BUF << 5u);
	regValue |= (REFN_BUF << 4u);
	regValue |= (REFSEL << 2u);
	regValue |= (REFCON << 0u);

	return regValue;
}

REF::REF(Register val)
	: FL_REF_EN{static_cast<Register>((val >> 6u) & 0x03u)},
	  REFP_BUF{static_cast<Register>((val >> 5u) & 0x01u)},
	  REFN_BUF{static_cast<Register>((val >> 4u) & 0x01u)},
	  REFSEL{static_cast<Register>((val >> 2u) & 0x03u)},
	  REFCON{static_cast<Register>((val >> 0u) & 0x03u)} {}

#ifdef ADS124S08_GTEST_TESTING
constexpr REF::REF(const std::array<Register, 5> &vals)
	: FL_REF_EN{vals[0]}, //
	  REFP_BUF{vals[1]},  //
	  REFN_BUF{vals[2]},  //
	  REFSEL{vals[3]},	  //
	  REFCON{vals[4]} {}
#endif

REF &REF::setReferenceMonitorConfig(ReferenceMonitorConfig config) {
	FL_REF_EN = static_cast<Register>(config);
	return *this;
}

REF &REF::setPositiveRefBufferBypass(BufferBypassConfig bypass) {
	REFP_BUF = static_cast<Register>(bypass);
	return *this;
}

REF &REF::setNegativeRefBufferBypass(BufferBypassConfig bypass) {
	REFN_BUF = static_cast<Register>(bypass);
	return *this;
}

REF &REF::setReferenceInputSelection(
	InternalReferenceSelect sel, //
	bool					disableBuffersIfInternalSelected
) {
	REFSEL = static_cast<Register>(sel);
	if (sel == InternalReferenceSelect::INTERNAL && disableBuffersIfInternalSelected) {
		// Reference buffers must be disabled when using internal reference
		REFN_BUF = 0b1u;
		REFP_BUF = 0b1u;
	}
	return *this;
}

REF &REF::setInternalReferenceVoltageConfig(IntRefVoltConfig config) {
	REFCON = static_cast<Register>(config);
	return *this;
}
