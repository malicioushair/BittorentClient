#include <cctype>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

#include "lib/nlohmann/json.hpp"

using json = nlohmann::json;

bool is_number(const std::string & value)
{
	return !value.empty() && std::find_if(value.begin(), value.end(), [](unsigned char c) { return !std::isdigit(c); }) == value.end();
}

json decode_bencoded_value(const std::string & encoded_value)
{
	if (std::isdigit(encoded_value[0]))
	{
		// Example: "5:hello" -> "hello"
		size_t colon_index = encoded_value.find(':');
		if (colon_index != std::string::npos)
		{
			std::string number_string = encoded_value.substr(0, colon_index);
			int64_t number = std::atoll(number_string.c_str());
			std::string str = encoded_value.substr(colon_index + 1, number);
			return json(str);
		}
		else
		{
			throw std::runtime_error("Invalid encoded value: " + encoded_value);
		}
	}
	if (encoded_value.starts_with('i') && encoded_value.ends_with('e'))
	{
		const auto resultStr = std::string(std::next(encoded_value.cbegin()), std::prev(encoded_value.cend()));
		try
		{
			return json(std::stoll(resultStr));
		}
		catch (const std::invalid_argument & ex)
		{
			throw std::runtime_error("Failed to convert string to int. Invalid argument! Input value: " + resultStr + ". Message: " + ex.what());
		}
		catch (const std::out_of_range & ex)
		{
			throw std::runtime_error("Failed to convert string to int. Out of range! Input value: " + resultStr + ". Message: " + ex.what());
		}
	}
	else
		throw std::runtime_error("Unhandled encoded value: " + encoded_value);
}

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

		std::string encoded_value = argv[2];
		json decoded_value = decode_bencoded_value(encoded_value);
		std::cout << decoded_value.dump() << std::endl;
	}
	else
	{
		std::cerr << "unknown command: " << command << std::endl;
		return 1;
	}

	return 0;
}
