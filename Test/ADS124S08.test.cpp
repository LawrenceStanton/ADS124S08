#include "gmock/gmock.h"
#include "gtest/gtest.h"

#define ADS124S08_GTEST_TESTING

#include "../Src/ADS124S08.cpp"

using ::testing::_;
using ::testing::Eq;
using ::testing::Return;

using std::nullopt;

using Register			 = ADS124S08::SPI::Register;
using Address			 = ADS124S08::SPI::Address;
using ControlCommand	 = ADS124S08::SPI::ControlCommand;
using CalibrationCommand = ADS124S08::SPI::CalibrationCommand;

static constexpr std::array<ControlCommand, 6> ControlCommands = {
	ControlCommand::NOP,
	ControlCommand::WAKEUP,
	ControlCommand::POWERDOWN,
	ControlCommand::RESET,
	ControlCommand::START,
	ControlCommand::STOP
};

TEST(ADS124S08_TestStatic, controlCommandsAssumesTheEvenByteValues) {
	for (auto &cmd : ControlCommands) {
		EXPECT_EQ(static_cast<uint8_t>(cmd) % 2, 0u) //
			<< "This driver only uses even byte values for control commands, "
			   "even though the LSB "
			   "is X.";
	}
}

TEST(ADS124S08_TestStatic, dataReadCommandAssumesTheEvenByteValue) {
	EXPECT_EQ(static_cast<uint8_t>(ADS124S08::SPI::DataReadCommand::RDATA), 0x12u)
		<< "This driver only uses even byte values for data read commands, "
		   "even though the LSB is X.";
}

class FakeSPI {
public:
	static std::optional<uint8_t>
	fakeRead(Register *const buffer, uint8_t count, Register const *fakeValues) noexcept {
		std::copy_n(fakeValues, count, buffer);
		return count;
	}

	static std::optional<uint8_t> //
	fakeWrite(const Register *const buffer, uint8_t count) noexcept {
		return count;
	}

	static std::optional<std::tuple<uint8_t, uint8_t>> fakeReadWrite(
		const Register *const txBuffer,
		Register *const		  rxBuffer,
		uint8_t				  count,
		Register const		 *fakeReadValues
	) noexcept {
		std::copy_n(fakeReadValues, count, rxBuffer);
		return std::make_tuple(count, count);
	}
};

class MockSPI : public ADS124S08::SPI {
public:
	MOCK_METHOD(
		std::optional<uint8_t>, //
		read,
		(Register *const buffer, uint8_t count),
		(noexcept, override)
	);
	MOCK_METHOD(
		std::optional<uint8_t>, //
		write,
		(const Register *const buffer, uint8_t count),
		(noexcept, override)
	);
	MOCK_METHOD(
		(std::optional<std::tuple<uint8_t, uint8_t>>),
		readWrite,
		(const Register *const txBuffer, Register *const rxBuffer, uint8_t count),
		(noexcept, override)
	);

	void delegateToFakes(Register const *fakeReadValues) {
		ON_CALL(*this, read(_, _))
			.WillByDefault([this, fakeReadValues](Register *const buffer, uint8_t count) {
				return FakeSPI::fakeRead(buffer, count, fakeReadValues);
			});
		ON_CALL(*this, write(_, _))
			.WillByDefault([this](const Register *const buffer, uint8_t count) {
				return FakeSPI::fakeWrite(buffer, count);
			});
		ON_CALL(*this, readWrite(_, _, _))
			.WillByDefault([this, fakeReadValues](
							   const Register *const txBuffer,
							   Register *const		 rxBuffer,
							   uint8_t				 count
						   ) {
				return FakeSPI::fakeReadWrite(txBuffer, rxBuffer, count, fakeReadValues);
			});
	}

	void disableMISO(void) { ON_CALL(*this, read(_, _)).WillByDefault(Return(nullopt)); }
	void disableMOSI(void) { ON_CALL(*this, write(_, _)).WillByDefault(Return(nullopt)); }
	void disableSPI(void) {
		disableMISO();
		disableMOSI();
		ON_CALL(*this, readWrite(_, _, _)).WillByDefault(Return(nullopt));
	}
};

