#include "ADS124S08.hpp"

#include <array>

using Address		 = ADS124S08::SPI::Address;
using Register		 = ADS124S08::SPI::Register;
using Command		 = ADS124S08::SPI::Command;
using ControlCommand = ADS124S08::SPI::ControlCommand;

constexpr Address ADS124S08_MAX_REGISTER_ADDRESS = static_cast<Address>(0x11u);

std::optional<ADS124S08::Register> ADS124S08::rreg(
	const ADS124S08::SPI::Address startAddress,
	const uint8_t				  count,
	SPI::Register *const		  buffer
) noexcept {
	// Range checks
	if (count < 1u || count > 18) return std::nullopt;
	if (startAddress + count > ADS124S08_MAX_REGISTER_ADDRESS + 1) return std::nullopt;

	// Nullptr check for single register read
	if (buffer == nullptr) {
		if (count != 1u) return std::nullopt;
	}

	std::array<uint8_t, 2> mosi = {
		static_cast<uint8_t>(0x20u | (startAddress)), // RREG command
		(uint8_t)(count - 1u),						  // Number of registers to read minus one
	};

	Register		reg0 = 0;
	Register *const miso = (buffer != nullptr) ? buffer : &reg0;

	const auto writeResult = spi.write(mosi.data(), mosi.size());

	if (writeResult) {
		const auto readResult = spi.read(miso, count);
		if (readResult) { return std::optional(miso[0]); }
	}
	return std::nullopt;
}

static std::optional<Register>
writeSingleByteCommand(ADS124S08::SPI &spi, Command command) noexcept {
	const auto writeResult = spi.write(&command, 1u);
	if (writeResult) return command;
	else return std::nullopt;
}

std::optional<ADS124S08::Register> ADS124S08::wakeup() noexcept {
	return writeSingleByteCommand(spi, ControlCommand::WAKEUP);
}

std::optional<ADS124S08::Register> ADS124S08::powerdown() noexcept {
	return writeSingleByteCommand(spi, ControlCommand::POWERDOWN);
}

std::optional<ADS124S08::Register> ADS124S08::reset() noexcept {
	return writeSingleByteCommand(spi, ControlCommand::RESET);
}

std::optional<ADS124S08::Register> ADS124S08::start() noexcept {
	return writeSingleByteCommand(spi, ControlCommand::START);
}

std::optional<ADS124S08::Register> ADS124S08::stop() noexcept {
	return writeSingleByteCommand(spi, ControlCommand::STOP);
}

std::optional<ADS124S08::Register> ADS124S08::offsetCalibrate() noexcept {
	return writeSingleByteCommand(spi, SPI::CalibrationCommand::SYS_OFFSET_CAL);
}

std::optional<ADS124S08::Register> ADS124S08::gainCalibrate() noexcept {
	return writeSingleByteCommand(spi, SPI::CalibrationCommand::SYS_GAIN_CAL);
}

std::optional<ADS124S08::Register> ADS124S08::selfOffsetCalibrate() noexcept {
	return writeSingleByteCommand(spi, SPI::CalibrationCommand::SELF_OFFSET_CAL);
}
