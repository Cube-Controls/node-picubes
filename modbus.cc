#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <termios.h>

int uart0_filestream = -1;
int Success = 0;



//****************************************************************************
//*   Get current time in ms                                                 *
//****************************************************************************

long get_current_ms(void)
{
	long            ms; // Milliseconds
	struct timespec spec;

	clock_gettime(CLOCK_REALTIME, &spec);

	ms = round(spec.tv_nsec / 1.0e6) + (spec.tv_sec * 1000); // Convert nanoseconds to milliseconds
	
	return (ms);
}


//****************************************************************************
//*   genereate CRC function                                                 *
//****************************************************************************

unsigned int generateCRC(unsigned char *buffer, unsigned char messageLength)
{
	unsigned int crc = 0xFFFF;
	unsigned int crcHigh = 0;
	unsigned int crcLow = 0;
	int i, j = 0;

	for (i = 0;i < messageLength - 2;i++)
	{
		crc ^= buffer[i];
		for (j = 8; j != 0; j--)
		{
			if ((crc & 0x0001) != 0)
			{
				crc >>= 1;
				crc ^= 0xA001;
			}
			else
			{
				crc >>= 1;
			}
		}
	}
	//bytes are wrong way round so doing a swap here..
	crcHigh = (crc & 0x00FF) << 8;
	crcLow = (crc & 0xFF00) >> 8;
	crcHigh |= crcLow;
	crc = crcHigh;
	return crc;
}


//*********************************************************************************
// Open serial port for Modbus
//*********************************************************************************

int openModbus(void)
{
	struct termios options;
	
	uart0_filestream = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
	if (uart0_filestream == -1)
	{
		//ERROR - CAN'T OPEN SERIAL PORT
		printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
		return (-1);
	}
	
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);
	
	return (0);
}	

//*********************************************************************************
// Write Holding Register using Command 0x06
//*********************************************************************************

int writeHoldingRegister(int SlaveAddress, int RegisterAddress, unsigned int Value, int TimeOut)
{
	unsigned char tx_buffer[16];
	unsigned char rx_buffer[16];
	unsigned char rx_char[1];
	unsigned int  rx_length;
	unsigned int  crc;
	unsigned int i;
	long start_time;
		
	// send request
	tx_buffer[0] = SlaveAddress;     // Slave Address
	tx_buffer[1] = 06;               // Function
	tx_buffer[2] = 00;				 // Starting Address Hi
	tx_buffer[3] = RegisterAddress;  // Starting Address Lo
	tx_buffer[4] = Value >> 8;       // Value Hi
	tx_buffer[5] = Value & 0xFF;     // Value Lo
		
	crc = generateCRC(tx_buffer, 8);
	
	tx_buffer[6] = crc >> 8;
	tx_buffer[7] = crc & 0xFF;
	
	if (uart0_filestream != -1)
	{
		int count = write(uart0_filestream, &tx_buffer[0], 8);		//Filestream, bytes to write, number of bytes to write
		if (count < 0)
		{
			return (-1);
		}
	}
	
	// reset rx_length
	rx_length = 0;
	// take start time
	start_time = get_current_ms();
	// loop
	while (rx_length < 8)
	{
		if (read(uart0_filestream, rx_char, 1) == 1)
		{
			rx_buffer[rx_length] = rx_char[0];
			rx_length++;
		}
		if (get_current_ms() > start_time + TimeOut) break;
	}		
	
	// check is it correct lenght	
	if (rx_length == 8)
	{
		// respons has to be same as request
		for (i = 0; i < rx_length; i++)
		{
			if (rx_buffer[i] != tx_buffer[i]) return (-1);
		}	
	}
	else return (-1);
	
	return (0);	
}	


//*********************************************************************************
// Read Holding Register using Command 0x03
//*********************************************************************************

int readHoldingRegister(int SlaveAddress, int RegisterAddress, unsigned int *Value, int TimeOut)
{
	unsigned char tx_buffer[16];
	unsigned char rx_buffer[16];
	unsigned int  rx_length;
	unsigned char rx_char[1];
	long start_time;
	unsigned int  crc;
	
	// send request
	tx_buffer[0] = SlaveAddress;     // Slave Address
	tx_buffer[1] = 03;               // Function
	tx_buffer[2] = 00;				 // Starting Address Hi
	tx_buffer[3] = RegisterAddress;  // Starting Address Lo
	tx_buffer[4] = 00;               // No of Registers Hi
	tx_buffer[5] = 01;               // No of Registers Lo
		
	crc = generateCRC(tx_buffer, 8);
	
	tx_buffer[6] = crc >> 8;
	tx_buffer[7] = crc & 0xFF;
	
	if (uart0_filestream != -1)
	{
		int count = write(uart0_filestream, &tx_buffer[0], 8);		//Filestream, bytes to write, number of bytes to write
		if (count < 0)
		{
			return (-1);
		}
	}
	
	// reset rx_length
	rx_length = 0;
	// take start time
	start_time = get_current_ms();
	// loop
	while (rx_length < 7)
	{
		if (read(uart0_filestream, rx_char, 1) == 1)
		{
			rx_buffer[rx_length] = rx_char[0];
			rx_length++;
		}
		if (get_current_ms() > start_time + TimeOut) break;
	}	
	
	// check is it correct lenght	
	if(rx_length == 7)
	{
		// calculate CRC
		crc = generateCRC(rx_buffer, 7) ;
		// check if crc is correct
		if ((rx_buffer[6] != (crc & 0xFF)) || (rx_buffer[5] != (crc >> 8))) return(-1);
		if (rx_buffer[0] != SlaveAddress) return(-1);
		if (rx_buffer[1] != 0x03) return(-1);
		if (rx_buffer[2] != 0x02) return(-1);
		*Value = ((unsigned int)(rx_buffer[3]) << 8) + (unsigned int)rx_buffer[4] ;
	} else return(-1);
	
    return(0);	
}	




//*********************************************************************************
// Close serial port for Modbus
//*********************************************************************************

void closeModbus(void)
{
	//----- CLOSE THE UART -----
	close(uart0_filestream);
}