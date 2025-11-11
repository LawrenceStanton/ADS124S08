struct ADS124S08::STATUS : SPI_Register_I {
private:
	Register	   FL_POR	  : 1; // Power-On Reset Flag
	const Register RDY		  : 1; // Ready Flag
	const Register FL_P_RAILP : 1; // Positive PGA Output Positive Rail
	const Register FL_P_RAILN : 1; // Positive PGA Output Negative Rail
	const Register FL_N_RAILP : 1; // Negative PGA Output Positive Rail
	const Register FL_N_RAILN : 1; // Negative PGA Output Negative Rail
	const Register FL_REF_L1  : 1; // Reference Voltage Monitor Level 1
	const Register FL_REF_L0  : 1; // Reference Voltage Monitor Level 0

	static const Address  ADDRESS{0x01u};
	static const Register RESET_VALUE{0x80u};

public:
	STATUS(Register val = STATUS::RESET_VALUE);
	virtual ~STATUS() = default;

	virtual Register toRegister(void) const override;

	virtual Address	 getAddress(void) const override { return ADDRESS; }
	virtual Register getResetValue(void) const override { return RESET_VALUE; }
};
