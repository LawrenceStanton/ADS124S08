#pragma once

using Register = ADS124S08::SPI::Register;

struct ADS124S08::PGA : SPI_Register_I {
private:
	static const Address  ADDRESS{0x03u};
	static const Register RESET_VALUE{0x00u};

	Register DELAY	: 3;
	Register PGA_EN : 2;
	Register GAIN	: 3;

public:
	PGA(Register val = RESET_VALUE);

#ifdef ADS124S08_GTEST_TESTING
	PGA(const std::array<Register, 3> &vals)
		: DELAY(vals[0]),  //
		  PGA_EN(vals[1]), //
		  GAIN(vals[2]) {}

	friend class PGA_Test;
	FRIEND_TEST(PGA_Test, constructor_InitializesFieldsCorrectly_FromRegister);
#endif

	/**
	 * @brief Sets the programmable conversion delay time for the first conversion after WREG
	 * when a configuration change resets of the digital filter and triggers new conversion.
	 *
	 * @note Units in terms of t_{MOD}.
	 */
	enum class CONVERSION_DELAY : Register {
		DELAY_14   = 0b000u,
		DELAY_25   = 0b001u,
		DELAY_64   = 0b010u,
		DELAY_256  = 0b011u,
		DELAY_1024 = 0b100u,
		DELAY_2048 = 0b101u,
		DELAY_4096 = 0b110u,
		DELAY_1	   = 0B111u,
	};

	PGA &setDelay(CONVERSION_DELAY delay);

	/**
	 * @brief Enables or bypasses the PGA.
	 *
	 */
	enum class ENABLE : Register {
		BYPASSED = 0b00u, // (default) @warning When bypassed, GAIN must be set to 0b000u.
		ENABLED	 = 0b01u,
		// 0b10 and 0b11 are reserved
	};

	PGA &setEnable(ENABLE enable, bool setUnityGainIfBypassed = true);

	/**
	 * @brief Configures the PGA gain.
	 *
	 */
	enum class GAIN_SELECT : Register {
		GAIN_1	 = 0b000u,
		GAIN_2	 = 0b001u,
		GAIN_4	 = 0b010u,
		GAIN_8	 = 0b011u,
		GAIN_16	 = 0b100u,
		GAIN_32	 = 0b101u,
		GAIN_64	 = 0b110u,
		GAIN_128 = 0b111u,
	};

	PGA &setGain(GAIN_SELECT gain, bool setPGAEnabledIfGainNotUnity = true);

	Register		 toRegister(void) const override;
	virtual Address	 getAddress(void) const override { return ADDRESS; }
	virtual Register getResetValue(void) const override { return RESET_VALUE; }
};
