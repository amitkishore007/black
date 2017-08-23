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
    ResetArgs("-BDC");
    BOOST_CHECK(GetBoolArg("-BDC"));
    BOOST_CHECK(GetBoolArg("-BDC", false));
    BOOST_CHECK(GetBoolArg("-BDC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-BDCo"));
    BOOST_CHECK(!GetBoolArg("-BDCo", false));
    BOOST_CHECK(GetBoolArg("-BDCo", true));

    ResetArgs("-BDC=0");
    BOOST_CHECK(!GetBoolArg("-BDC"));
    BOOST_CHECK(!GetBoolArg("-BDC", false));
    BOOST_CHECK(!GetBoolArg("-BDC", true));

    ResetArgs("-BDC=1");
    BOOST_CHECK(GetBoolArg("-BDC"));
    BOOST_CHECK(GetBoolArg("-BDC", false));
    BOOST_CHECK(GetBoolArg("-BDC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noBDC");
    BOOST_CHECK(!GetBoolArg("-BDC"));
    BOOST_CHECK(!GetBoolArg("-BDC", false));
    BOOST_CHECK(!GetBoolArg("-BDC", true));

    ResetArgs("-noBDC=1");
    BOOST_CHECK(!GetBoolArg("-BDC"));
    BOOST_CHECK(!GetBoolArg("-BDC", false));
    BOOST_CHECK(!GetBoolArg("-BDC", true));

    ResetArgs("-BDC -noBDC");  // -BDC should win
    BOOST_CHECK(GetBoolArg("-BDC"));
    BOOST_CHECK(GetBoolArg("-BDC", false));
    BOOST_CHECK(GetBoolArg("-BDC", true));

    ResetArgs("-BDC=1 -noBDC=1");  // -BDC should win
    BOOST_CHECK(GetBoolArg("-BDC"));
    BOOST_CHECK(GetBoolArg("-BDC", false));
    BOOST_CHECK(GetBoolArg("-BDC", true));

    ResetArgs("-BDC=0 -noBDC=0");  // -BDC should win
    BOOST_CHECK(!GetBoolArg("-BDC"));
    BOOST_CHECK(!GetBoolArg("-BDC", false));
    BOOST_CHECK(!GetBoolArg("-BDC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--BDC=1");
    BOOST_CHECK(GetBoolArg("-BDC"));
    BOOST_CHECK(GetBoolArg("-BDC", false));
    BOOST_CHECK(GetBoolArg("-BDC", true));

    ResetArgs("--noBDC=1");
    BOOST_CHECK(!GetBoolArg("-BDC"));
    BOOST_CHECK(!GetBoolArg("-BDC", false));
    BOOST_CHECK(!GetBoolArg("-BDC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BDC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BDC", "eleven"), "eleven");

    ResetArgs("-BDC -bar");
    BOOST_CHECK_EQUAL(GetArg("-BDC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BDC", "eleven"), "");

    ResetArgs("-BDC=");
    BOOST_CHECK_EQUAL(GetArg("-BDC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BDC", "eleven"), "");

    ResetArgs("-BDC=11");
    BOOST_CHECK_EQUAL(GetArg("-BDC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-BDC", "eleven"), "11");

    ResetArgs("-BDC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-BDC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-BDC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BDC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-BDC", 0), 0);

    ResetArgs("-BDC -bar");
    BOOST_CHECK_EQUAL(GetArg("-BDC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-BDC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-BDC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-BDC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-BDC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--BDC");
    BOOST_CHECK_EQUAL(GetBoolArg("-BDC"), true);

    ResetArgs("--BDC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-BDC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noBDC");
    BOOST_CHECK(!GetBoolArg("-BDC"));
    BOOST_CHECK(!GetBoolArg("-BDC", true));
    BOOST_CHECK(!GetBoolArg("-BDC", false));

    ResetArgs("-noBDC=1");
    BOOST_CHECK(!GetBoolArg("-BDC"));
    BOOST_CHECK(!GetBoolArg("-BDC", true));
    BOOST_CHECK(!GetBoolArg("-BDC", false));

    ResetArgs("-noBDC=0");
    BOOST_CHECK(GetBoolArg("-BDC"));
    BOOST_CHECK(GetBoolArg("-BDC", true));
    BOOST_CHECK(GetBoolArg("-BDC", false));

    ResetArgs("-BDC --noBDC");
    BOOST_CHECK(GetBoolArg("-BDC"));

    ResetArgs("-noBDC -BDC"); // BDC always wins:
    BOOST_CHECK(GetBoolArg("-BDC"));
}

BOOST_AUTO_TEST_SUITE_END()
