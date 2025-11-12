#pragma once

using Register = ADS124S08::SPI::Register;

struct ADS124S08::ID : SPI_Register_I {
private:
	static const Address  ADDRESS{0x00u};
	static const Register RESET_VALUE{0x00u};

	const Register DEV_ID : 3; // Device ID

public:
	virtual Register toRegister(void) const override;

	ID(Register id = ID::RESET_VALUE) : DEV_ID(id) {}

	virtual ~ID() = default;

	enum class DeviceID : Register {
		ADS124S08 = 0b000u, // ADS124S08 Device ID
		ADS124S06 = 0b001u, // ADS124S06 Device ID
							// 0b010u to 0b111u Reserved
	};

	DeviceID getDeviceID(void) const { return static_cast<DeviceID>(DEV_ID); }

	virtual Address	 getAddress(void) const override { return this->ADDRESS; }
	virtual Register getResetValue(void) const override { return ID::RESET_VALUE; }
};

Register ADS124S08::ID::toRegister(void) const {
	return static_cast<Register>(DEV_ID);
}
