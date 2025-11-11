#include "gtest/gtest.h"

#define ADS124S08_GTEST_TESTING

#include "../Src/ADS124S08_REF.cpp"

using REF	   = ADS124S08::REF;
using Register = ADS124S08::Register;

// class ADS124S08_REF_Test : public ::testing::Test {};

TEST(ADS124S08_REF_Test, toRegister_PacksFieldsCorrectly) {
	const std::pair<Register, REF> testCases[] = {
		{0x00u, REF({0b00u, 0b0u, 0b0u, 0b00u, 0b00u})},
		{0x80u, REF({0b10u, 0b0u, 0b0u, 0b00u, 0b00u})},
		{0xFFu, REF({0b11u, 0b1u, 0b1u, 0b11u, 0b11u})},
		{0x55u, REF({0b01u, 0b0u, 0b1u, 0b01u, 0b01u})},
		{0xAAu, REF({0b10u, 0b1u, 0b0u, 0b10u, 0b10u})},
	};

	for (const auto &testCase : testCases) {
		EXPECT_EQ((Register)testCase.first, testCase.second.toRegister());
	}
}

TEST(ADS124S08_REF_Test, constructor_InitializesFieldsCorrectly_FromRegister) {
	const std::pair<Register, std::array<Register, 5>> testCases[] = {
		{0x00u, {0b00u, 0b0u, 0b0u, 0b00u, 0b00u}},
		{0x01u, {0b00u, 0b0u, 0b0u, 0b00u, 0b01u}},
		{0x80u, {0b10u, 0b0u, 0b0u, 0b00u, 0b00u}},
		{0xFFu, {0b11u, 0b1u, 0b1u, 0b11u, 0b11u}},
		{0x55u, {0b01u, 0b0u, 0b1u, 0b01u, 0b01u}},
		{0xAAu, {0b10u, 0b1u, 0b0u, 0b10u, 0b10u}},
	};

	for (const auto &testCase : testCases) {
		const REF &ref = REF(testCase.first);
		EXPECT_EQ(testCase.second[0], ref.FL_REF_EN);
		EXPECT_EQ(testCase.second[1], ref.REFP_BUF);
		EXPECT_EQ(testCase.second[2], ref.REFN_BUF);
		EXPECT_EQ(testCase.second[3], ref.REFSEL);
		EXPECT_EQ(testCase.second[4], ref.REFCON);
	}
}

Register fl_ref_en(Register r) {
	return (r >> 6U) & 0x03u;
}

TEST(ADS124S08_REF_Test, setReferenceMonitorConfigSetsFieldCorrectly) {
	REF	 ref(0x00u);
	auto fl_ref_en = [](Register r) { return (r >> 6U) & 0x03u; };
	auto nMask	   = ~static_cast<Register>(0b11u << 6u);

	ref.setReferenceMonitorConfig(REF::ReferenceMonitorConfig::DISABLED);
	EXPECT_EQ(0b00u, fl_ref_en(ref.toRegister()));
	EXPECT_EQ(0u, ref.toRegister() & nMask);

	ref.setReferenceMonitorConfig(REF::ReferenceMonitorConfig::LEVEL_0_ONLY);
	EXPECT_EQ(0b01u, fl_ref_en(ref.toRegister()));
	EXPECT_EQ(0u, ref.toRegister() & nMask);

	ref.setReferenceMonitorConfig(REF::ReferenceMonitorConfig::LEVEL_0_LEVEL1);
	EXPECT_EQ(0b10u, fl_ref_en(ref.toRegister()));
	EXPECT_EQ(0u, ref.toRegister() & nMask);

	ref.setReferenceMonitorConfig(REF::ReferenceMonitorConfig::LEVEL_0_10MR_PULL_TOGETHER);
	EXPECT_EQ(0b11u, fl_ref_en(ref.toRegister()));
	EXPECT_EQ(0u, ref.toRegister() & nMask);

	ref.setReferenceMonitorConfig(REF::ReferenceMonitorConfig::DISABLED);
	EXPECT_EQ(0b00u, fl_ref_en(ref.toRegister()));
	EXPECT_EQ(0u, ref.toRegister() & nMask);
}

static void testSetRefBufferBypass(
	REF													&ref,
	std::function<REF &(REF &, REF::BufferBypassConfig)> setRefBufferBypass,
	Register											 bitOffset
) {
	auto ref_buf = [bitOffset](Register r) { return (r >> bitOffset) & 0x01u; };
	auto nMask	 = ~static_cast<Register>(0b1u << bitOffset);

	setRefBufferBypass(ref, REF::BufferBypassConfig::ENABLED);
	EXPECT_EQ(0b0u, ref_buf(ref.toRegister()));
	EXPECT_EQ(0u, ref.toRegister() & nMask);

	setRefBufferBypass(ref, REF::BufferBypassConfig::DISABLED);
	EXPECT_EQ(0b1u, ref_buf(ref.toRegister()));
	EXPECT_EQ(0u, ref.toRegister() & nMask);
}

