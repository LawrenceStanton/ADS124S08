#pragma once

using Register = ADS124S08::SPI::Register;

struct ADS124S08::DATARATE : public SPI_Register_I {
private:
	static const Address  ADDRESS{0x04u};
	static const Register RESET_VALUE{0x14u};

	Register G_CHOP : 1;
	Register CLK	: 1;
	Register MODE	: 1;
	Register FILTER : 1;
	Register DR		: 4;

public:
	DATARATE(Register val = RESET_VALUE);

	enum class ChopperEnable : Register {
		DISABLED = 0b0u, // (default) Chopper Disabled
		ENABLED	 = 0b1u, // Chopper Enabled
	};

	DATARATE &setGlobalChop(ChopperEnable enable);

	ChopperEnable getGlobalChop(void) const;

	enum class ClockSelect : Register {
		INTERNAL = 0b0u, // (default) Internal Clock
		EXTERNAL = 0b1u, // External Clock
	};

	DATARATE &setClockSelect(ClockSelect clk);

	ClockSelect getClockSelect(void) const;

	enum class ModeSelect : Register {
		CONTINUOUS	= 0b0u,
		SINGLE_SHOT = 0b1u,
	};

	DATARATE &setConversionMode(ModeSelect mode);

	ModeSelect getConversionMode(void) const;

	enum class FilterSelect : Register {
		SINC3		= 0b0u, // (default) SINC3 Filter
		LOW_LATENCY = 0b1u, // SINC4 Filter with 2x Oversampling
	};

	DATARATE &setFilter(FilterSelect filter);

	FilterSelect getFilter(void) const;

	enum class DataRate : Register {
		RATE_2_5  = 0b0000u,
		RATE_5	  = 0b0001u,
		RATE_10	  = 0b0010u,
		RATE_16_6 = 0b0011u,
		RATE_20	  = 0b0100u, // (default)
		RATE_50	  = 0b0101u,
		RATE_60	  = 0b0110u,
		RATE_100  = 0b0111u,
		RATE_200  = 0b1000u,
		RATE_400  = 0b1001u,
		RATE_800  = 0b1010u,
		RATE_1000 = 0b1011u,
		RATE_2000 = 0b1100u,
		RATE_4000 = 0b1101u,
		// 0b1110u and 0b1101 both map to RATE_4000
		// 0b1111u is reserved
	};

	DATARATE &setDataRate(DataRate rate);

	DATARATE::DataRate getDataRate(void) const;

	virtual Register toRegister(void) const override;
	virtual Address	 getAddress(void) const override { return ADDRESS; }
	virtual Register getResetValue(void) const override { return RESET_VALUE; }
};
