#pragma once

#include <cstdint>
#include <optional>
#include <tuple>

class ADS124S08 {
public:
	/**
	 * @brief SPI interface for the ADS124S08.
	 *
	 * This class defines the interface for SPI communication with the ADS124S08 analog-to-digital
	 * converter. The user must provide concrete implementations for reading, writing, and
	 * simultaneous read/write operations.
	 */
	class SPI {
	public:
		typedef uint8_t	 Register;
		typedef Register Command;

		enum ControlCommand : Command {
			NOP		  = 0x00u,
			WAKEUP	  = 0x02u,
			POWERDOWN = 0x04u,
			RESET	  = 0x06u,
			START	  = 0x08u,
			STOP	  = 0x0Au,
		};

		enum CalibrationCommand : Command {
			SYS_OFFSET_CAL	= 0x16u,
			SYS_GAIN_CAL	= 0x17u,
			SELF_OFFSET_CAL = 0x19u,
		};

		enum DataReadCommand : Command {
			RDATA = 0x12u,
		};

		enum Address : uint8_t {
			ID		  = 0x00u,
			STATUS	  = 0x01u,
			INP_MUX	  = 0x02u,
			PGA		  = 0x03u,
			DATA_RATE = 0x04u,
			REF		  = 0x05u,
			IDAC_MUG  = 0x06u,
			IDAC_MUX  = 0x07u,
			VBIAS	  = 0x08u,
			SYS		  = 0x09u,
			OF_CAL0	  = 0x0Au,
			OF_CAL1	  = 0x0Bu,
			OF_CAL2	  = 0x0Cu,
			FS_CAL0	  = 0x0Du,
			FS_CAL1	  = 0x0Eu,
			FS_CAL2	  = 0x0Fu,
			GPIO_DATA = 0x10u,
			GPIO_CON  = 0x11u,
		};

		/**
		 * @brief Read multiple SPI bytes from the ADS124S08.
		 *
		 * @param buffer A reference to an array to store the register values.
		 * @param count The number of registers to read.
		 * @return The number of bytes read, or std::nullopt if any read failed.
		 */
		virtual std::optional<uint8_t> read(Register *const buffer, uint8_t count) noexcept = 0;

		/**
		 * @brief Write multiple SPI bytes to the ADS124S08.
		 *
		 * @param buffer A reference to an array containing the register values to write.
		 * @param count The number of registers to write.
		 * @return The number of bytes written, or std::nullopt if any write failed.
		 */
		virtual std::optional<uint8_t>
		write(const Register *const buffer, uint8_t count) noexcept = 0;

		/**
		 * @brief Simultaneously read and write multiple SPI bytes to/from the ADS124S08.
		 *
		 * @param txBuffer A reference to an array containing the register values to write.
		 * @param rxBuffer A reference to an array to store the register values read.
		 * @param count The number of registers to read/write.
		 * @return A tuple containing the number of bytes read and written respectively, or
		 * `std::nullopt` if any operation failed.
		 */
		virtual std::optional<std::tuple<uint8_t, uint8_t>> readWrite(
			const Register *const txBuffer,
			Register *const		  rxBuffer,
			uint8_t				  count
		) noexcept = 0;

		virtual ~SPI() = default;
	};

private:
	SPI &spi;

public:
	using Address  = SPI::Address;
	using Register = SPI::Register;

	struct SPI_Register_I;

	struct ID;
	struct STATUS;
	struct INPMUX;
	struct PGA;
	struct DATARATE;
	struct REF;
	struct IDACMAG;
	struct IDACMUX;
	struct VBIAS;
	struct SYS;
	struct OFCAL0;
	struct OFCAL1;
	struct OFCAL2;
	struct FSCAL0;
	struct FSCAL1;
	struct FSCAL2;
	struct GPIODAT;
	struct GPIOCON;

	explicit constexpr ADS124S08(SPI &spi) : spi(spi) {}

