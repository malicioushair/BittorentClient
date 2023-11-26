#include "Decoder.h"

#include <memory>

#include <string_view>
#include <variant>

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

auto IsNumber(const std::string_view encodedValue)
{
	return encodedValue.starts_with('i') && encodedValue.ends_with('e');
}

auto IsList(const std::string_view encodedValue)
{
	return encodedValue.starts_with('l') && encodedValue.ends_with('e');
}

auto IsString(const std::string_view encodedValue)
{
	return std::isdigit(encodedValue[0]) && (encodedValue.find(':') != std::string::npos);
}

auto IsMap(const std::string_view encodedValue)
{
	return encodedValue.starts_with('d') && encodedValue.ends_with('e');
}

EncodedValueType GetEncodedValueType(const std::string & encodedValue)
{
	using enum EncodedValueType;
	if (IsString(encodedValue))
		return String;
	else if (IsNumber(encodedValue))
		return Number;
	else if (IsList(encodedValue))
		return List;
	else if (IsMap(encodedValue))
		return Map;
	else
		return Invalid;
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

auto ParseNumber(const std::string & encodedValue)
{
	const auto numberFront = encodedValue.find('i') + 1;
	const auto numberEnd = encodedValue.find('e');
	const auto numberLength = numberEnd - numberFront;

	try
	{
		return std::stoll(encodedValue.substr(numberFront, numberLength));
	}
	catch (const std::invalid_argument & ex)
	{
		throw std::runtime_error("Failed to convert string to int. Invalid argument! Input value: " + encodedValue + ". Message: " + ex.what());
	}
	catch (const std::out_of_range & ex)
	{
		throw std::runtime_error("Failed to convert string to int. Out of range! Input value: " + encodedValue + ". Message: " + ex.what());
	}
}

auto ParseStringInContainer(auto & frontIt, const std::string & str)
{
	const auto parsedString = ParseString(str);

	const auto numberOfSpecialCharacters = str.find(':') + 1;
	frontIt += parsedString.size() + numberOfSpecialCharacters;

	return parsedString;
}

auto CountDigits(int64_t n)
{
	if (n == 0)
		return 1;
	int count = 0;
	while (n != 0)
	{
		n /= 10;
		++count;
	}
	return count;
}

auto ParseNumberInContainer(auto & frontIt, const std::string & str)
{
	const auto parsedNumber = ParseNumber(str);

	static constexpr auto NUMBER_OF_SPECIAL_CHARACTERS = 2;
	frontIt += CountDigits(parsedNumber) + NUMBER_OF_SPECIAL_CHARACTERS;

	return parsedNumber;
}

}

class Decoder::Impl
{
public:
	json DecodeString(const std::string & encodedValue) const
	{
		return json(ParseString(encodedValue));
	}

	json DecodeNumber(const std::string & encodedValue) const
	{
		return json(ParseNumber(encodedValue));
	}

	json DecodeList(const std::string_view encodedValue)
	{
		auto frontIt = std::next(encodedValue.cbegin());
		const auto endIt = std::prev(encodedValue.cend());

		return DecodeListEntries(frontIt, endIt);
	}

	json DecodeListEntries(std::string_view::const_iterator & frontIt, const std::string_view::const_iterator & endIt)
	{
		auto result = json::array();
		while (frontIt != endIt)
		{
			const auto encodedValue = std::string(frontIt, endIt);
			const auto encodedValueType = GetEncodedValueType(encodedValue);
			switch (encodedValueType)
			{
				using enum EncodedValueType;
				case String:
				{
					result.emplace_back(ParseStringInContainer(frontIt, encodedValue));
					continue;
				}
				case Number:
				{
					result.emplace_back(ParseNumberInContainer(frontIt, encodedValue));
					continue;
				}
				case List:
				{
					result.emplace_back(DecodeList(encodedValue));
					return result;
				}
				case Map:
				{
					result.emplace_back(DecodeMap(encodedValue));
					return result;
				}
				default:
					throw std::runtime_error("Invalid value type");
			}
		}

		return result;
	}

	json DecodeMap(const std::string_view encodedValue)
	{
		auto frontIt = std::next(encodedValue.cbegin());
		const auto endIt = std::prev(encodedValue.cend());

		return DecodeMapEntries(frontIt, endIt);
	}

private:
	json DecodeMapEntries(std::string_view::const_iterator & frontIt, const std::string_view::const_iterator & endIt)
	{
		auto result = json::object();
		while (frontIt != endIt)
		{
			const auto key = ParseStringInContainer(frontIt, std::string(frontIt, endIt));
			const auto value = GetNextValue(frontIt, endIt);
			std::visit([&result, &key](const auto & val) { result[key] = val; }, value);
		}
		return result;
	}

	std::variant<std::string, int64_t, json> GetNextValue(std::string_view::const_iterator & frontIt, const std::string_view::const_iterator & endIt)
	{
		const auto remainingStr = std::string(frontIt, endIt);
		const auto valueType = GetEncodedValueType(remainingStr);

		switch (valueType)
		{
			using enum EncodedValueType;
			case String:
				return ParseStringInContainer(frontIt, remainingStr);
			case Number:
				return ParseNumberInContainer(frontIt, remainingStr);
			case List:
				frontIt = endIt;
				return DecodeList(remainingStr);
			case Map:
				frontIt = endIt;
				return DecodeMap(remainingStr);
			default:
				throw std::runtime_error("Invalid value type in map.");
		}
	}
};

json Decoder::DecodeBencodedValue(const std::string & encodedValue)
{
	EncodedValueType type = GetEncodedValueType(encodedValue);

	switch (type)
	{
		using enum EncodedValueType;
		case String:
			return m_impl->DecodeString(encodedValue);

		case Number:
			return m_impl->DecodeNumber(encodedValue);

		case List:
			return m_impl->DecodeList(encodedValue);

		case Map:
			return m_impl->DecodeMap(encodedValue);

		default:
			throw std::runtime_error("Unhandled encoded value: " + encodedValue);
	}
}

Bittorent::Decoder::Decoder() = default;
Bittorent::Decoder::~Decoder() = default;
