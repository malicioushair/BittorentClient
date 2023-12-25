#include <gtest/gtest.h>

#include <Decoder/Decoder.h>

TEST(Decoder, TestString)
{
	Bittorent::Decoder decoder;
	const auto actual = decoder.DecodeBencodedValue("5:hello").dump();
	const auto expected = R"("hello")";
	ASSERT_EQ(actual, expected);
}

TEST(Decoder, TestSimpleBinaryNonACIIString)
{
	Bittorent::Decoder decoder;
	const auto actual = decoder.DecodeBencodedValue("12:йцукен").dump();
	const auto expected = "\"йцукен\"";
	const auto foo = std::string(expected).length();
	ASSERT_EQ(actual, expected);
}

// TEST(Decoder, TestBinaryDataString)
// {
// 	Bittorent::Decoder decoder;
// 	const auto expected = R"(иvцz*€†иуk\x13g&Г\xfў—\x3\x2-n\"uж\x4 vfVsnЃя\x10µR\x4­Ќ5р\r“z\x2\x13Я\x19‚јЌ\tr'­ћђљМ\x17)";
// 	const auto actual = decoder.DecodeBencodedValue(R"(60:иvцz*€†иуk\x13g&Г\xfў—\x3\x2-n\"uж\x4 vfVsnЃя\x10µR\x4­Ќ5р\r“z\x2\x13Я\x19‚јЌ\tr'­ћђљМ\x17)").dump();
// 	ASSERT_EQ(actual, expected);
// }

TEST(Decoder, TestNumber)
{
	Bittorent::Decoder decoder;
	const auto actual = decoder.DecodeBencodedValue("i52e").dump();
	const auto expected = "52";
	ASSERT_EQ(actual, expected);
}

TEST(Decoder, TestList)
{
	Bittorent::Decoder decoder;
	const auto actual = decoder.DecodeBencodedValue("l5:helloi52ee").dump();
	const auto expected = nlohmann::json { "hello", 52 }.dump();

	ASSERT_EQ(actual, expected);
}

TEST(Decoder, TestEmptyDict)
{
	Bittorent::Decoder decoder;
	const auto expected = nlohmann::json::object().dump();
	const auto actual = decoder.DecodeBencodedValue("de").dump();
	ASSERT_EQ(actual, expected);
}

TEST(Decoder, TestDict)
{
	Bittorent::Decoder decoder;
	const auto actual = decoder.DecodeBencodedValue("d3:foo3:bar5:helloi52ee").dump();
	const auto expected = nlohmann::json {
		{"foo",    "bar"},
		{ "hello", 52   }
	}.dump();
	ASSERT_EQ(actual, expected);
}

TEST(Decoder, TestEmptyString)
{
	Bittorent::Decoder decoder;
	const auto actual = decoder.DecodeBencodedValue("0:").dump();
	const auto expected = R"("")"; // or whatever is expected
	ASSERT_EQ(actual, expected);
}

TEST(Decoder, TestSpecialCharacters)
{
	Bittorent::Decoder decoder;
	const auto actual = decoder.DecodeBencodedValue("10:hi!@#$%^&*").dump();
	const auto expected = R"("hi!@#$%^&*")";
	ASSERT_EQ(actual, expected);
}

TEST(Decoder, TestNegativeNumber)
{
	Bittorent::Decoder decoder;
	const auto actual = decoder.DecodeBencodedValue("i-52e").dump();
	const auto expected = "-52";
	ASSERT_EQ(actual, expected);
}

TEST(Decoder, TestNestedList)
{
	Bittorent::Decoder decoder;
	const auto actual = decoder.DecodeBencodedValue("ll5:helloi52eee").dump();
	const auto expected = [] {
		nlohmann::json j;
		j.emplace_back("hello");
		j.emplace_back(52);

		nlohmann::json jj;
		jj.emplace_back(j);
		return jj.dump();
	}();
	ASSERT_EQ(actual, expected);
}

TEST(Decoder, TestNestedDict)
{
	Bittorent::Decoder decoder;
	const auto actual = decoder.DecodeBencodedValue("d3:food3:bari52eee").dump();
	const auto expected = nlohmann::json {
		{"foo", { { "bar", 52 } }}
	}.dump();
	ASSERT_EQ(actual, expected);
}

TEST(Decoder, TestEmptyList)
{
	Bittorent::Decoder decoder;
	const auto actual = decoder.DecodeBencodedValue("le").dump();
	const auto expected = nlohmann::json::array().dump();
	ASSERT_EQ(actual, expected);
}

TEST(Decoder, TestDictInList)
{
	Bittorent::Decoder decoder;
	const auto expected = [] {
		auto j = nlohmann::json::array();
		const auto jj = nlohmann::json::object({
			{"one", 1}
        });
		j.emplace_back(jj);
		return j.dump();
	}();
	const auto actual = decoder.DecodeBencodedValue("ld3:onei1eee").dump();
	ASSERT_EQ(actual, expected);
}

int main(int argc, char ** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}