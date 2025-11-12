#pragma once

/**
 * @brief Configuration representation of the ADS124S08 REF register (Address 0x05).
 *
 */
struct ADS124S08::REF : SPI_Register_I {
private:
	static const Address  ADDRESS{0x05u};
	static const Register RESET_VALUE{0x10u};

	Register FL_REF_EN : 2; // Reference Monitor Configuration
	Register REFP_BUF  : 1; // Positive Reference Buffer Bypass
	Register REFN_BUF  : 1; // Negative Reference Buffer Bypass
	Register REFSEL	   : 2; // Reference Input Selection
	Register REFCON	   : 2; // Internal Reference Configuration

public:
	virtual Register toRegister(void) const override;

	REF(Register val = REF::RESET_VALUE);

#ifdef ADS124S08_GTEST_TESTING
	//? Only to be used for unit testing
	constexpr REF(const std::array<Register, 5> &vals);
#endif

	virtual ~REF() = default;

	enum class ReferenceMonitorConfig : Register {
		DISABLED				   = 0b00u, // Reference Monitor Disabled
		LEVEL_0_ONLY			   = 0b01u, // Level 0 enabled, threshold 0.3V.
		// Level 0 and Level 1  Enabled, thresholds 0.3V and 1/3 (AVDD - AVSS).
		LEVEL_0_LEVEL1			   = 0b10u,
		// Level 1 enabled, threshold 0.3V, 10MΩ pull-together enabled.
		LEVEL_0_10MR_PULL_TOGETHER = 0b11u,
	};

	REF &setReferenceMonitorConfig(ReferenceMonitorConfig config);

	enum class BufferBypassConfig : Register {
		ENABLED	 = 0b0u, // Bypass is Enabled
		DISABLED = 0b1u, // Bypass is Disabled
	};

	/**
	 * @brief Enable or disable the positive reference buffer bypass.
	 *
	 * @param bypass The bypass configuration.
	 * @return This `REF` object reference.
	 *
	 *! @note `setReferenceInputSelection()` for possible side effects affecting the
	 * reference buffers.
	 */
	REF &setPositiveRefBufferBypass(BufferBypassConfig bypass);

	/** @brief Enable or disable the negative reference buffer bypass.
	 *
	 * @param bypass The bypass configuration.
	 * @return This `REF` object reference.
	 *
	 *! @note `setReferenceInputSelection()` for possible side effects affecting the
	 * reference buffers.
	 */
	REF &setNegativeRefBufferBypass(BufferBypassConfig bypass);

	enum class InternalReferenceSelect : Register {
		REFP0_REFN0 = 0b00u, // Use REFP0 and REFN0 Inputs
		REFP1_REFN0 = 0b01u, // Use REFP1 and REFN0 Inputs
		INTERNAL	= 0b10u, // Use Internal Reference
							 // 0b11u is Reserved
	};

	/**
	 * @brief Select which reference input voltage to use.
	 *
	 * @param sel The reference input selection.
	 * @param disableBuffersIfInternalSelected Side effect to disable reference buffers if internal
	 * reference is selected. Defaults to `true`.
	 *
	 * @return This REF object reference.
	 */
	REF &setReferenceInputSelection(
		InternalReferenceSelect sel, //
		bool					disableBuffersIfInternalSelected = true
	);

	enum class IntRefVoltConfig : Register {
		OFF		   = 0b00u, // Internal Reference Off
		ON_POWERUP = 0b01u, // Internal Reference On except during Power-Down.
		ON_ALWAYS  = 0b10u, // Internal Reference Always On
							// 0b11u is Reserved
	};

	/**
	 * @brief Enable or disable the internal reference voltage.
	 *
	 * @param config
	 * @return This REF object reference.
	 */
	REF &setInternalReferenceVoltageConfig(IntRefVoltConfig config);

	virtual Address	 getAddress(void) const override { return ADDRESS; }
	virtual Register getResetValue(void) const override { return RESET_VALUE; }

#ifdef ADS124S08_GTEST_TESTING
	FRIEND_TEST(REF_Test, constructor_InitializesFieldsCorrectly_FromRegister);
#endif
};
