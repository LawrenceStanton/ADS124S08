#include "gtest/gtest.h"

#define ADS124S08_GTEST_TESTING

#include "../Src/SYS.cpp"

using SYS	   = ADS124S08::SYS;
using Register = ADS124S08::Register;

TEST(SYS_Test, toRegister_PacksFieldsCorrectly) {
	const std::pair<Register, SYS> testCases[] = {
		{0x00u, SYS({0b000u, 0b00u, 0b0u, 0b0u, 0b0u})},
		{0xFFu, SYS({0b111u, 0b11u, 0b1u, 0b1u, 0b1u})},
		{0x5Au, SYS({0b010u, 0b11u, 0b0u, 0b1u, 0b0u})},
		{0xA5u, SYS({0b101u, 0b00u, 0b1u, 0b0u, 0b1u})},
	};

	for (const auto &testCase : testCases) {
		EXPECT_EQ((Register)testCase.first, testCase.second.toRegister());
	}
}

TEST(SYS_Test, constructor_InitializesFieldsCorrectly_FromRegister) {
	const std::pair<Register, std::array<Register, 5>> testCases[] = {
		{0x00u, {0b000u, 0b00u, 0b0u, 0b0u, 0b0u}},
		{0xFFu, {0b111u, 0b11u, 0b1u, 0b1u, 0b1u}},
		{0x5Au, {0b010u, 0b11u, 0b0u, 0b1u, 0b0u}},
		{0xA5u, {0b101u, 0b00u, 0b1u, 0b0u, 0b1u}},
	};

	for (const auto &testCase : testCases) {
		const SYS &sys = SYS(testCase.first);
		EXPECT_EQ(testCase.second[0], sys.SYS_MON);
		EXPECT_EQ(testCase.second[1], sys.CAL_SAMP);
		EXPECT_EQ(testCase.second[2], sys.TIMEOUT);
		EXPECT_EQ(testCase.second[3], sys.CRC_EN);
		EXPECT_EQ(testCase.second[4], sys.SENDSTAT);
	}
}

TEST(SYS_Test, setSystemMonitorConfig_SetsFieldCorrectly) {
	const std::pair<SYS::SystemMonitorConfig, Register> testCases[] = {
		{SYS::SystemMonitorConfig::DISABLED, 0b000u},
		{SYS::SystemMonitorConfig::PGA_SHORT, 0b001u},
		{SYS::SystemMonitorConfig::INT_TEMP, 0b010u},
		{SYS::SystemMonitorConfig::SUPPLY_A, 0b011u},
		{SYS::SystemMonitorConfig::SUPPLY_B, 0b100u},
		{SYS::SystemMonitorConfig::BURNOUT_0U2, 0b101u},
		{SYS::SystemMonitorConfig::BURNOUT_1U0, 0b110u},
		{SYS::SystemMonitorConfig::BURNOUT_10U, 0b111u},
	};

	for (const auto &testCase : testCases) {
		SYS sys{0x00u};
		sys.setSystemMonitorConfig(testCase.first);
		EXPECT_EQ(testCase.second, sys.toRegister() >> 5u & 0b111u);
	}
}

TEST(SYS_Test, setCalibrationSampleSize_SetsFieldCorrectly) {
	const std::pair<SYS::CalSampleSize, Register> testCases[] = {
		{SYS::CalSampleSize::AVG_1_SAMPLES, 0b00u},
		{SYS::CalSampleSize::AVG_4_SAMPLES, 0b01u},
		{SYS::CalSampleSize::AVG_8_SAMPLES, 0b10u},
		{SYS::CalSampleSize::AVG_16_SAMPLES, 0b11u},
	};

	for (const auto &testCase : testCases) {
		SYS sys{0x00u};
		sys.setCalibrationSampleSize(testCase.first);
		EXPECT_EQ(testCase.second, (sys.toRegister() >> 3u) & 0b11u);
	}
}

TEST(SYS_Test, setTimeout_SetsFieldCorrectly) {
	SYS sys{0x00u};

	sys.setTimeout(false);
	EXPECT_EQ(0b0u, (sys.toRegister() >> 2u) & 0b1u);

	sys.setTimeout(true);
	EXPECT_EQ(0b1u, (sys.toRegister() >> 2u) & 0b1u);
}

TEST(SYS_Test, getSetCRCReturnsCorrectValue) {
	SYS sys{0x00u};

	sys.setCRCEnable(false);
	EXPECT_EQ(false, sys.crc());

	sys.setCRCEnable(true);
	EXPECT_EQ(true, sys.crc());
}

TEST(SYS_Test, getSetSendStatusReturnsCorrectValue) {
	SYS sys{0x00u};

	sys.setSendStatus(false);
	EXPECT_EQ(false, sys.sendStat());

	sys.setSendStatus(true);
	EXPECT_EQ(true, sys.sendStat());
}

TEST(SYS_Test, getAddressReturnsCorrectValue) {
	EXPECT_EQ(0x09u, SYS().getAddress());
}

TEST(SYS_Test, getResetValueReturnsCorrectValue) {
	EXPECT_EQ(0x10u, SYS().getResetValue());
}

// TEST(SYS_Test, settersReturnReferenceToSelf) {
// 	SYS sys{0x00u};

// 	SYS &ref1 = sys.setSystemMonitorConfig(SYS::SystemMonitorConfig::SUPPLY_A);
// 	EXPECT_EQ(&sys, &ref1);

// 	SYS &ref2 = sys.setCalibrationSampleSize(SYS::CalSampleSize::AVG_16_SAMPLES);
// 	EXPECT_EQ(&sys, &ref2);

// 	SYS &ref3 = sys.setTimeout(true);
// 	EXPECT_EQ(&sys, &ref3);

// 	SYS &ref4 = sys.setCRCEnable(true);
// 	EXPECT_EQ(&sys, &ref4);

// 	SYS &ref5 = sys.setSendStatus(true);
// 	EXPECT_EQ(&sys, &ref5);
// }