class ADS124S08_Test : public ::testing::Test {
public:
	MockSPI	  mockSPI{};
	ADS124S08 adc{mockSPI};

	std::array<std::pair<Command, std::function<std::optional<uint8_t>()>>, 8u> singleByteCommands =
		{{
			{ControlCommand::WAKEUP, [&]() { return adc.wakeup(); }},
			{ControlCommand::POWERDOWN, [&]() { return adc.powerdown(); }},
			{ControlCommand::RESET, [&]() { return adc.reset(); }},
			{ControlCommand::START, [&]() { return adc.start(); }},
			{ControlCommand::STOP, [&]() { return adc.stop(); }},
			{CalibrationCommand::SYS_OFFSET_CAL, [&]() { return adc.offsetCalibrate(); }},
			{CalibrationCommand::SYS_GAIN_CAL, [&]() { return adc.gainCalibrate(); }},
			{CalibrationCommand::SELF_OFFSET_CAL, [&]() { return adc.selfOffsetCalibrate(); }},
		}};
};

static constexpr uint8_t MAX_TEST_CASES = 19;

struct RREG_TestCase {
	Address address;
	uint8_t count;

	std::array<Register, MAX_TEST_CASES> expectedValues;

	constexpr RREG_TestCase(Address addr, uint8_t count, std::initializer_list<Register> vals)
		: address(addr), count(count), expectedValues{} {
		size_t i = 0;
		for (auto val : vals) {
			expectedValues[i++] = val;
		}
	}
};

constexpr RREG_TestCase const rregCases[] = {
	// 1-byte read
	{static_cast<Address>(0x00u), 1u, {0x00u}}, //
	{static_cast<Address>(0x00u), 1u, {0x01u}}, //
	{static_cast<Address>(0x01u), 1u, {0x80u}}, //
	{static_cast<Address>(0x02u), 1u, {0x01u}}, //
	{static_cast<Address>(0x03u), 1u, {0x00u}}, //
	{static_cast<Address>(0x04u), 1u, {0x14u}}, //
	{static_cast<Address>(0x05u), 1u, {0x10u}}, //
	{static_cast<Address>(0x06u), 1u, {0x00u}}, //
	{static_cast<Address>(0x07u), 1u, {0xFFu}}, //
	{static_cast<Address>(0x08u), 1u, {0x00u}}, //
	{static_cast<Address>(0x09u), 1u, {0x00u}}, //
	{static_cast<Address>(0x0Au), 1u, {0x00u}}, //
	{static_cast<Address>(0x0Bu), 1u, {0x00u}}, //
	{static_cast<Address>(0x0Cu), 1u, {0x00u}}, //
	{static_cast<Address>(0x0Du), 1u, {0x00u}}, //
	{static_cast<Address>(0x0Eu), 1u, {0x00u}}, //
	{static_cast<Address>(0x0Fu), 1u, {0x40u}}, //
	{static_cast<Address>(0x10u), 1u, {0x00u}}, //
	{static_cast<Address>(0x11u), 1u, {0x00u}},
	// 2-byte read
	{static_cast<Address>(0x00u), 2u, {0x00u, 0x80u}}, //
	{static_cast<Address>(0x02u), 2u, {0x01u, 0x00u}}, //
	{static_cast<Address>(0x06u), 2u, {0x00u, 0xFFu}}, //
	{static_cast<Address>(0x0Fu), 2u, {0x40u, 0x00u}},
	// Full 18-byte read
	{static_cast<Address>(0x00u),
	 18u,
	 {
		 0x00u, //
		 0x80u, //
		 0x01u, //
		 0x00u, //
		 0x14u, //
		 0x10u, //
		 0x00u, //
		 0xFFu, //
		 0x00u, //
		 0x00u, //
		 0x00u, //
		 0x00u, //
		 0x00u, //
		 0x00u, //
		 0x00u, //
		 0x40u, //
		 0x00u, //
		 0x00u, //
	 }}
};

