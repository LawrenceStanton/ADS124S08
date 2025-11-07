#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "../Src/ADS124S08.cpp"

using ::testing::_;

using std::nullopt;

static constexpr std::array<ADS124S08::SPI::ControlCommand, 6> ControlCommands = {
	ADS124S08::SPI::ControlCommand::NOP,
	ADS124S08::SPI::ControlCommand::WAKEUP,
	ADS124S08::SPI::ControlCommand::POWERDOWN,
	ADS124S08::SPI::ControlCommand::RESET,
	ADS124S08::SPI::ControlCommand::START,
	ADS124S08::SPI::ControlCommand::STOP
};

TEST(ADS124S08_TestStatic, controlCommandsAssumesTheEvenByteValues) {
	for (auto &cmd : ControlCommands) {
		EXPECT_EQ(static_cast<uint8_t>(cmd) % 2, 0u) //
			<< "This driver only uses even byte values for control commands, even though the LSB is X.";
	}
}

TEST(ADS124S08_TestStatic, dataReadCommandAssumesTheCorrectByteValue) {
	EXPECT_EQ(static_cast<uint8_t>(ADS124S08::SPI::DataReadCommand::RDATA), 0x12u);
}

class MockSPI : public ADS124S08::SPI {
public:
	MOCK_METHOD(
		std::optional<uint8_t>, //
		read,
		(ADS124S08::SPI::Register *const buffer, uint8_t count),
		(noexcept, override)
	);
	MOCK_METHOD(
		std::optional<uint8_t>, //
		write,
		(const ADS124S08::SPI::Register *const buffer, uint8_t count),
		(noexcept, override)
	);
	MOCK_METHOD(
		(std::optional<std::tuple<uint8_t, uint8_t>>),
		readWrite,
		(const ADS124S08::SPI::Register *const txBuffer, ADS124S08::SPI::Register *const rxBuffer, uint8_t count),
		(noexcept, override)
	);
};
