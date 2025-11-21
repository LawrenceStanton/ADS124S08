#include "ADS124S08.hpp"

#include <algorithm>
#include <array>

using Address		 = ADS124S08::SPI::Address;
using Register		 = ADS124S08::SPI::Register;
using Command		 = ADS124S08::SPI::Command;
using ControlCommand = ADS124S08::SPI::ControlCommand;

ADS124S08::ADS124S08(SPI &spi) : spi(spi) {
	getSystemControl();
}

static constexpr Address ADS124S08_MAX_REGISTER_ADDRESS = static_cast<Address>(0x11u);
static constexpr uint8_t ADS124S08_MAX_REGISTER_COUNT	= 18u;

static constexpr bool
validateAddressRange(const ADS124S08::SPI::Address startAddress, const uint8_t count) noexcept {
	if (count < 1u || count > ADS124S08_MAX_REGISTER_COUNT) return false;
	if (startAddress + count > ADS124S08_MAX_REGISTER_ADDRESS + 1) return false;
	return true;
}

std::optional<ADS124S08::Register> ADS124S08::rreg(
	const ADS124S08::SPI::Address startAddress,
	const uint8_t				  count,
	SPI::Register *const		  buffer
) const noexcept {
	// Range checks
	if (!validateAddressRange(startAddress, count)) return std::nullopt;

	// Nullptr check for single register read
	if (buffer == nullptr) {
		if (count != 1u) return std::nullopt;
	}

	std::array<uint8_t, 2> mosi = {
		(uint8_t)(0x20u | startAddress), // RREG command
		(uint8_t)(count - 1u),			 // Number of registers to read minus one
	};

	Register		reg0 = 0;
	Register *const miso = (buffer != nullptr) ? buffer : &reg0;

	const auto writeResult = spi.write(mosi.data(), mosi.size());

	if (writeResult) {
		const auto readResult = spi.read(miso, count);
		if (readResult) { return miso[0]; }
	}
	return std::nullopt;
}

std::optional<ADS124S08::Register> ADS124S08::wreg(
	const ADS124S08::SPI::Address startAddress,
	const uint8_t				  count,
	const SPI::Register *const	  buffer
) const noexcept {
	// Range checks
	if (count > ADS124S08_MAX_REGISTER_COUNT) return std::nullopt;
	if (!validateAddressRange(startAddress, count)) return std::nullopt;
	if (buffer == nullptr) return std::nullopt;

	// Allocating excess to maintain STATIC stack usage
	Register mosi[2 + ADS124S08_MAX_REGISTER_COUNT];

	mosi[0] = (uint8_t)(0x40u | startAddress); // WREG command
	mosi[1] = (uint8_t)(count - 1u);		   // Number of registers to write minus one
	std::copy_n(buffer, count, &mosi[2]);

	const auto writeResult = spi.write(mosi, 2u + count);

	if (writeResult) {
		return mosi[2];
	} else return std::nullopt;
}

std::optional<ADS124S08::Register> ADS124S08::wreg(
	const ADS124S08::SPI::Address startAddress,
	const SPI::Register			 &value
) const noexcept {
	return wreg(startAddress, 1u, &value);
}

std::optional<ADS124S08::RDATA>
ADS124S08::rdata(std::optional<bool> statusEnabled, std::optional<bool> crcEnabled) const noexcept {
	uint8_t byteCount = 3u; // Data bytes

	bool statusByte =
		statusEnabled.value_or(SYS(sysCache).sendStat());	 // Default to cached SYS register value
	bool crcByte = crcEnabled.value_or(SYS(sysCache).crc()); // Default to cached SYS register value

	if (statusByte) byteCount += 1u;
	if (crcByte) byteCount += 1u;

	Register misoBuffer[5u] = {0}; // Max 5 bytes (STATUS + 3 data + CRC)

	Register mosiBuffer[1u] = {
		static_cast<Register>(SPI::DataReadCommand::RDATA),
	};

	auto writeResult = spi.write(mosiBuffer, sizeof(mosiBuffer));
	if (!writeResult) return std::nullopt;

	auto readResult = spi.read(misoBuffer, byteCount);
	if (!readResult) return std::nullopt;

	RDATA result;

	uint8_t index = 0u;
	if (statusByte) result.status = misoBuffer[index++];
	else result.status = std::nullopt;

	result.data = 0u;
	for (uint8_t i = index; i < index + 3u; i++) {
		result.data = (result.data << 8u) | misoBuffer[i];
	}
	index += 3u;

	if (crcByte) result.crc = misoBuffer[index++];
	else result.crc = std::nullopt;

	return result;
}

std::optional<ADS124S08::SYS> ADS124S08::getSystemControl(void) noexcept {
	auto sysReg = rreg(SPI::Address::SYS, 1u);
	if (sysReg) {
		sysCache = *sysReg;
		return SYS(*sysReg);
	} else return std::nullopt;
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

std::optional<Register> ADS124S08::setRegister(const SPI_Register_I &reg) const noexcept {
	Register value = reg.toRegister();
	return wreg(reg.getAddress(), value);
}

float ADS124S08::RDATA::toVoltage(float pgaGain, float vRef) const {
	// Convert 24-bit two's complement data to a signed integer
	int32_t rawData = static_cast<int32_t>(data);
	if (rawData & 0x800000) {  // Check if the sign bit is set
		rawData |= 0xFF000000; // Sign-extend to 32 bits
	}

	// Convert to voltage
	return (rawData / static_cast<float>(0x800000)) * (vRef / pgaGain);
}
