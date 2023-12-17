#pragma once

#include <string_view>
#include <vector>

namespace Bittorent {

class CommandLineParser
{
public:
	void ParseCommandLineArguments(const std::vector<std::string_view> & clArgs) const;
};

}