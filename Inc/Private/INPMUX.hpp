#pragma once

/**
 * @brief Input multiplexer configuration for the ADS124S08.
 *
 */
struct ADS124S08::INPMUX : SPI_Register_I {
private:
	static const Address  ADDRESS{0x02u};
	static const Register RESET_VALUE{0x01u};

	Register MUXP : 4; // Positive Input Channel Selection
	Register MUXN : 4; // Negative Input Channel Selection

public:
	enum class InputSelect : Register {
		AIN0   = 0b0000u,
		AIN1   = 0b0001u,
		AIN2   = 0b0010u,
		AIN3   = 0b0011u,
		AIN4   = 0b0100u,
		AIN5   = 0b0101u,
		AIN6   = 0b0110u, // ADS124S08 only
		AIN7   = 0b0111u, // ADS124S08 only
		AIN8   = 0b1000u, // ADS124S08 only
		AIN9   = 0b1001u, // ADS124S08 only
		AIN10  = 0b1010u, // ADS124S08 only
		AIN11  = 0b1011u, // ADS124S08 only
		AINCOM = 0b1100u,
		// Reserved 0b1101u - 0b1110u
	};

	INPMUX(Register val = INPMUX::RESET_VALUE);
	INPMUX(InputSelect posChannel, InputSelect negChannel);

#ifdef ADS124S08_GTEST_TESTING
	FRIEND_TEST(INPMUX_Test, constructor_InitializesFieldsCorrectly_FromRegister);
	FRIEND_TEST(INPMUX_Test, constructor_InitializesFieldsCorrectly_FromInputSelect);
#endif

	virtual ~INPMUX() = default;

	virtual Register toRegister(void) const override;

	INPMUX &setPositiveInputChannel(InputSelect channel);

	INPMUX &setNegativeInputChannel(InputSelect channel);

	virtual Address	 getAddress(void) const override { return ADDRESS; }
	virtual Register getResetValue(void) const override { return RESET_VALUE; }
};