TEST_F(ADS124S08_Test, rregNormallyReadsExpectedValues) {
	for (const auto &testCase : rregCases) {
		mockSPI.delegateToFakes(testCase.expectedValues.data());

		EXPECT_CALL(mockSPI, write(_, Eq(2u))).Times(1);
		EXPECT_CALL(mockSPI, read(_, Eq(testCase.count))).Times(1);

		/* This test assumes the readWrite interface method is not used.
		If changed to readWrite, update the test, packing the necessary zero
		bytes into the expectedValues array.
		*/
		EXPECT_CALL(mockSPI, readWrite(_, _, _)).Times(0);

		Register readBuffer[testCase.count];

		auto result = adc.rreg(testCase.address, testCase.count, readBuffer);

		EXPECT_TRUE(result.has_value());
		EXPECT_EQ(result.value(), testCase.expectedValues[0]);
		for (size_t i = 0; i < testCase.count; i++) {
			EXPECT_EQ(testCase.expectedValues[i], readBuffer[i]);
		}
	}
}

TEST_F(ADS124S08_Test, rregReturnsExpectedValueForSingleByteRead) {
	for (const auto &testCase : rregCases) {
		if (testCase.count != 1u) {
			continue; // Only test single-byte reads here
		}

		mockSPI.delegateToFakes(testCase.expectedValues.data());

		auto result = adc.rreg(testCase.address);

		EXPECT_TRUE(result.has_value());
		EXPECT_EQ(result.value(), testCase.expectedValues[0]);
	}
}

TEST_F(ADS124S08_Test, rregChecksForNullptrBufferOnMultiByteRead) {
	EXPECT_CALL(mockSPI, write(_, _)).Times(0);
	EXPECT_CALL(mockSPI, read(_, _)).Times(0);
	EXPECT_CALL(mockSPI, readWrite(_, _, _)).Times(0);

	auto result = adc.rreg(Address::ID, 2u, nullptr);

	EXPECT_FALSE(result.has_value());
}

TEST_F(ADS124S08_Test, rregChecksCountRange) {
	EXPECT_CALL(mockSPI, write(_, _)).Times(0);
	EXPECT_CALL(mockSPI, read(_, _)).Times(0);
	EXPECT_CALL(mockSPI, readWrite(_, _, _)).Times(0);

	Register buffer[0x12u];

	auto resultLow = adc.rreg(Address::ID, 0x00u, buffer);
	EXPECT_FALSE(resultLow.has_value());

	auto resultHigh = adc.rreg(Address::ID, 0x13u, buffer);
	EXPECT_FALSE(resultHigh.has_value());
}

TEST_F(ADS124S08_Test, rregChecksAddressRange) {
	EXPECT_CALL(mockSPI, write(_, _)).Times(0);
	EXPECT_CALL(mockSPI, read(_, _)).Times(0);
	EXPECT_CALL(mockSPI, readWrite(_, _, _)).Times(0);

	auto result = adc.rreg(static_cast<Address>(0x12u));

	EXPECT_FALSE(result.has_value());

	Register buffer[2u];
	result = adc.rreg(static_cast<Address>(0x11u), 2u, buffer);
	EXPECT_FALSE(result.has_value());
}

TEST_F(ADS124S08_Test, rregReturnsNulloptWhenSpiFails) {
	const auto &testCase = rregCases[0u];
	mockSPI.disableSPI();

	Register readBuffer[testCase.count];

	auto result = adc.rreg(testCase.address, testCase.count, readBuffer);

	EXPECT_FALSE(result.has_value());
}

TEST_F(ADS124S08_Test, wregNormallyWritesExpectedValues) {
	mockSPI.delegateToFakes(nullptr);

	for (const auto &testCase : rregCases) {
		auto result = adc.wreg(testCase.address, testCase.count, testCase.expectedValues.data());

		EXPECT_TRUE(result.has_value());
		EXPECT_EQ(result.value(), testCase.expectedValues[0]);
	}
}

TEST_F(ADS124S08_Test, wregSingleByteOverloadWritesExpectedValue) {
	mockSPI.delegateToFakes(nullptr);

	for (const auto &testCase : rregCases) {
		if (testCase.count != 1u) {
			continue; // Only test single-byte writes here
		}

		auto result = adc.wreg(testCase.address, testCase.expectedValues[0]);

		EXPECT_TRUE(result.has_value());
		EXPECT_EQ(result.value(), testCase.expectedValues[0]);
	}
}

