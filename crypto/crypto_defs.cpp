/**
 * Enecuum Crypto Library source code
 * See LICENCE file at the top of the source tree
 *
 * ******************************************
 *
 * crypto_defs.cpp
 * Error-handling class
 *
 * ******************************************
 *
 * Authors: I. Korobkov
 */

#include "crypto_defs.h"

char charBuff[MAX_LEN_BUF] = "\0";



const std::string errorDescription(const uint errorCode) {
	static std::vector<std::string> errorStrAll;
	if (errorStrAll.empty()) {
		errorStrAll.push_back("Not allocated memory, NULL was returned");
		errorStrAll.push_back("Pointer(-s) is NULL");
		errorStrAll.push_back("Parameter(-s) is invalid");
		errorStrAll.push_back("Not found element(-s) in container");
		errorStrAll.push_back("Element(-s) is invalid");
		errorStrAll.push_back("ID of element is invalid");
		errorStrAll.push_back("Calculation is failed");
		errorStrAll.push_back("Not defined action");
		errorStrAll.push_back("Not put new element, container is overflow");
		errorStrAll.push_back("Unknown error");
		errorStrAll.push_back(""); //NO_MESSAGE
	}

	if (errorCode < errorStrAll.size())
		return errorStrAll[errorCode];
	else {
		uint last = errorStrAll.size() - 1;
		return errorStrAll[last];
	}
}


const std::string toHex(void* p) {
	static std::stringstream ss;
	ss.str("");
	ss.clear();
	ss << "0x";
	ss << std::hex << (unsigned long)((void*)p);
	return ss.str();
}


char* getCharBuff() {
	return charBuff;
}

void handleError(const uint errorCode)
{
	static std::string mess = errorDescription(errorCode);
	std::cout << std::endl << mess << std::endl;
	std::cout << ERR_error_string(ERR_get_error(), getCharBuff()) << std::endl;
	throw ERR_get_error();
}




