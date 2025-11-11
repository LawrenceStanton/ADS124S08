#pragma once

struct ADS124S08::STATUS : SPI_Register_I {
private:
	static const Address  ADDRESS{0x01u};
	static const Register RESET_VALUE{0x80u};

	const Register FL_POR	  : 1; // Power-On Reset Flag
	const Register RDY		  : 1; // Ready Flag
	const Register FL_P_RAILP : 1; // Positive PGA Output Positive Rail
	const Register FL_P_RAILN : 1; // Positive PGA Output Negative Rail
	const Register FL_N_RAILP : 1; // Negative PGA Output Positive Rail
	const Register FL_N_RAILN : 1; // Negative PGA Output Negative Rail
	const Register FL_REF_L1  : 1; // Reference Voltage Monitor Level 1
	const Register FL_REF_L0  : 1; // Reference Voltage Monitor Level 0

public:
	STATUS(Register val = STATUS::RESET_VALUE);
	virtual ~STATUS() = default;

	virtual Register toRegister(void) const override;

	virtual Address	 getAddress(void) const override { return ADDRESS; }
	virtual Register getResetValue(void) const override { return RESET_VALUE; }

	enum class POR_Flag : Register {
		NOT_CLEARED = 0b1u, // Power-On Reset has occurred since last read of STATUS
		CLEARED		= 0b0u, // No Power-On Reset has occurred since last read of STATUS
	};

	enum class Dev_RDY : Register {
		NOT_READY = 0b1u, // ADC Not Ready for Communication
		READY	  = 0b0u, // ADC Ready for Communication
	};

	enum class FL_RAIL : Register {
		ERROR	 = 0b1u, // PGA Output within 150mV of AVDD or AVSS
		NO_ERROR = 0b0u, // No Error
	};

	/**
	 * @brief Reference Voltage Monitor Level Status
	 *
	 * Indicates whether the differential reference voltage has exceeded. Thresholds are:
	 * - Level 0: 0.3V
	 * - Level 1: 1/3 (AVDD - AVSS)
	 */
	enum class FL_REF_LVL : Register {
		NOT_EXCEEDED = 0b1u, // Less than Reference Voltage Monitor Threshold for Level
		EXCEEDED	 = 0b0u, // More than Reference Voltage Monitor Threshold for Level
	};

	inline POR_Flag	  get_FL_POR(void) const { return static_cast<POR_Flag>(FL_POR); }
	inline Dev_RDY	  get_RDY(void) const { return static_cast<Dev_RDY>(RDY); }
	inline FL_RAIL	  get_FL_P_RAILP(void) const { return static_cast<FL_RAIL>(FL_P_RAILP); }
	inline FL_RAIL	  get_FL_P_RAILN(void) const { return static_cast<FL_RAIL>(FL_P_RAILN); }
	inline FL_RAIL	  get_FL_N_RAILP(void) const { return static_cast<FL_RAIL>(FL_N_RAILP); }
	inline FL_RAIL	  get_FL_N_RAILN(void) const { return static_cast<FL_RAIL>(FL_N_RAILN); }
	inline FL_REF_LVL get_FL_REF_L1(void) const { return static_cast<FL_REF_LVL>(FL_REF_L1); }
	inline FL_REF_LVL get_FL_REF_L0(void) const { return static_cast<FL_REF_LVL>(FL_REF_L0); }

#ifdef ADS124S08_GTEST_TESTING
	friend class ADS124S08_STATUS_Test;
	FRIEND_TEST(ADS124S08_STATUS_Test, constructor_InitializesFieldsCorrectly_FromRegister);
#endif
};
