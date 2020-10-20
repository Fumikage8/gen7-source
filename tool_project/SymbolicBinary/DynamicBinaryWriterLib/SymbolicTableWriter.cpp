#include "SymbolicTableWriter.h"

SymbolicTableWriter::SymbolicTableWriter(char const * file_path)
{
	symbolFp.open(file_path, std::ios::out);

	if (symbolFp.is_open() == false) {
		close();
		// 開けなかったことを出力
		std::cerr << "--------------------" << std::endl;
		std::cerr << "Can not Create " << file_path << std::endl;
		std::cerr << "--------------------" << std::endl;

		throw std::exception();
	}
}

SymbolicTableWriter::~SymbolicTableWriter()
{
	this->close();
}

void SymbolicTableWriter::close()
{
	symbolFp.close();
}

void SymbolicTableWriter::recordRefSymbol(const unsigned int startAddr, const unsigned int data_size, const char* symbol, const char* comment)
{
	// 参照シンボル記録
	this->symbolFp << "R,";
	this->symbolFp << std::hex << std::setw(8) << std::setfill('0') << startAddr;
	this->symbolFp << ",";
	this->symbolFp << std::hex << std::setw(4) << std::setfill('0') << data_size;
	this->symbolFp << "," << symbol;
	if (comment != NULL) {
		this->symbolFp << "," << comment;
	}
	this->symbolFp << std::endl;
}

void SymbolicTableWriter::recordDefSymbol(const char* symbol, const unsigned char* value, const unsigned int value_size, const char* type)
{
	char* str = new char[value_size * 4 + 1];

	this->Crypt2OctetStr(value, value_size, str);

	this->symbolFp << "D," << symbol << ","  << str << "," << type << std::endl;

	delete str;
}

void SymbolicTableWriter::Crypt2OctetStr(const unsigned char* bytes, const unsigned int byte_size, char* outStr)
{
	char buf[5];

	outStr[0] = '\0';
	for (unsigned int i = 0; i < byte_size; ++i) {
		#pragma warning(disable:4996)
		sprintf(buf, "\\%03u", bytes[i]);
		strcat(outStr, buf);		
		#pragma warning(default:4996)

		/*
		sprintf_s(buf, 5, "\\%03u", bytes[i]);
		strcat_s(outStr, outsize, buf);
		*/
	}
}