TEST_F(ADS124S08_Test, wregChecksForNullptrBuffer) {
	EXPECT_CALL(mockSPI, write(_, _)).Times(0);
	EXPECT_CALL(mockSPI, read(_, _)).Times(0);
	EXPECT_CALL(mockSPI, readWrite(_, _, _)).Times(0);

	auto result = adc.wreg(Address::ID, 2u, nullptr);

	EXPECT_FALSE(result.has_value());
}

TEST_F(ADS124S08_Test, wregChecksCountRange) {
	EXPECT_CALL(mockSPI, write(_, _)).Times(0);
	EXPECT_CALL(mockSPI, read(_, _)).Times(0);
	EXPECT_CALL(mockSPI, readWrite(_, _, _)).Times(0);

	Register buffer[0x12u] = {0};

	auto resultLow = adc.wreg(Address::ID, 0x00u, buffer);
	EXPECT_FALSE(resultLow.has_value());

	auto resultHigh = adc.wreg(Address::ID, 0x13u, buffer);
	EXPECT_FALSE(resultHigh.has_value());
}

TEST_F(ADS124S08_Test, wregChecksAddressRange) {
	EXPECT_CALL(mockSPI, write(_, _)).Times(0);
	EXPECT_CALL(mockSPI, read(_, _)).Times(0);
	EXPECT_CALL(mockSPI, readWrite(_, _, _)).Times(0);

	Register buffer[2u] = {0};

	auto result = adc.wreg(static_cast<Address>(0x12u), 1u, buffer);
	EXPECT_FALSE(result.has_value());
	result = adc.wreg(static_cast<Address>(0x12u), 0x00u);
	EXPECT_FALSE(result.has_value());
}

TEST_F(ADS124S08_Test, wregReturnsNulloptWhenSpiFails) {
	const auto &testCase = rregCases[0u];
	mockSPI.disableSPI();

	auto result = adc.wreg(testCase.address, testCase.count, testCase.expectedValues.data());

	EXPECT_FALSE(result.has_value());
}

TEST_F(ADS124S08_Test, singleByteCommandsNormallySucceed) {
	for (const auto &cmdPair : singleByteCommands) {
		EXPECT_CALL(mockSPI, write(_, Eq(1u))).WillOnce(Return(1u));

		const auto result = cmdPair.second();
		EXPECT_TRUE(result.has_value());
		EXPECT_EQ(result.value(), static_cast<Register>(cmdPair.first));
	}
}

TEST_F(ADS124S08_Test, wakeupReturnsNulloptWhenSpiFails) {
	for (const auto &cmdPair : singleByteCommands) {
		EXPECT_CALL(mockSPI, write(_, Eq(1u))).WillOnce(Return(nullopt));
		const auto result = cmdPair.second();
		EXPECT_FALSE(result.has_value());
	}
}

TEST_F(ADS124S08_Test, setRegistersNormallySucceedsAndReturnsWrittenValue) {
	class MockStructRegister : public ADS124S08::SPI_Register_I {
	private:
		Register regValue;

	public:
		MockStructRegister(Register val) : regValue(val) {}

		virtual Register toRegister(void) const override { return regValue; }
		virtual Address	 getAddress(void) const override { return static_cast<Address>(0x00u); }
		virtual Register getResetValue(void) const override { return 0xFFu; }
	};

	MockStructRegister reg1(0xA5u);

	mockSPI.delegateToFakes(nullptr);

	EXPECT_CALL(mockSPI, write(_, _)).Times(1);

	auto result = adc.setRegister(reg1);
	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(result.value(), 0xA5u);
}

TEST_F(ADS124S08_Test, rdataNormallyReadsExpectedValues) {
	std::array<Register, 3u> fakeData = {0x12u, 0x34u, 0x56u};

	mockSPI.delegateToFakes(fakeData.data());

	EXPECT_CALL(mockSPI, write(_, Eq(1u))).Times(1);
	EXPECT_CALL(mockSPI, read(_, Eq(3u))).Times(1);
	EXPECT_CALL(mockSPI, readWrite(_, _, _)).Times(0);

	auto result = adc.rdata();
	EXPECT_TRUE(result.has_value());
	EXPECT_FALSE(result->status.has_value());
	EXPECT_FALSE(result->crc.has_value());
	EXPECT_EQ(result->data, 0x123456);
}

