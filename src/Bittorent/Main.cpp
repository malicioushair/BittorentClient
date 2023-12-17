#include <iostream>

#include "../Decoder/Decoder.h"
#include "CommandLineParser/CommandLineParser.h"

using namespace Bittorent;

int main(int argc, char * argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
		return 1;
	}

	CommandLineParser clParser;
	const auto clArguments = std::vector<std::string_view>(argv, argv + argc);
	try
	{
		clParser.ParseCommandLineArguments(clArguments);
	}
	catch (const std::exception & e)
	{
		std::cerr << "Exception caught during parsing command line arguments. Description: " << e.what();
		throw; // crash!
	}

	return 0;
}
