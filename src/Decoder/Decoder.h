#pragma once

#include <memory>
#include <string.h>

#include "Decoder/Nlohmann/json.hpp"

namespace Bittorent {

class Decoder
{
	using json = nlohmann::json;

public:
	Decoder();
	~Decoder();

	json DecodeBencodedValue(const std::string_view encodedValue);

private:
	class Impl;
	std::unique_ptr<Impl> m_impl;
};

}