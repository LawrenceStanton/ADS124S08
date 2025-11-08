#include "ADS124S08.hpp"

#include <array>

using Address = ADS124S08::SPI::Address;

constexpr Address ADS124S08_MAX_REGISTER_ADDRESS = static_cast<Address>(0x11u);

std::optional<ADS124S08::Register> ADS124S08::rreg(
	ADS124S08::SPI::Address startAddress,
	const uint8_t			count,
	SPI::Register *const	buffer
) noexcept {
	// Range checks
	if (count < 1u || count > 18) return std::nullopt;
	if (static_cast<uint8_t>(startAddress) + count
		> static_cast<uint8_t>(ADS124S08_MAX_REGISTER_ADDRESS) + 1)
		return std::nullopt;

	// Nullptr check for single register read
	if (buffer == nullptr) {
		if (count != 1u) return std::nullopt;
	}

	std::array<uint8_t, 2> mosi = {
		static_cast<uint8_t>(0x20u | static_cast<uint8_t>(startAddress)), // RREG command
		static_cast<uint8_t>(count - 1), // Number of registers to read minus one
	};

	Register		reg0 = 0;
	Register *const miso = (buffer != nullptr) ? buffer : &reg0;

	const auto writeResult = spi.write(mosi.data(), static_cast<uint8_t>(mosi.size()));

	if (writeResult) {
		const auto readResult = spi.read(miso, count);
		if (readResult) { return std::optional(miso[0]); }
	}
	return std::nullopt;
}

std::optional<ADS124S08::Register> ADS124S08::wakeup() noexcept {
	const auto mosi		   = static_cast<uint8_t>(SPI::ControlCommand::WAKEUP);
	const auto writeResult = spi.write(&mosi, 1u);
	if (writeResult) return mosi;
	else return std::nullopt;
}
