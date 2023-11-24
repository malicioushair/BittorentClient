#pragma once

#include <string.h>

#include "lib/nlohmann/json.hpp"

namespace Bittorent {

class Decoder
{
	using json = nlohmann::json;

public:
	json DecodeBencodedValue(const std::string & encodedValue);
};

}