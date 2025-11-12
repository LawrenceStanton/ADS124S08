#include "gtest/gtest.h"

#define ADS124S08_GTEST_TESTING

#include "../Src/INPMUX.cpp"

using INPMUX   = ADS124S08::INPMUX;
using Register = ADS124S08::Register;

TEST(INPMUX_Test, toRegister_PacksFieldsCorrectly) {
	const std::pair<Register, INPMUX> testCases[] = {
		{0x00u, INPMUX({0b0000u, 0b0000u})},
		{0xFFu, INPMUX({0b1111u, 0b1111u})},
		{0x5Au, INPMUX({0b0101u, 0b1010u})},
		{0xA5u, INPMUX({0b1010u, 0b0101u})},
	};

	for (const auto &testCase : testCases) {
		EXPECT_EQ((Register)testCase.first, testCase.second.toRegister());
	}
}

TEST(INPMUX_Test, constructor_InitializesFieldsCorrectly_FromRegister) {
	const std::pair<Register, std::array<Register, 2>> testCases[] = {
		{0x00u, {0b0000u, 0b0000u}},
		{0xFFu, {0b1111u, 0b1111u}},
		{0x5Au, {0b0101u, 0b1010u}},
		{0xA5u, {0b1010u, 0b0101u}},
	};

	for (const auto &testCase : testCases) {
		const INPMUX &inpmux = INPMUX(testCase.first);
		EXPECT_EQ(testCase.second[0], inpmux.MUXP);
		EXPECT_EQ(testCase.second[1], inpmux.MUXN);
	}
}

void testSetInputChannel(
	INPMUX												  &inpmux,
	std::function<INPMUX &(INPMUX &, INPMUX::InputSelect)> setInputChannel,
	Register											   bitOffset
) {
	auto mux_field = [bitOffset](Register r) { return (r >> bitOffset) & 0x0Fu; };
	auto nMask	   = ~static_cast<Register>(0x0Fu << bitOffset);

	constexpr auto testCases = std::array<std::pair<INPMUX::InputSelect, Register>, 4>{{
		{INPMUX::InputSelect::AIN0, 0b0000u},
		{INPMUX::InputSelect::AIN5, 0b0101u},
		{INPMUX::InputSelect::AIN10, 0b1010u},
		{INPMUX::InputSelect::AINCOM, 0b1100u},
	}};

	for (const auto &testCase : testCases) {
		setInputChannel(inpmux, testCase.first);
		EXPECT_EQ(testCase.second, mux_field(inpmux.toRegister()));
		EXPECT_EQ(0u, inpmux.toRegister() & nMask);
	}
}

TEST(INPMUX_Test, setPositiveInputChannelSetsFieldCorrectly) {
	INPMUX												   inpmux(0x00u);
	std::function<INPMUX &(INPMUX &, INPMUX::InputSelect)> call =
		[](INPMUX &inpmux, INPMUX::InputSelect sel) -> INPMUX & {
		return inpmux.setPositiveInputChannel(sel);
	};

	testSetInputChannel(
		inpmux,
		call,
		4u // Pass the correct bit offset for MUXP
	);
}

TEST(INPMUX_Test, setNegativeInputChannelSetsFieldCorrectly) {
	INPMUX												   inpmux(0x00u);
	std::function<INPMUX &(INPMUX &, INPMUX::InputSelect)> call =
		[](INPMUX &inpmux, INPMUX::InputSelect sel) -> INPMUX & {
		return inpmux.setNegativeInputChannel(sel);
	};

	testSetInputChannel(
		inpmux,
		call,
		0u // Pass the correct bit offset for MUXN
	);
}

TEST(INPMUX_Test, settersReturnReferenceToSelf) {
	INPMUX inpmux(0x00u);

	INPMUX &ref1 = inpmux.setPositiveInputChannel(INPMUX::InputSelect::AIN3);
	EXPECT_EQ(&inpmux, &ref1);

	INPMUX &ref2 = inpmux.setNegativeInputChannel(INPMUX::InputSelect::AIN7);
	EXPECT_EQ(&inpmux, &ref2);
}

TEST(INPMUX_Test, getAddressReturnsCorrectAddress) {
	EXPECT_EQ(0x02u, INPMUX().getAddress());
}

TEST(INPMUX_Test, getResetValueReturnsCorrectResetValue) {
	EXPECT_EQ(0x01u, INPMUX().getResetValue());
}
