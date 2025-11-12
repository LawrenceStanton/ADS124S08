#pragma once

struct ADS124S08::SPI_Register_I {
public:
	/**
	 * @brief Pack the writable fields of the structured register into a Register value.
	 *
	 * @return The writable Register value.
	 * @note This method does not perform any SPI transaction.
	 * @note Read-only bits are set to zero.
	 */
	virtual Register toRegister(void) const = 0;

	/**
	 * @brief Get the SPI address of this register.
	 *
	 */
	virtual Address getAddress(void) const = 0;

	/**
	 * @brief Get the reset value of this register.
	 *
	 */
	virtual Register getResetValue(void) const = 0;

protected:
	virtual ~SPI_Register_I() = default;
};
