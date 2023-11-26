#include "Decoder.h"
#include <string>

using namespace Bittorent;
using json = nlohmann::json;

namespace {
enum class EncodedValueType
{
	String,
	Number,
	List,
	Map,
	Invalid,
};

auto IsNumber(const std::string & encodedValue)
{
	return encodedValue.starts_with('i') && encodedValue.ends_with('e');
}

auto IsList(const std::string & encodedValue)
{
	return encodedValue.starts_with('l') && encodedValue.ends_with('e');
}

auto IsString(const std::string & encodedValue)
{
	return std::isdigit(encodedValue[0]) && (encodedValue.find(':') != std::string::npos);
}

auto IsMap(const std::string & encodedValue)
{
	return encodedValue.starts_with('d') && encodedValue.ends_with('e');
}

EncodedValueType GetEncodedValueType(const std::string & encodedValue)
{
	if (IsString(encodedValue))
		return EncodedValueType::String;
	else if (IsNumber(encodedValue))
		return EncodedValueType::Number;
	else if (IsList(encodedValue))
		return EncodedValueType::List;
	else if (IsMap(encodedValue))
		return EncodedValueType::Map;
	else
		return EncodedValueType::Invalid;
}

auto ParseString(const std::string & encodedValue)
{
	if (const auto colonIndex = encodedValue.find(':'); colonIndex != std::string::npos)
	{
		const auto numberString = encodedValue.substr(0, colonIndex);
		const auto number = std::atoll(numberString.data());
		return encodedValue.substr(colonIndex + 1, number);
	}
	else
	{
		throw std::runtime_error("Invalid encoded value: " + encodedValue);
	}
}

auto StringToJson(const std::string & str)
{
	return json(ParseString(str));
}

auto ParseNumber(const std::string & encodedValue)
{
	const auto numberFront = encodedValue.find('i') + 1;
	const auto numberEnd = encodedValue.find('e');
	const auto numberLength = numberEnd - numberFront;
	return encodedValue.substr(numberFront, numberLength);
}

auto NumberStrToJson(const std::string & num)
{
	try
	{
		return json(std::stoll(ParseNumber(num)));
	}
	catch (const std::invalid_argument & ex)
	{
		throw std::runtime_error("Failed to convert string to int. Invalid argument! Input value: " + num + ". Message: " + ex.what());
	}
	catch (const std::out_of_range & ex)
	{
		throw std::runtime_error("Failed to convert string to int. Out of range! Input value: " + num + ". Message: " + ex.what());
	}
}

auto ParseStringInList(auto & frontIt, const std::string & str)
{
	const auto parsedString = ParseString(str);

	const auto numberOfSpecialCharacters = str.find(':') + 1;
	frontIt += parsedString.size() + numberOfSpecialCharacters;

	return parsedString;
}

auto ParseNumberInList(auto & frontIt, const std::string & str)
{
	const auto parsedNumber = ParseNumber(str);

	static constexpr auto NUMBER_OF_SPECIAL_CHARACTERS = 2;
	frontIt += parsedNumber.size() + NUMBER_OF_SPECIAL_CHARACTERS;

	return parsedNumber;
}

auto ParseList(const std::string & encodedValue)
{
	auto frontIt = std::next(encodedValue.cbegin());
	const auto endIt = std::prev(encodedValue.cend());

	const auto encodedValueParsed = [&] { return frontIt == endIt; };

	std::string result = "[";
	while (!encodedValueParsed())
	{
		const auto str = std::string(frontIt, endIt);
		const auto encodedValueType = GetEncodedValueType(str);
		switch (encodedValueType)
		{
			case EncodedValueType::String:
			{
				result.append(ParseStringInList(frontIt, str));
				if (!encodedValueParsed())
					result.append(", ");
				continue;
			}
			case EncodedValueType::Number:
			{
				result.append(ParseNumberInList(frontIt, str));
				if (!encodedValueParsed())
					result.append(", ");
				continue;
			}

			default:
				throw std::runtime_error("Invalid value type");
		}
	}

	result.append("]");
	return result;
}

auto ListStrToJson(const std::string & list)
{
	return json(ParseList(list));
}

auto ParseMap(const std::string & encodedMap)
{
	auto frontIt = std::next(encodedMap.cbegin());
	const auto endIt = std::prev(encodedMap.cend());

	const auto encodedMapParsed = [&] { return frontIt == endIt; };

	std::string result = "{";
	while (!encodedMapParsed())
	{
		const auto getRemainingString = [&] { return std::string(frontIt, endIt); };

		const auto key = ParseStringInList(frontIt, getRemainingString());
		const auto value = [&] {
			const auto encodedValueType = GetEncodedValueType(getRemainingString());
			switch (encodedValueType)
			{
				case EncodedValueType::String:
					return ParseStringInList(frontIt, getRemainingString());
				case EncodedValueType::Number:
					return ParseNumberInList(frontIt, getRemainingString());
				default:
					throw std::runtime_error("Invalid value type");
			}
		}();

		result.append(std::format("{}: {}", key, value));
		if (!encodedMapParsed())
			result.append(", ");
	}

	result.append("}");
	return result;
}

auto MapStrToJson(const std::string & map)
{
	return json(ParseMap(map));
}

}

json Decoder::DecodeBencodedValue(const std::string & encodedValue)
{
	EncodedValueType type = GetEncodedValueType(encodedValue);

	switch (type)
	{
		case EncodedValueType::String:
			return StringToJson(encodedValue);

		case EncodedValueType::Number:
			return NumberStrToJson(encodedValue);

		case EncodedValueType::List:
			return ListStrToJson(encodedValue);

		case EncodedValueType::Map:
			return MapStrToJson(encodedValue);

		default:
			throw std::runtime_error("Unhandled encoded value: " + encodedValue);
	}
}
