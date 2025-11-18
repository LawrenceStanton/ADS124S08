#include "gtest/gtest.h"

#define ADS124S08_GTEST_TESTING

#include "../Src/PGA.cpp"

#include "ADS124S08.hpp"

using PGA	   = ADS124S08::PGA;
using Register = ADS124S08::Register;

TEST(PGA_Test, toRegister_PacksFieldsCorrectly) {
	const std::pair<Register, PGA> testCases[] = {
		{0x00u, PGA({0b000u, 0b00u, 0b000u})},
		{0xEFu, PGA({0b111u, 0b01u, 0b111u})}, // 0b11 for PGA_EN is reserved
		{0x55u, PGA({0b010u, 0b10u, 0b101u})},
		{0xAAu, PGA({0b101u, 0b01u, 0b010u})},
	};

	for (const auto &testCase : testCases) {
		EXPECT_EQ((Register)testCase.first, testCase.second.toRegister());
	}
}

TEST(PGA_Test, constructor_InitializesFieldsCorrectly_FromRegister) {
	const std::pair<Register, std::array<Register, 3>> testCases[] = {
		{0x00u, {0b000u, 0b00u, 0b000u}},
		{0xFFu, {0b111u, 0b11u, 0b111u}},
		{0x55u, {0b010u, 0b10u, 0b101u}},
		{0xAAu, {0b101u, 0b01u, 0b010u}},
	};

	for (const auto &[reg, expected] : testCases) {
		PGA pga(reg);
		EXPECT_EQ(expected[0], pga.DELAY);
		EXPECT_EQ(expected[1], pga.PGA_EN);
		EXPECT_EQ(expected[2], pga.GAIN);
	}
}

TEST(PGA_Test, setDelaySetsFieldCorrectly) {
	PGA pga(0x00u);

	auto getDelayField = [](Register r) { return (r >> 5U) & 0b111u; };

	pga.setDelay(PGA::CONVERSION_DELAY::DELAY_14);
	EXPECT_EQ(0b000u, getDelayField(pga.toRegister()));

	pga.setDelay(PGA::CONVERSION_DELAY::DELAY_256);
	EXPECT_EQ(0b011u, getDelayField(pga.toRegister()));

	pga.setDelay(PGA::CONVERSION_DELAY::DELAY_1);
	EXPECT_EQ(0b111u, getDelayField(pga.toRegister()));
}

TEST(PGA_Test, setEnableSetsFieldsCorrectly) {
	PGA	 pga{};
	auto getEnableField = [](Register r) { return (r >> 3u) & 0b11u; };

	pga.setEnable(PGA::ENABLE::BYPASSED);
	EXPECT_EQ(0b00u, getEnableField(pga.toRegister()));

	pga.setEnable(PGA::ENABLE::ENABLED);
	EXPECT_EQ(0b01u, getEnableField(pga.toRegister()));
}

TEST(PGA_Test, setEnable_SetsGainToUnityWhenBypassedIfFlagIsSet) {
	PGA	 pga{};
	auto getGainField	= [](Register r) { return (r >> 0u) & 0b111u; };
	auto getEnableField = [](Register r) { return (r >> 3u) & 0b11u; };

	pga.setGain(PGA::GAIN_SELECT::GAIN_16, false).setEnable(PGA::ENABLE::ENABLED, false);
	EXPECT_EQ(0b100u, getGainField(pga.toRegister()));
	EXPECT_EQ(0b01u, getEnableField(pga.toRegister()));

	pga.setEnable(PGA::ENABLE::BYPASSED);
	EXPECT_EQ(0b000u, getGainField(pga.toRegister()));
	EXPECT_EQ(0b00u, getEnableField(pga.toRegister()));
}

TEST(PGA_Test, setGainSetsFieldsCorrectly) {
	PGA pga{};

	auto getGainField = [](Register r) { return (r >> 0u) & 0b111u; };

	const std::pair<PGA::GAIN_SELECT, Register> testCases[] = {
		{PGA::GAIN_SELECT::GAIN_1, 0b000u},
		{PGA::GAIN_SELECT::GAIN_16, 0b100u},
		{PGA::GAIN_SELECT::GAIN_128, 0b111u},
	};

	for (const auto &[gain, expected] : testCases) {
		pga.setGain(gain, false);
		EXPECT_EQ(expected, getGainField(pga.toRegister()));
	}
}

TEST(PGA_Test, setGain_SetsPGAEnabledWhenGainNotUnityIfFlagIsSet) {
	PGA	 pga{};
	auto getGainField	= [](Register r) { return (r >> 0u) & 0b111u; };
	auto getEnableField = [](Register r) { return (r >> 3u) & 0b11u; };

	pga.setEnable(PGA::ENABLE::BYPASSED, false).setGain(PGA::GAIN_SELECT::GAIN_1, false);
	EXPECT_EQ(0b000u, getGainField(pga.toRegister()));
	EXPECT_EQ(0b00u, getEnableField(pga.toRegister()));

	pga.setGain(PGA::GAIN_SELECT::GAIN_32);
	EXPECT_EQ(0b101u, getGainField(pga.toRegister()));
	EXPECT_EQ(0b01u, getEnableField(pga.toRegister()));
}

TEST(PGA_Test, address_returnsCorrectValue) {
	PGA pga;
	EXPECT_EQ(0x03u, pga.getAddress());
}

TEST(PGA_Test, getResetValue_returnsCorrectValue) {
	PGA pga;
	EXPECT_EQ(0x00u, pga.getResetValue());
}
