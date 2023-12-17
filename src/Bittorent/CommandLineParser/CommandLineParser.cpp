#include "CommandLineParser.h"

#include <iostream>
#include <stdexcept>
#include <string>

#include "../../Decoder/Decoder.h"

namespace Bittorent {

void CommandLineParser::ParseCommandLineArguments(const std::vector<std::string_view> & clArgs) const
{
	const auto command = clArgs[1];
	if (command == "decode")
	{
		if (clArgs.size() < 3)
		{
			std::cerr << "Usage: " << clArgs[0] << " decode <encoded_value>" << std::endl;
			throw std::runtime_error("Incorrect usage: too few arguments");
		}

		Decoder decoder;
		const auto encodedValue = clArgs[2];
		const auto decodedValue = decoder.DecodeBencodedValue(encodedValue);
		std::cout << decodedValue.dump() << std::endl;
	}
	else if (command == "infog")
	{
	}
	else
	{
		std::cerr << "unknown command: " << command << std::endl;
		throw std::logic_error("Incorrect usage: unknown command");
	}
}

}