TEST(ADS124S08_REF_Test, setPositiveRefBufferBypassSetsFieldCorrectly) {
	REF													 ref(0x00u);
	std::function<REF &(REF &, REF::BufferBypassConfig)> call =
		[](REF &ref, REF::BufferBypassConfig cfg) -> REF & {
		return ref.setPositiveRefBufferBypass(cfg);
	};

	testSetRefBufferBypass(
		ref,
		call,
		5u // Pass the correct bitOffset value
	);
}

TEST(ADS124S08_REF_Test, setNegativeRefBufferBypassSetsFieldCorrectly) {
	REF													 ref(0x00u);
	std::function<REF &(REF &, REF::BufferBypassConfig)> call =
		[](REF &ref, REF::BufferBypassConfig cfg) -> REF & {
		return ref.setNegativeRefBufferBypass(cfg);
	};

	testSetRefBufferBypass(
		ref,
		call,
		4u // Pass the correct bitOffset value
	);
}

TEST(ADS124S08_REF_Test, setReferenceInputSelectionSetsFieldsCorrectly) {
	REF	 ref(0x00u);
	auto refsel = [](Register r) { return (r >> 2U) & 0x03u; };
	auto nMask	= ~static_cast<Register>(0b11u << 2u);

	ref.setReferenceInputSelection(REF::InternalReferenceSelect::REFP0_REFN0, false);
	EXPECT_EQ(0b00u, refsel(ref.toRegister()));
	EXPECT_EQ(0u, ref.toRegister() & nMask);

	ref.setReferenceInputSelection(REF::InternalReferenceSelect::REFP1_REFN0, false);
	EXPECT_EQ(0b01u, refsel(ref.toRegister()));
	EXPECT_EQ(0u, ref.toRegister() & nMask);

	ref.setReferenceInputSelection(REF::InternalReferenceSelect::INTERNAL, false);
	EXPECT_EQ(0b10u, refsel(ref.toRegister()));
	EXPECT_EQ(0u, ref.toRegister() & nMask);

	ref.setReferenceInputSelection(REF::InternalReferenceSelect::REFP0_REFN0, false);
	EXPECT_EQ(0b00u, refsel(ref.toRegister()));
	EXPECT_EQ(0u, ref.toRegister() & nMask);
}

TEST(
	ADS124S08_REF_Test,
	setReferenceInputSelection_byDefaultDisablesRefBuffersWhenInternalSelected
) {
	REF ref(0x00u);

	ref.setPositiveRefBufferBypass(REF::BufferBypassConfig::ENABLED);
	ref.setNegativeRefBufferBypass(REF::BufferBypassConfig::ENABLED);

	ref.setReferenceInputSelection(REF::InternalReferenceSelect::INTERNAL);

	EXPECT_EQ(0b10u, (ref.toRegister() >> 2u) & 0x03u);
	EXPECT_EQ(0b1u, (ref.toRegister() >> 5u) & 0x01u);
	EXPECT_EQ(0b1u, (ref.toRegister() >> 4u) & 0x01u);

	ref = REF(0x00u); // Reset
	ref.setPositiveRefBufferBypass(REF::BufferBypassConfig::ENABLED);
	ref.setNegativeRefBufferBypass(REF::BufferBypassConfig::ENABLED);
	ref.setReferenceInputSelection(REF::InternalReferenceSelect::INTERNAL, false);

	EXPECT_EQ(0b10u, (ref.toRegister() >> 2u) & 0x03u);
	EXPECT_NE(0b1u, (ref.toRegister() >> 5u) & 0x01u);
	EXPECT_NE(0b1u, (ref.toRegister() >> 4u) & 0x01u);
}

TEST(ADS124S08_REF_Test, setInternalReferenceVoltageConfigSetsFieldCorrectly) {
	REF	 ref(0x00u);
	auto refcon = [](Register r) { return (r >> 0U) & 0x03u; };
	auto nMask	= ~static_cast<Register>(0b11u << 0u);

	ref.setInternalReferenceVoltageConfig(REF::IntRefVoltConfig::OFF);
	EXPECT_EQ(0b00u, refcon(ref.toRegister()));
	EXPECT_EQ(0u, ref.toRegister() & nMask);

	ref.setInternalReferenceVoltageConfig(REF::IntRefVoltConfig::ON_POWERUP);
	EXPECT_EQ(0b01u, refcon(ref.toRegister()));
	EXPECT_EQ(0u, ref.toRegister() & nMask);

	ref.setInternalReferenceVoltageConfig(REF::IntRefVoltConfig::ON_ALWAYS);
	EXPECT_EQ(0b10u, refcon(ref.toRegister()));
	EXPECT_EQ(0u, ref.toRegister() & nMask);

	ref.setInternalReferenceVoltageConfig(REF::IntRefVoltConfig::OFF);
	EXPECT_EQ(0b00u, refcon(ref.toRegister()));
	EXPECT_EQ(0u, ref.toRegister() & nMask);
}

TEST(ADS124S08_REF_Test, getAddressReturnsExpectedValue) {
	REF ref{};
	EXPECT_EQ(0x05u, ref.getAddress());
}

TEST(ADS124S08_REF_Test, getResetValueReturnsExpectedValue) {
	REF ref{};
	EXPECT_EQ(0x10u, ref.getResetValue());
}
