#include "gtest/gtest.h"

#include "ADS124S08.hpp"

using ID	   = ADS124S08::ID;
using Register = ADS124S08::Register;

TEST(ADS124S08_ID_Test, constructor_InitializesFieldsCorrectly_FromRegister) {
	const std::pair<Register, ID::DeviceID> testCases[] = {
		{0b000u, ID::DeviceID::ADS124S08},
		{0b001u, ID::DeviceID::ADS124S06},
	};

	for (const auto &testCase : testCases) {
		const ADS124S08::ID &id = ADS124S08::ID(testCase.first);
		EXPECT_EQ(testCase.second, id.getDeviceID());
	}
}
