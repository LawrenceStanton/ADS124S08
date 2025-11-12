#include "ADS124S08.hpp"

using Register = ADS124S08::Register;
using STATUS   = ADS124S08::STATUS;

Register STATUS::toRegister(void) const {
	return 0x00u;
}

STATUS::STATUS(Register val)
	: FL_POR{static_cast<Register>((val >> 7u) & 0x01u)},
	  RDY{static_cast<Register>((val >> 6u) & 0x01u)},
	  FL_P_RAILP{static_cast<Register>((val >> 5u) & 0x01u)},
	  FL_P_RAILN{static_cast<Register>((val >> 4u) & 0x01u)},
	  FL_N_RAILP{static_cast<Register>((val >> 3u) & 0x01u)},
	  FL_N_RAILN{static_cast<Register>((val >> 2u) & 0x01u)},
	  FL_REF_L1{static_cast<Register>((val >> 1u) & 0x01u)},
	  FL_REF_L0{static_cast<Register>((val >> 0u) & 0x01u)} {}
