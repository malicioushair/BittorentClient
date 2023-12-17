#include <filesystem>
#include <gtest/gtest.h>

#include <../../../../../../dev/CodeCrafters/codecrafters-bittorrent-cpp/src/FileUtils/FileUtils.h>

TEST(FileUtils, TestReadWrite)
{
	const std::filesystem::path testFile("build/4tests/read_write_test.txt");
	FileUtils::Write(testFile, "foo bar buz");
	const auto actual = FileUtils::Read(testFile);
	const auto expected = std::string("foo bar buz");
	ASSERT_EQ(actual, expected);
}

TEST(FileUtils, TestAppend)
{
	const std::filesystem::path testFile("build/4tests/append_test.txt");
	FileUtils::Write(testFile, "foo bar buz");
	FileUtils::Append(testFile, " qux");
	const auto actual = FileUtils::Read(testFile);
	const auto expected = std::string("foo bar buz qux");
	ASSERT_EQ(actual, expected);
}

int main(int argc, char ** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}