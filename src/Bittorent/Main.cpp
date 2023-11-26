#include <cctype>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

#include "../Decoder/Decoder.h"

using namespace Bittorent;

int main(int argc, char * argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
		return 1;
	}

	std::string command = argv[1];

	if (command == "decode")
	{
		if (argc < 3)
		{
			std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
			return 1;
		}

		std::string encodedValue = argv[2];
		Decoder decoder;
		auto decodedValue = decoder.DecodeBencodedValue(encodedValue);
		std::cout << decodedValue.dump() << std::endl;
	}
	else
	{
		std::cerr << "unknown command: " << command << std::endl;
		return 1;
	}

	return 0;
}
