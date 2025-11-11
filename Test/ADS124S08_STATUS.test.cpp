#include "gtest/gtest.h"

#define ADS124S08_GTEST_TESTING

#include "../Src/ADS124S08_STATUS.cpp"

using STATUS   = ADS124S08::STATUS;
using Register = ADS124S08::Register;

TEST(ADS124S08_STATUS_Test, toRegister_AlwaysReturnsZero) {
	const STATUS testCases[] = {
		STATUS(0x00u),
		STATUS(0x80u),
		STATUS(0xFFu),
		STATUS(0x55u),
		STATUS(0xAAu),
	};

	for (const auto &testCase : testCases) {
		EXPECT_EQ(0x00u, testCase.toRegister());
	}
}

TEST(ADS124S08_STATUS_Test, constructor_InitializesFieldsCorrectly_FromRegister) {
	const std::pair<Register, std::array<Register, 8>> testCases[] = {
		{0x00u, {0b0u, 0b0u, 0b0u, 0b0u, 0b0u, 0b0u, 0b0u, 0b0u}},
		{0x01u, {0b0u, 0b0u, 0b0u, 0b0u, 0b0u, 0b0u, 0b0u, 0b1u}},
		{0x80u, {0b1u, 0b0u, 0b0u, 0b0u, 0b0u, 0b0u, 0b0u, 0b0u}},
		{0xFFu, {0b1u, 0b1u, 0b1u, 0b1u, 0b1u, 0b1u, 0b1u, 0b1u}},
		{0x55u, {0b0u, 0b1u, 0b0u, 0b1u, 0b0u, 0b1u, 0b0u, 0b1u}},
		{0xAAu, {0b1u, 0b0u, 0b1u, 0b0u, 0b1u, 0b0u, 0b1u, 0b0u}},
	};

	for (const auto &[input, expected] : testCases) {
		SCOPED_TRACE(::testing::Message() << "Input: " << std::hex << static_cast<int>(input));
		STATUS status{input};
		EXPECT_EQ(expected[0], status.FL_POR);
		EXPECT_EQ(expected[1], status.RDY);
		EXPECT_EQ(expected[2], status.FL_P_RAILP);
		EXPECT_EQ(expected[3], status.FL_P_RAILN);
		EXPECT_EQ(expected[4], status.FL_N_RAILP);
		EXPECT_EQ(expected[5], status.FL_N_RAILN);
		EXPECT_EQ(expected[6], status.FL_REF_L1);
		EXPECT_EQ(expected[7], status.FL_REF_L0);
	}
}

TEST(ADS124S08_STATUS_Test, gettersReturnExpectedEnumValues) {
	STATUS reg{0x00};

	EXPECT_EQ(reg.get_FL_POR(), STATUS::POR_Flag::CLEARED);
	EXPECT_EQ(reg.get_RDY(), STATUS::Dev_RDY::READY);
	EXPECT_EQ(reg.get_FL_P_RAILP(), STATUS::FL_RAIL::NO_ERROR);
	EXPECT_EQ(reg.get_FL_P_RAILN(), STATUS::FL_RAIL::NO_ERROR);
	EXPECT_EQ(reg.get_FL_N_RAILP(), STATUS::FL_RAIL::NO_ERROR);
	EXPECT_EQ(reg.get_FL_N_RAILN(), STATUS::FL_RAIL::NO_ERROR);
	EXPECT_EQ(reg.get_FL_REF_L1(), STATUS::FL_REF_LVL::EXCEEDED);
	EXPECT_EQ(reg.get_FL_REF_L0(), STATUS::FL_REF_LVL::EXCEEDED);

	STATUS reg2(0xFFu);
	EXPECT_EQ(reg2.get_FL_POR(), STATUS::POR_Flag::NOT_CLEARED);
	EXPECT_EQ(reg2.get_RDY(), STATUS::Dev_RDY::NOT_READY);
	EXPECT_EQ(reg2.get_FL_P_RAILP(), STATUS::FL_RAIL::ERROR);
	EXPECT_EQ(reg2.get_FL_P_RAILN(), STATUS::FL_RAIL::ERROR);
	EXPECT_EQ(reg2.get_FL_N_RAILP(), STATUS::FL_RAIL::ERROR);
	EXPECT_EQ(reg2.get_FL_N_RAILN(), STATUS::FL_RAIL::ERROR);
	EXPECT_EQ(reg2.get_FL_REF_L1(), STATUS::FL_REF_LVL::NOT_EXCEEDED);
	EXPECT_EQ(reg2.get_FL_REF_L0(), STATUS::FL_REF_LVL::NOT_EXCEEDED);
}

TEST(ADS124S08_STATUS_Test, getAddressReturnsExpectedValue) {
	STATUS status{};
	EXPECT_EQ(0x01u, status.getAddress());
}

TEST(ADS124S08_STATUS_Test, getResetValueReturnsExpectedValue) {
	STATUS status{};
	EXPECT_EQ(0x80u, status.getResetValue());
}
