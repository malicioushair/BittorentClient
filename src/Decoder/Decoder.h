#pragma once

#include <memory>
#include <string.h>

#include "Nlohmann/json.hpp"

namespace Bittorent {

class Decoder
{
	using json = nlohmann::json;

public:
	Decoder();  // конструктор
	~Decoder(); // детруктор

	json DecodeBencodedValue(const std::string & encodedValue);

private:
	class Impl;
	std::unique_ptr<Impl> m_impl;
};

}