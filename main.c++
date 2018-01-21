// compile with "g++ main.c++ lwiringPi"

#include <iostream>
#include <wiringPiI2C.h>
#include <unistd.h>
#include <string>

static const int DEVICE_ADDRESS = 0x50;

class eeprom24LC{
	public:
		eeprom24LC (int given_base_address):
			is_initialised{false,false, false, false,false,false,false, false}, 
			base_address(given_base_address)
		{
		}
		void write (unsigned int eeaddress, unsigned char data ); 
		unsigned char read (unsigned int eeaddress ); 
	private:
		void initialise_if_needed (unsigned int const page);
		bool is_initialised[8];
		int file_descriptors[8];
		int base_address;
};

void eeprom24LC::initialise_if_needed (unsigned int const page){
	if (!is_initialised[page]){
	 	std::cout << "Initialising page " << page << std::endl;
		file_descriptors[page] = wiringPiI2CSetup(DEVICE_ADDRESS);
		usleep (4000);
		is_initialised[page] = true;
	}
}

void eeprom24LC::write(unsigned int const eeaddress, unsigned char const data) 
{
	unsigned int page = (eeaddress>>8) & 0x0F;
	unsigned int offset = eeaddress & 0xFF;

	this->initialise_if_needed (page);

//	std::cout << "Writing " << data << " to address " << offset << std::endl;

	if ( wiringPiI2CWriteReg8(file_descriptors[page], offset, data) != 0){
		throw new std::exception;
	}
	usleep (4000);
}
 
unsigned char eeprom24LC::read(unsigned int const eeaddress)
{
  	unsigned char rdata = 0xFF;
 	unsigned int page = (eeaddress>>8) & 0x0F;
	unsigned int offset = eeaddress & 0xFF;

	this->initialise_if_needed (page);
	
	rdata = wiringPiI2CReadReg8 (file_descriptors[page], offset);
 
	return rdata;
}


int main()
{	
	eeprom24LC my_eeprom (DEVICE_ADDRESS);

	for (int i=0;i<0x200;i++)
		my_eeprom.write(i, 0);
	
	std::string text ("Lydia ist lieb");
  	
	for(std::string::size_type address = 0; address < text.size(); ++address) {
		my_eeprom.write(address, text[address]);
	}
}


