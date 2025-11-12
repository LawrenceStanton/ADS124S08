#include "ADS124S08.hpp"

using Register = ADS124S08::Register;
using ID	   = ADS124S08::ID;

Register ADS124S08::ID::toRegister(void) const {
	return static_cast<Register>(DEV_ID);
}
