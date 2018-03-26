/*!
@file

 @brief      handels the SERIAL_PRINT parameters

 -----------------------------------------------------------------------
 @author  2018 Alexander Ulbrich
 alexander.ulbrich@uni-ulm.de
 -----------------------------------------------------------------------
 @History

 1.0  30.01.2018 First working version

@end
@ingroup gb4
@addtogroup gb4
*/
#ifndef GOLDBOARD4_PRINT_H_
#define GOLDBOARD4_PRINT_H_

class SerialPrint
{
private:
	bool blockOn;
public:
	SerialPrint();
	void block(); //if this function is called all serial data over print is blocked
	void print(const char* string);
	void print(int number);
	void print(float floatnumber);
	void print(unsigned int number);
	void println(const char* string);
	void println(int number);
	void println(unsigned int number);
	void println(float floatnumber);
	void println();
};

extern SerialPrint serial;

#endif /* GOLDBOARD4_PRINT_H_ */
