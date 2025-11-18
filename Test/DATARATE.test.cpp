#include "gtest/gtest.h"

#include "ADS124S08.hpp"

using Register = ADS124S08::Register;
using DATARATE = ADS124S08::DATARATE;

TEST(DATARATE_Test, constructor_InitializesFieldsCorrectly_FromRegister) {
	const Register testValue = 0b10110010u;

	const std::pair<
		Register,
		std::tuple<
			ADS124S08::DATARATE::ChopperEnable,
			ADS124S08::DATARATE::ClockSelect,
			ADS124S08::DATARATE::ModeSelect,
			ADS124S08::DATARATE::FilterSelect,
			ADS124S08::DATARATE::DataRate>>
		testCases[] = {
			{0x00u,
			 {ADS124S08::DATARATE::ChopperEnable::DISABLED,
			  ADS124S08::DATARATE::ClockSelect::INTERNAL,
			  ADS124S08::DATARATE::ModeSelect::CONTINUOUS,
			  ADS124S08::DATARATE::FilterSelect::SINC3,
			  ADS124S08::DATARATE::DataRate::RATE_2_5}},
			{0xFDu, // DR = 0b111u is reserved
			 {ADS124S08::DATARATE::ChopperEnable::ENABLED,
			  ADS124S08::DATARATE::ClockSelect::EXTERNAL,
			  ADS124S08::DATARATE::ModeSelect::SINGLE_SHOT,
			  ADS124S08::DATARATE::FilterSelect::LOW_LATENCY,
			  ADS124S08::DATARATE::DataRate::RATE_4000}},
			{0xAAu,
			 {ADS124S08::DATARATE::ChopperEnable::ENABLED,
			  ADS124S08::DATARATE::ClockSelect::INTERNAL,
			  ADS124S08::DATARATE::ModeSelect::SINGLE_SHOT,
			  ADS124S08::DATARATE::FilterSelect::SINC3,
			  ADS124S08::DATARATE::DataRate::RATE_800}},
			{0x55u,
			 {ADS124S08::DATARATE::ChopperEnable::DISABLED,
			  ADS124S08::DATARATE::ClockSelect::EXTERNAL,
			  ADS124S08::DATARATE::ModeSelect::CONTINUOUS,
			  ADS124S08::DATARATE::FilterSelect::LOW_LATENCY,
			  ADS124S08::DATARATE::DataRate::RATE_50}},
		};

	for (const auto [inputReg, expected] : testCases) {

		DATARATE datarate(inputReg);
		EXPECT_EQ(datarate.getGlobalChop(), std::get<0>(expected));
		EXPECT_EQ(datarate.getClockSelect(), std::get<1>(expected));
		EXPECT_EQ(datarate.getConversionMode(), std::get<2>(expected));
		EXPECT_EQ(datarate.getFilter(), std::get<3>(expected));
		EXPECT_EQ(datarate.getDataRate(), std::get<4>(expected));
	}
}

TEST(DATARATE_Test, toRegister_ReturnsCorrectRegisterValue) {
	const std::pair<
		std::tuple<
			ADS124S08::DATARATE::ChopperEnable,
			ADS124S08::DATARATE::ClockSelect,
			ADS124S08::DATARATE::ModeSelect,
			ADS124S08::DATARATE::FilterSelect,
			ADS124S08::DATARATE::DataRate>,
		Register>
		testCases[] = {
			{{ADS124S08::DATARATE::ChopperEnable::DISABLED,
			  ADS124S08::DATARATE::ClockSelect::INTERNAL,
			  ADS124S08::DATARATE::ModeSelect::CONTINUOUS,
			  ADS124S08::DATARATE::FilterSelect::SINC3,
			  ADS124S08::DATARATE::DataRate::RATE_2_5},
			 0x00u},
			{{ADS124S08::DATARATE::ChopperEnable::ENABLED,
			  ADS124S08::DATARATE::ClockSelect::EXTERNAL,
			  ADS124S08::DATARATE::ModeSelect::SINGLE_SHOT,
			  ADS124S08::DATARATE::FilterSelect::LOW_LATENCY,
			  ADS124S08::DATARATE::DataRate::RATE_4000},
			 0xFDu}, // DR = 0b111u is reserved
			{{ADS124S08::DATARATE::ChopperEnable::ENABLED,
			  ADS124S08::DATARATE::ClockSelect::INTERNAL,
			  ADS124S08::DATARATE::ModeSelect::SINGLE_SHOT,
			  ADS124S08::DATARATE::FilterSelect::SINC3,
			  ADS124S08::DATARATE::DataRate::RATE_800},
			 0xAAu},
			{{ADS124S08::DATARATE::ChopperEnable::DISABLED,
			  ADS124S08::DATARATE::ClockSelect::EXTERNAL,
			  ADS124S08::DATARATE::ModeSelect::CONTINUOUS,
			  ADS124S08::DATARATE::FilterSelect::LOW_LATENCY,
			  ADS124S08::DATARATE::DataRate::RATE_50},
			 0x55u},
		};

	for (const auto [inputFields, expected] : testCases) {
		DATARATE datarate;
		datarate.setGlobalChop(std::get<0>(inputFields));
		datarate.setClockSelect(std::get<1>(inputFields));
		datarate.setConversionMode(std::get<2>(inputFields));
		datarate.setFilter(std::get<3>(inputFields));
		datarate.setDataRate(std::get<4>(inputFields));

		EXPECT_EQ(datarate.toRegister(), expected);
	}
}

TEST(DATARATE_Test, getAddress_ReturnsCorrectAddress) {
	EXPECT_EQ(DATARATE().getAddress(), 0x04u);
}

TEST(DATARATE_Test, getResetValue_ReturnsCorrectResetValue) {
	EXPECT_EQ(DATARATE().getResetValue(), 0x14u);
}