TEST_F(ADS124S08_Test, rdataWithStatusReturnsExpectedValues) {
	std::array<Register, 4u> fakeData = {0xABu, 0x12u, 0x34u, 0x56u};

	mockSPI.delegateToFakes(fakeData.data());

	EXPECT_CALL(mockSPI, write(_, Eq(1u))).Times(1);
	EXPECT_CALL(mockSPI, read(_, Eq(4u))).Times(1);
	EXPECT_CALL(mockSPI, readWrite(_, _, _)).Times(0);

	auto result = adc.rdata(true, false);
	EXPECT_TRUE(result.has_value());
	EXPECT_TRUE(result->status.has_value());
	EXPECT_EQ(result->status.value(), 0xABu);
	EXPECT_FALSE(result->crc.has_value());
	EXPECT_EQ(result->data, 0x123456);
}

TEST_F(ADS124S08_Test, rdataWithCrcReturnsExpectedValues) {
	std::array<Register, 4u> fakeData = {0x12u, 0x34u, 0x56u, 0xCDu};

	mockSPI.delegateToFakes(fakeData.data());

	EXPECT_CALL(mockSPI, write(_, Eq(1u))).Times(1);
	EXPECT_CALL(mockSPI, read(_, Eq(4u))).Times(1);
	EXPECT_CALL(mockSPI, readWrite(_, _, _)).Times(0);

	auto result = adc.rdata(false, true);
	EXPECT_TRUE(result.has_value());
	EXPECT_FALSE(result->status.has_value());
	EXPECT_TRUE(result->crc.has_value());
	EXPECT_EQ(result->crc.value(), 0xCDu);
	EXPECT_EQ(result->data, 0x123456);
}

TEST_F(ADS124S08_Test, rdataWithStatusAndCrcReturnsExpectedValues) {
	std::array<Register, 5u> fakeData = {0xABu, 0x12u, 0x34u, 0x56u, 0xCDu};

	mockSPI.delegateToFakes(fakeData.data());

	EXPECT_CALL(mockSPI, write(_, Eq(1u))).Times(1);
	EXPECT_CALL(mockSPI, read(_, Eq(5u))).Times(1);
	EXPECT_CALL(mockSPI, readWrite(_, _, _)).Times(0);

	auto result = adc.rdata(true, true);
	EXPECT_TRUE(result.has_value());
	EXPECT_TRUE(result->status.has_value());
	EXPECT_EQ(result->status.value(), 0xABu);
	EXPECT_TRUE(result->crc.has_value());
	EXPECT_EQ(result->crc.value(), 0xCDu);
	EXPECT_EQ(result->data, 0x123456);
}

TEST_F(ADS124S08_Test, rdataReturnsNulloptWhenSpiFails) {
	mockSPI.disableSPI();

	auto result = adc.rdata();
	EXPECT_FALSE(result.has_value());
}

TEST_F(ADS124S08_Test, getSystemControlNormallyReturnsExpectedValue) {
	Register fakeSysRegValue = 0x5Au;

	mockSPI.delegateToFakes(&fakeSysRegValue);

	EXPECT_CALL(mockSPI, write(_, Eq(2u))).Times(1);
	EXPECT_CALL(mockSPI, read(_, Eq(1u))).Times(1);
	EXPECT_CALL(mockSPI, readWrite(_, _, _)).Times(0);

	auto result = adc.getSystemControl();
	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(result->toRegister(), fakeSysRegValue);
}

TEST_F(ADS124S08_Test, getSystemControlReturnsNulloptWhenSpiFails) {
	mockSPI.disableSPI();

	auto result = adc.getSystemControl();
	EXPECT_FALSE(result.has_value());
}

TEST_F(ADS124S08_Test, getSystemControlUpdatesSysCache) {
	Register fakeSysRegValue = 0xA5u;

	mockSPI.delegateToFakes(&fakeSysRegValue);

	adc.sysCache = 0x00u; // Set to known value
	adc.getSystemControl();
	EXPECT_EQ(adc.sysCache, fakeSysRegValue);
}