	/**
	 * @brief Send the WAKEUP command to the ADS124S08 to exit power-down mode.
	 *
	 * @return The command sent if successful, `std::nullopt` otherwise.
	 * @note Refer to the ADS124S08 §9.5.3.2 "WAKEUP" for details.
	 */
	std::optional<Register> wakeup() noexcept;

	/**
	 * @brief Send the POWERDOWN command to the ADS124S08 to enter power-down mode.
	 *
	 * @return The command sent if successful, `std::nullopt` otherwise.
	 * @note Refer to the ADS124S08 §9.5.3.3 "POWERDOWN" for details.
	 */
	std::optional<Register> powerdown() noexcept;

	/**
	 * @brief Send the RESET command to the ADS124S08 to reset the device.
	 *
	 * @return The command sent if successful, `std::nullopt` otherwise.
	 */
	std::optional<Register> reset() noexcept;

	/**
	 * @brief Send the START command to the ADS124S08 to start a conversion.
	 *
	 * @return The command sent if successful, `std::nullopt` otherwise.
	 */
	std::optional<Register> start() noexcept;

	/**
	 * @brief Send the STOP command to the ADS124S08 to stop a conversion.
	 *
	 * @return The command sent if successful, `std::nullopt` otherwise.
	 */
	std::optional<Register> stop() noexcept;

	/**
	 * @brief Perform system offset calibration.
	 *
	 * @return The command sent if successful, `std::nullopt` otherwise.
	 */
	std::optional<Register> offsetCalibrate() noexcept;

	/**
	 * @brief Perform system gain calibration.
	 *
	 * @return The command sent if successful, `std::nullopt` otherwise.
	 */
	std::optional<Register> gainCalibrate() noexcept;

	/**
	 * @brief Perform self offset calibration.
	 *
	 * @return The command sent if successful, `std::nullopt` otherwise.
	 */
	std::optional<Register> selfOffsetCalibrate() noexcept;

	std::optional<Register> setRegister(const SPI_Register_I &reg) const noexcept;

	/**
	 * @brief Perform a RREG operation to read registers from the ADS124S08.
	 *
	 * @param startAddress The starting register address to read from.
	 * @param count The number of registers to read.
	 * @param buffer A pointer to an array where the read register values will be stored. Only
	 * needed if count > 1.
	 * @return The first register value read if successful, `std::nullopt` otherwise.
	 * @warning If std::nullopt is returned, the ADC must either be reset, CS brought high, or
	 * the ADC SPI timeout must elapse before the next command will be accepted.
	 * @note Refer to the ADS124S08 §9.5.3.11 "RREG" for details.
	 */
	std::optional<Register> rreg(
		Address			startAddress, //
		uint8_t			count  = 1,
		Register *const buffer = nullptr
	) const noexcept;

	/**
	 * @brief Perform a WREG operation to write registers to the ADS124S08.
	 *
	 * @param startAddress The starting register address to write to.
	 * @param count The number of registers to write.
	 * @param buffer A pointer to an array containing the register values to write.
	 * @return The first register value written if successful, `std::nullopt` otherwise.
	 * @warning If std::nullopt is returned, the ADC must either be reset, CS brought high, or the
	 * ADC SPI timeout must elapse before the next command will be accepted.
	 * @note Writing to certain registers may reset the digital filter and start a new conversion.
	 * @note Refer to the ADS124S08 §9.5.3.12 "WREG" for details.
	 */
	std::optional<Register> wreg(
		Address				  startAddress, //
		uint8_t				  count,
		const Register *const buffer
	) const noexcept;

	std::optional<Register> wreg(
		Address			startAddress, //
		const Register &value
	) const noexcept;
};

#include "Private/ADS124S08_SPI_Register_I.hpp"

#include "Private/ADS124S08_STATUS.hpp"

#include "Private/ADS124S08_REF.hpp"

#include "Private/ADS124S08_INPMUX.hpp"
