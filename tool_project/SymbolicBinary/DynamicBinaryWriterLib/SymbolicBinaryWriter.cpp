#include "SymbolicBinaryWriter.h"

SymbolicBinaryWriter::SymbolicBinaryWriter(const char* filename)
	: tableWriter((std::string(filename) + ".symbol").c_str())
{
	binaryFp.open(filename, std::ios::out | std::ios::binary);

	if (binaryFp.is_open() == false) {
		close();
		// TODO:
		// äJÇØÇ»Ç©Ç¡ÇΩÇ±Ç∆ÇèoóÕ
		std::cerr << "--------------------" << std::endl;
		std::cerr << "Can not Create " << filename << "(.symbol)" << std::endl;
		std::cerr << "--------------------" << std::endl;

		throw SymbolicBinaryWriterException();
	}
}
SymbolicBinaryWriter::~SymbolicBinaryWriter()
{
	close();
}

void SymbolicBinaryWriter::close()
{
	this->binaryFp.close();
	this->tableWriter.close();
}

void SymbolicBinaryWriter::write(const char* data, const unsigned int data_size)
{	
	this->binaryFp.write(data, data_size);
}

void SymbolicBinaryWriter::writeWithSymbol(const unsigned int data_size, const char* symbol, const char* comment)
{
	const unsigned int startAddr = static_cast<unsigned int>(this->binaryFp.tellp());

	this->binaryFp << std::setw(data_size) << std::setfill('\0') << '\0';

	this->tableWriter.recordRefSymbol(startAddr, data_size, symbol, comment);
}

void SymbolicBinaryWriter::recordDefSymbol(const char* symbol, const unsigned char* value, const unsigned int value_size, const char* type)
{
	this->tableWriter.recordDefSymbol(symbol, value, value_size, type);
}

void SymbolicBinaryWriter::setPos(const unsigned int address)
{
	this->binaryFp.seekp(address);
}

unsigned int SymbolicBinaryWriter::getPos()
{
	return static_cast<unsigned int>(this->binaryFp.tellp());
}
