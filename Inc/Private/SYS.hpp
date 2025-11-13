#pragma once

struct ADS124S08::SYS : SPI_Register_I {
private:
	static const Address  ADDRESS{0x09u};
	static const Register RESET_VALUE{0x10u};

	Register SYS_MON  : 3;
	Register CAL_SAMP : 2;
	Register TIMEOUT  : 1;
	Register CRC_EN	  : 1; // Appended _EN to avoid conflict with #define CRC in some environments
	Register SENDSTAT : 1;

public:
	SYS(Register val = SYS::RESET_VALUE);
	virtual ~SYS() = default;

#ifdef ADS124S08_GTEST_TESTING
	SYS(const std::array<Register, 5> &vals)
		: SYS_MON(vals[0]),	 //
		  CAL_SAMP(vals[1]), //
		  TIMEOUT(vals[2]),	 //
		  CRC_EN(vals[3]),	 //
		  SENDSTAT(vals[4]) {}

	FRIEND_TEST(SYS_Test, constructor_InitializesFieldsCorrectly_FromRegister);
#endif

	bool crc(void) const { return static_cast<bool>(CRC_EN); }
	bool sendStat(void) const { return static_cast<bool>(SENDSTAT); }

	/**
	 * @brief System Monitor Configuration
	 *
	 * @note Refer to ACS124S08 §9.6.1.10 "SYS Register" for details.
	 */
	enum class SystemMonitorConfig : Register {
		DISABLED	= 0b000u,
		PGA_SHORT	= 0b001u,
		INT_TEMP	= 0b010u,
		SUPPLY_A	= 0b011u,
		SUPPLY_B	= 0b100u,
		BURNOUT_0U2 = 0b101u,
		BURNOUT_1U0 = 0b110u,
		BURNOUT_10U = 0b111u,
	};

	enum class CalSampleSize : Register {
		AVG_1_SAMPLES  = 0b00u, // 1 Sample for Calibration
		AVG_4_SAMPLES  = 0b01u, // 4 Samples
		AVG_8_SAMPLES  = 0b10u, // 8 Samples (default)
		AVG_16_SAMPLES = 0b11u, // 16 Samples
	};

	SYS &setSystemMonitorConfig(SystemMonitorConfig config);
	SYS &setCalibrationSampleSize(CalSampleSize size);
	SYS &setTimeout(bool enable);
	SYS &setCRCEnable(bool enable);
	SYS &setSendStatus(bool enable);

	virtual Register toRegister(void) const override;

	virtual Address	 getAddress(void) const override { return ADDRESS; }
	virtual Register getResetValue(void) const override { return RESET_VALUE; }
};
