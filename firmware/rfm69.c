/**
 * RFM69HW (HopeRF) radio module driver for WRSC2014.
 *
 * First (pre)release 4 Sep 2014. Please check back in a few days for an update.
 *
 * Joe Desbonnet, jdesbonnet@gmail.com
 */

#include <stdint.h>
#include "rfm69.h"
#include "err.h" //currently returns errors as defined in this header file

#define RFM69_SPID SPID2 
#define RFM69W_SPI_CS_PORT A //change as needed
#define RFM69W_SPI_CS_PIN 0

extern const uint8_t RFM69_CONFIG[][2];

uint8_t telemBfr[66]; //data written into this buffer

/**
 * Wait for a register bit to go high, with timeout.
 */
int rfm69_wait_for_bit_high (*SPID SPID, uint8_t reg_addr, uint8_t mask) {
	int niter=50000;
	while ( (rfm69_register_read(SPID, reg_addr) & mask) == 0) {
		if (--niter == 0) {
			return E_TIMEOUT;
		}
	}
	return E_OK;
}

/** 
 * Test for presence of RFM69
 */
int rfm69_test (*SPID SPID) {
	// Backup AES key register 1
	int aeskey1 = rfm69_register_read(SPID, 0x3E);

	rfm69_register_write(SPID, 0x3E,0x55);
	if (rfm69_register_read(SPID, 0x3E)!=0x55) {
		return -1;
	}
	rfm69_register_write(SPID, 0x3E,0xAA);
	if (rfm69_register_read(SPID, 0x3E)!=0xAA) {
		return -1;
	}
	// Restore value
	rfm69_register_write(SPID, 0x3E, aeskey1);

	return 0;
}
/**
 * Configure RFM69 radio module for use. Assumes SPI interface is already configured.
 */
void rfm69_config(*SPID SPID) {
	int i;
	for (i = 0; RFM69_CONFIG[i][0] != 255; i++) {
	    rfm69_register_write(SPID, RFM69_CONFIG[i][0], RFM69_CONFIG[i][1]);
	}
}

/**
 * Set RFM69 operating mode. Use macro values RFM69_OPMODE_Mode_xxxx as arg.
 */
int rfm69_mode (*SPID SPID, uint8_t mode) {
	uint8_t regVal = rfm69_register_read(SPID, RFM69_OPMODE);
	regVal &= ~RFM69_OPMODE_Mode_MASK;
	regVal |= RFM69_OPMODE_Mode_VALUE(mode);
	rfm69_register_write(SPID, RFM69_OPMODE,regVal);

	// Wait until mode change is complete
	// IRQFLAGS1[7] ModeReady: Set to 0 when mode change, 1 when mode change complete
	return rfm69_wait_for_bit_high(SPID, RFM69_IRQFLAGS1, RFM69_IRQFLAGS1_ModeReady);
}

/**
 * Get RSSI
 */
uint8_t rfm69_rssi (*SPID SPID) {

	rfm69_register_write(SPID, RFM69_RSSICONFIG, RFM69_RSSICONFIG_RssiStart);

	// Wait for RSSI ready
	//while ((rfm69_register_read(SPID, RFM69_RSSICONFIG) & RFM69_RSSICONFIG_RssiDone_MASK) == 0x00);
	rfm69_wait_for_bit_high(SPID, RFM69_RSSICONFIG, RFM69_RSSICONFIG_RssiDone);
	return rfm69_register_read(SPID, RFM69_RSSIVALUE);
}

/**
 * Check if packet has been received and is ready to read from FIFO.
 * @return zero if no packet available, non-zero if packet available.
 */
uint8_t rfm69_payload_ready(*SPID SPID) {
	return rfm69_register_read(SPID, RFM69_IRQFLAGS2) & RFM69_IRQFLAGS2_PayloadReady_MASK;
}

/**
 * Retrieve a frame. If successful returns length of frame. If not an error code (negative value).
 * Frame is returned in buf but will not exceed length maxlen. Should only be called when
 * a frame is ready to download.
 *
 * @return frame_length if successful, else a negative value error code
 * Error codes:
 * -2 : frame too long
 */
