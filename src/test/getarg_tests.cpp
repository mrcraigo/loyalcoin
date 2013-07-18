#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-lyc");
    BOOST_CHECK(GetBoolArg("-lyc"));
    BOOST_CHECK(GetBoolArg("-lyc", false));
    BOOST_CHECK(GetBoolArg("-lyc", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-lyco"));
    BOOST_CHECK(!GetBoolArg("-lyco", false));
    BOOST_CHECK(GetBoolArg("-lyco", true));

    ResetArgs("-lyc=0");
    BOOST_CHECK(!GetBoolArg("-lyc"));
    BOOST_CHECK(!GetBoolArg("-lyc", false));
    BOOST_CHECK(!GetBoolArg("-lyc", true));

    ResetArgs("-lyc=1");
    BOOST_CHECK(GetBoolArg("-lyc"));
    BOOST_CHECK(GetBoolArg("-lyc", false));
    BOOST_CHECK(GetBoolArg("-lyc", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-nolyc");
    BOOST_CHECK(!GetBoolArg("-lyc"));
    BOOST_CHECK(!GetBoolArg("-lyc", false));
    BOOST_CHECK(!GetBoolArg("-lyc", true));

    ResetArgs("-nolyc=1");
    BOOST_CHECK(!GetBoolArg("-lyc"));
    BOOST_CHECK(!GetBoolArg("-lyc", false));
    BOOST_CHECK(!GetBoolArg("-lyc", true));

    ResetArgs("-lyc -nolyc");  // -lyc should win
    BOOST_CHECK(GetBoolArg("-lyc"));
    BOOST_CHECK(GetBoolArg("-lyc", false));
    BOOST_CHECK(GetBoolArg("-lyc", true));

    ResetArgs("-lyc=1 -nolyc=1");  // -lyc should win
    BOOST_CHECK(GetBoolArg("-lyc"));
    BOOST_CHECK(GetBoolArg("-lyc", false));
    BOOST_CHECK(GetBoolArg("-lyc", true));

    ResetArgs("-lyc=0 -nolyc=0");  // -lyc should win
    BOOST_CHECK(!GetBoolArg("-lyc"));
    BOOST_CHECK(!GetBoolArg("-lyc", false));
    BOOST_CHECK(!GetBoolArg("-lyc", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--lyc=1");
    BOOST_CHECK(GetBoolArg("-lyc"));
    BOOST_CHECK(GetBoolArg("-lyc", false));
    BOOST_CHECK(GetBoolArg("-lyc", true));

    ResetArgs("--nolyc=1");
    BOOST_CHECK(!GetBoolArg("-lyc"));
    BOOST_CHECK(!GetBoolArg("-lyc", false));
    BOOST_CHECK(!GetBoolArg("-lyc", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-lyc", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-lyc", "eleven"), "eleven");

    ResetArgs("-lyc -bar");
    BOOST_CHECK_EQUAL(GetArg("-lyc", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-lyc", "eleven"), "");

    ResetArgs("-lyc=");
    BOOST_CHECK_EQUAL(GetArg("-lyc", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-lyc", "eleven"), "");

    ResetArgs("-lyc=11");
    BOOST_CHECK_EQUAL(GetArg("-lyc", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-lyc", "eleven"), "11");

    ResetArgs("-lyc=eleven");
    BOOST_CHECK_EQUAL(GetArg("-lyc", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-lyc", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-lyc", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-lyc", 0), 0);

    ResetArgs("-lyc -bar");
    BOOST_CHECK_EQUAL(GetArg("-lyc", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-lyc=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-lyc", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-lyc=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-lyc", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--lyc");
    BOOST_CHECK_EQUAL(GetBoolArg("-lyc"), true);

    ResetArgs("--lyc=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-lyc", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-nolyc");
    BOOST_CHECK(!GetBoolArg("-lyc"));
    BOOST_CHECK(!GetBoolArg("-lyc", true));
    BOOST_CHECK(!GetBoolArg("-lyc", false));

    ResetArgs("-nolyc=1");
    BOOST_CHECK(!GetBoolArg("-lyc"));
    BOOST_CHECK(!GetBoolArg("-lyc", true));
    BOOST_CHECK(!GetBoolArg("-lyc", false));

    ResetArgs("-nolyc=0");
    BOOST_CHECK(GetBoolArg("-lyc"));
    BOOST_CHECK(GetBoolArg("-lyc", true));
    BOOST_CHECK(GetBoolArg("-lyc", false));

    ResetArgs("-lyc --nolyc");
    BOOST_CHECK(GetBoolArg("-lyc"));

    ResetArgs("-nolyc -lyc"); // lyc always wins:
    BOOST_CHECK(GetBoolArg("-lyc"));
}

BOOST_AUTO_TEST_SUITE_END()
