#include "ADS124S08.hpp"

using Register = ADS124S08::SPI::Register;
using SYS	   = ADS124S08::SYS;

SYS::SYS(Register val)
	: SYS_MON{static_cast<Register>((val >> 5u) & 0x07u)},
	  CAL_SAMP{static_cast<Register>((val >> 3u) & 0x03u)},
	  TIMEOUT{static_cast<Register>((val >> 2u) & 0x01u)},
	  CRC_EN{static_cast<Register>((val >> 1u) & 0x01u)},
	  SENDSTAT{static_cast<Register>((val >> 0u) & 0x01u)} {}

Register SYS::toRegister(void) const {
	Register regValue = 0u;
	regValue |= (SYS_MON << 5u);
	regValue |= (CAL_SAMP << 3u);
	regValue |= (TIMEOUT << 2u);
	regValue |= (CRC_EN << 1u);
	regValue |= (SENDSTAT << 0u);
	return regValue;
}

SYS &SYS::setSystemMonitorConfig(SystemMonitorConfig config) {
	SYS_MON = static_cast<Register>(config);
	return *this;
}

SYS &SYS::setCalibrationSampleSize(CalSampleSize size) {
	CAL_SAMP = static_cast<Register>(size);
	return *this;
}

SYS &SYS::setTimeout(bool enable) {
	TIMEOUT = static_cast<Register>(enable ? 1u : 0u);
	return *this;
}

SYS &SYS::setSendStatus(bool enable) {
	SENDSTAT = static_cast<Register>(enable ? 1u : 0u);
	return *this;
}

SYS &SYS::setCRCEnable(bool enable) {
	CRC_EN = static_cast<Register>(enable ? 1u : 0u);
	return *this;
}