int rfm69_frame_rx(*SPID SPID, uint8_t *buf, int maxlen, uint8_t *rssi) {

	int i;

    uint8_t frame_length;

    spiSelect(SPID);
    rfm69_spi_transfer_byte(SPID, RFM69_FIFO);

	// Read frame length;
    frame_length = rfm69_spi_transfer_byte(SPID, 0);

    // Probably SPI bus problem
	if (frame_length == 0xff) {
		return E_SPI;
	}

    if (frame_length > 66) {
    	// error condition really
    	frame_length = 66;
    }

    for (i = 0; i < frame_length; i++) {
    	if (i == maxlen) {
    		return E_PKT_TOO_LONG;
    	}
    	buf[i] = rfm69_spi_transfer_byte(SPID, 0);
    }
    spiUnselect(SPID);

    // If pointer to rssi given, fetch it
    if (rssi != 0) {
    	*rssi = rfm69_rssi(SPID);
    }

    return frame_length;
}

/**
 * Transmit a frame stored in buf
 */
void rfm69_frame_tx(*SPID SPID, uint8_t *buf, int len) {

	// Turn off receiver before writing to FIFO
	rfm69_mode(SPID, RFM69_OPMODE_Mode_STDBY);

	// Write frame to FIFO
	spiSelect(SPID);

	rfm69_spi_transfer_byte(SPID, RFM69_FIFO | 0x80);

	// packet length
	rfm69_spi_transfer_byte(SPID, len);

	int i;
	for (i = 0; i < len; i++) {
		rfm69_spi_transfer_byte(SPID, buf[i]);
	}

	spiUnselect(SPID);

	// Power up TX
	rfm69_mode(SPID, RFM69_OPMODE_Mode_TX);

	rfm69_wait_for_bit_high(SPID, RFM69_IRQFLAGS2, RFM69_IRQFLAGS2_PacketSent);
}

uint8_t rfm69_register_read (*SPID SPID, uint8_t reg_addr) {
	spiSelect(SPID);
	rfm69_spi_transfer_byte(SPID, reg_addr);
	uint8_t reg_value = rfm69_spi_transfer_byte(SPID, 0xff);
	spiUnselect(SPID);
	return reg_value;
}

void rfm69_register_write (*SPID SPID, uint8_t reg_addr, uint8_t reg_value) {
	spiSelect(SPID);
	rfm69_spi_transfer_byte (SPID, reg_addr | 0x80); // Set bit 7 to indicate write op
	rfm69_spi_transfer_byte (SPID, reg_value);
	spiUnselect(SPID);
}

uint8_t rfm69_spi_transfer_byte(*SPID SPID, uint8_t out) {
	//the spi_transfer_byte function in this library is an SPI exchange: out is sent and MISO is sampled synchronously, so think can be replaced with the CHiBiOS spiExchange method
	
	uint8_t val; //receive buffer	
	spiExchange(SPID, 1, (void*) &out, (void*) &val);
	
	return val;
}

msg_t rfm69_thread(void *arg) {
	(void) arg;

	const SPIConfig spi_cfg = { //setup SPI
		NULL,
		RFM69_SPI_CS_PORT,
		RFM69_SPI_CS_PIN,
		SPI_CR1_BR_1 | SPI_CR1_BR_0 | SPI_CR1_CPOL | SPI_CR1_CPHA
	};

	chRegSetThreadName("RFM69");

	spiStart(&RFM69_SPID, &spi_cfg);
	rfm69_config(SPID);

	while(TRUE) {
		//update the buffer with telemetry data
		
		//send telemetry data
		rfm69_frame_tx(SPID, telemBfr, 66);
		rfm69_mode(SPID, RFM69_OPMODE_Mode_STDBY); //turn receiver to standby mode
		chThdSleepMilliseconds(100);
		
		//assume that the FIFO can be overwritten instead of having to be cleared
	}

	return (msg_t)NULL;
}
	


