#include "ADS124S08.hpp"

using Register = ADS124S08::SPI::Register;

using DATARATE		= ADS124S08::DATARATE;
using ChopperEnable = ADS124S08::DATARATE::ChopperEnable;
using ClockSelect	= ADS124S08::DATARATE::ClockSelect;
using ModeSelect	= ADS124S08::DATARATE::ModeSelect;
using FilterSelect	= ADS124S08::DATARATE::FilterSelect;
using DataRate		= ADS124S08::DATARATE::DataRate;

DATARATE::DATARATE(Register val)
	: G_CHOP((val >> 7) & 0x01u), //
	  CLK((val >> 6) & 0x01u),	  //
	  MODE((val >> 5) & 0x01u),	  //
	  FILTER((val >> 4) & 0x01u), //
	  DR(val & 0x0Fu) {}

Register DATARATE::toRegister(void) const {
	return (static_cast<Register>(G_CHOP) << 7) | //
		   (static_cast<Register>(CLK) << 6) |	  //
		   (static_cast<Register>(MODE) << 5) |	  //
		   (static_cast<Register>(FILTER) << 4) | //
		   (static_cast<Register>(DR) & 0x0Fu);
}

DATARATE &DATARATE::setGlobalChop(ChopperEnable enable) {
	G_CHOP = static_cast<Register>(enable);
	return *this;
}

ChopperEnable DATARATE::getGlobalChop(void) const {
	return static_cast<ChopperEnable>(G_CHOP);
}

DATARATE &DATARATE::setClockSelect(ClockSelect clk) {
	CLK = static_cast<Register>(clk);
	return *this;
}

ClockSelect DATARATE::getClockSelect(void) const {
	return static_cast<ClockSelect>(CLK);
}

DATARATE &DATARATE::setConversionMode(ModeSelect mode) {
	MODE = static_cast<Register>(mode);
	return *this;
}

ModeSelect DATARATE::getConversionMode(void) const {
	return static_cast<ModeSelect>(MODE);
}

DATARATE &DATARATE::setFilter(FilterSelect filter) {
	FILTER = static_cast<Register>(filter);
	return *this;
}

FilterSelect DATARATE::getFilter(void) const {
	return static_cast<FilterSelect>(FILTER);
}

DATARATE &DATARATE::setDataRate(DataRate rate) {
	DR = static_cast<Register>(rate);
	return *this;
}

DataRate DATARATE::getDataRate(void) const {
	return static_cast<DataRate>(DR);
}
