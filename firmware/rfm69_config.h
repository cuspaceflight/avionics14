/**
 * RFM69 configuration parameters (application specific). This configuration is intended
 * for packet mode for WRSC2014 real time boat locations.
 *
 * First release 4 Sep 2014.
 * Joe Desbonnet, jdesbonnet@gmail.com
 *
 */

#include <stdint.h>
#include <rfm69_rdl.h>

/**
 * Using a 2 byte sync word: 0x2D, NETWORK_ID. Can run multiple networks by changing NETWORK_ID.
 */
#define NETWORK_ID (100)

/*
 * RFM69 register configuration for packet operation on 433MHz
 */
extern const uint8_t RFM69_CONFIG[][2] = {
	{ RFM69_OPMODE,
			RFM69_OPMODE_Mode_VALUE(RFM69_OPMODE_Mode_RX)
	},

	// Data operations mode and modulation settings §6.2 Table 24, page 63
	// DATAMODUL[6:5] DataMode 0x0=Packet; 0x2=Continuous with bit syncer; 0x3=Continuous, no bit syncer
	// DATAMODUL[4:3] ModulateType 0x0=FSK, 0x1=OOK
	// DATAMODUL[1:0] ModulationShaping 0x0=no shaping
	{ RFM69_DATAMODUL,
			RFM69_DATAMODUL_DataMode_Packet
			| RFM69_DATAMODUL_ModulationType_FSK
			| RFM69_DATAMODUL_ModulationShaping_None
	},

	// Bit rate settings (x2 registers BITRATEMSB, BITRATELSB)
	// bitrate = FXO / (BITRATEMSB*256 + BITRATELSB), where FXO=32MHz
	// Ref Table 9, page 20.
	// For (default) 4.8kbps (MSB=0x1a, LSB=0x0b, 16 bit value 6667)
	// For 1200bps want 0x682B
	{ RFM69_BITRATEMSB, 0x68},
	{ RFM69_BITRATELSB, 0x2B},

	// Carrier frequency (x3 registers for MSB 0x7, MID 0x8, LSB 0x9)
	// 433MHz operation
	{ RFM69_FRFMSB, 0x6C},
	{ RFM69_FRFMID, 0x40},
	{ RFM69_FRFLSB, 0} ,

	// Power Amplifier, 10dBm to comply with regulations
	{ RFM69_PALEVEL,
    	RFM69_PALEVEL_Pa0On
    	| RFM69_PALEVEL_OutputPower_VALUE(31)
	},


	// REG_RXBW Channel Filter BW Control
	// RXBW[7:5] DccFreq Cut-off freq for DC offset canceller (DCC)
	// RXBW[4:3] RxBwMant Channel filter bandwidth control (mantissa)
	// BXBW[2:0] RxBwExp Channel filter bandwidth control (exponent)
	// FSK mode RxBw = FXO / (RxBwMant * 2^(RxBwExp+2))
	// OOK mode RxBw = FXO / (RxBwMant * 2^(RxBwExp+3))
	{RFM69_RXBW,
			RFM69_RXBW_DccFreq_VALUE(2)
			| RFM69_RXBW_RxBwMant_VALUE(16)
			| RFM69_RXBW_RxBwExp_VALUE(2)
	},

	{ RFM69_SYNCCONFIG,
    		RFM69_SYNCCONFIG_SyncOn
    		| RFM69_SYNCCONFIG_SyncSize_VALUE(1) // want sync of 2 size in bytes =  SyncSize+1
    		| RFM69_SYNCCONFIG_SyncTol_VALUE(0)
	},

	// Sync word. Up to 8 bytes allowed. Using just 2. This also acts as a unique network ID.
	{ RFM69_SYNCVALUE1, 0x2D},
	{ RFM69_SYNCVALUE2, NETWORK_ID},

	{ RFM69_PACKETCONFIG1,
		RFM69_PACKETCONFIG1_PacketFormat_VALUE(RFM69_PACKETCONFIG1_PacketFormat_Variable)
    	| RFM69_PACKETCONFIG1_DcFree_VALUE(0)
    	| RFM69_PACKETCONFIG1_CrcOn_VALUE(1)
	},

    { RFM69_PAYLOADLENGTH, 66 },

    { RFM69_FIFOTHRESH, 0x8F},

    { RFM69_PACKETCONFIG2 ,
    		RFM69_PACKETCONFIG2_InterPacketRxDelay_VALUE(1) // 2^1 bit periods
    		| RFM69_PACKETCONFIG2_AutoRxRestartOn_VALUE(1)
    },


    { RFM69_TESTDAGC,
    		RFM69_TESTDAGC_ContinuousDagc_VALUE(RFM69_TESTDAGC_ContinuousDagc_LowBetaOff)},

    // Configuration Terminator
    {255, 0}
  };
