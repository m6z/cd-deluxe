#include "stdafx.h"

#include <catch2/catch_test_macros.hpp>

#include "cdd/cdd2.h"
#include "test_helpers.h"

//----------------------------------------------------------------------

TEST_CASE("cdd2_test")
{
    SECTION("trim function")
    {
        REQUIRE(trim("   hello world   ") == "hello world");
        REQUIRE(trim("\n\t  test string \r\n") == "test string");
        REQUIRE(trim("no_whitespace") == "no_whitespace");
        REQUIRE(trim("    ") == "");
        REQUIRE(trim("") == "");
    }

    SECTION("KeyedPath ignoring case")
    {
        bool ignore_case = true;
        Cdd2::KeyedPath kp1a("/Path/One", ignore_case);
        Cdd2::KeyedPath kp1b("/path/one", ignore_case);
        Cdd2::KeyedPath kp2("/PATH/TWO", ignore_case);

        REQUIRE(kp1a == kp1b);
        REQUIRE(kp1a != kp2);
        REQUIRE(((kp1a < kp2) && (kp2 > kp1a)));
        REQUIRE(((kp1b < kp2) && (kp2 > kp1b)));

        REQUIRE(kp1a.get_dir_key() == "/path/one");
        REQUIRE(kp1b.get_dir_key() == "/path/one");
        REQUIRE(kp2.get_dir_key() == "/path/two");

        // std::cout << "(case no) kp1a: " << kp1a << std::endl;
        // std::cout << "(case no) kp1b: " << kp1b << std::endl;
        // std::cout << "(case no) kp2:  " << kp2 << std::endl;
    }

    SECTION("KeyedPath using case")
    {
        bool ignore_case = false;
        Cdd2::KeyedPath kp1a("/Path/One", ignore_case);
        Cdd2::KeyedPath kp1b("/path/one", ignore_case);
        Cdd2::KeyedPath kp2("/PATH/TWO", ignore_case);

        REQUIRE(kp1a != kp1b);
        REQUIRE(kp1a != kp2);
        REQUIRE(((kp1a < kp1b) && (kp1b > kp1a)));

        REQUIRE(kp1a.get_dir_key() == "/Path/One");
        REQUIRE(kp1b.get_dir_key() == "/path/one");
        REQUIRE(kp2.get_dir_key() == "/PATH/TWO");

        // std::cout << "(case yes) kp1a: " << kp1a << std::endl;
        // std::cout << "(case yes) kp1b: " << kp1b << std::endl;
        // std::cout << "(case yes) kp2:  " << kp2 << std::endl;
    }

    SECTION("class constructors")
    {
        bool ignore_case = true;
        Cdd2::KeyedPath kp1("/Path/One", ignore_case);
        auto kp2 = kp1; // copy constructor
        REQUIRE(kp1 == kp2);
        auto kp3 = std::move(kp2); // move constructor
        REQUIRE(kp1 == kp3);
        std::swap(kp1, kp3); // fundamental for sorting algorithms
        REQUIRE(kp1 == kp3);

        Cdd2::CommonPath cp1(5, 1, kp1);
        auto cp2 = cp1; // copy constructor
        REQUIRE(cp1 == cp2);
        auto cp3 = std::move(cp2); // move constructor
        REQUIRE(cp1 == cp3);
        std::swap(cp1, cp3); // fundamental for sorting algorithms
        REQUIRE(cp1 == cp3);
    }

    SECTION("Cdd2_internals")
    {
        auto cdd = cdd_test({"_cdd", "--ignore-case"}, "", "/current/working/dir", R"(
            /path/one
            /path/two
            /path/Three
            /path/TWO
        )");

        auto dirs_last_to_first = cdd.create_dirs_last_to_first();
        REQUIRE(dirs_last_to_first.size() == 3);
        REQUIRE(dirs_last_to_first[0].prefix1 == " -1");
        REQUIRE(dirs_last_to_first[0].path == "/path/one");
        REQUIRE(dirs_last_to_first[1].prefix1 == " -2");
        REQUIRE(dirs_last_to_first[1].path == "/path/two");
        REQUIRE(dirs_last_to_first[2].prefix1 == " -3");
        REQUIRE(dirs_last_to_first[2].path == "/path/Three");

        auto dirs_first_to_last = cdd.create_dirs_first_to_last();
        REQUIRE(dirs_first_to_last.size() == 3);
        REQUIRE(dirs_first_to_last[0].prefix1 == "  0");
        REQUIRE(dirs_first_to_last[0].path == "/path/TWO");
        REQUIRE(dirs_first_to_last[1].prefix1 == "  1");
        REQUIRE(dirs_first_to_last[1].path == "/path/Three");
        REQUIRE(dirs_first_to_last[2].prefix1 == "  2");
        REQUIRE(dirs_first_to_last[2].path == "/path/one");

        auto dirs_most_to_least = cdd.create_dirs_most_to_least();
        REQUIRE(dirs_most_to_least.size() == 3);

        REQUIRE(dirs_most_to_least[0].get_keyed_path().get_dir_path() == "/path/two");
        REQUIRE(dirs_most_to_least[1].get_keyed_path().get_dir_path() == "/path/one");
        REQUIRE(dirs_most_to_least[2].get_keyed_path().get_dir_path() == "/path/Three");

        auto dirs_upwards = cdd.create_dirs_upwards();
        REQUIRE(dirs_upwards.size() == 2);
        REQUIRE(dirs_upwards[0].prefix1 == "..1");
        REQUIRE(nix_path(dirs_upwards[0].path) == "/current/working");
        REQUIRE(dirs_upwards[1].prefix1 == "..2");
        REQUIRE(nix_path(dirs_upwards[1].path) == "/current");
    }

    SECTION("back_none")
    {
        auto cdd = cdd_test({"_cdd", "-"}, "", "/current/working/dir", "");
        cdd.process();
        REQUIRE("" == cdd.get_out_str());
        REQUIRE("No history of directories\n" == cdd.get_err_str());
    }

    SECTION("back_dash_zero")
    {
        auto cdd = cdd_test({"_cdd", "-0"}, "", "/current/working/dir", "");
        cdd.process();
        REQUIRE("" == cdd.get_out_str());
        REQUIRE("No history of directories\n" == cdd.get_err_str());
    }

    SECTION("cdd_to_dir")
    {
        auto cdd = cdd_test({"_cdd", "/tmp/z"}, "", "/current/working/dir", "");
        cdd._is_directory = true;
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd \"/tmp/z\"\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp/z'\n" == cdd.get_out_str());
#endif
    }

    SECTION("cdd_to_dots")
    {
        auto cdd = cdd_test({"_cdd", "..."}, "", "/current/working/dir", "");
        cdd._is_directory = true;
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd \"..\\\\..\"\n" == cdd.get_out_str());
        REQUIRE("cdd: ..\\..\n" == cdd.get_err_str());
#else
        REQUIRE("pushd '../..'\n" == cdd.get_out_str());
        REQUIRE("cdd: ../..\n" == cdd.get_err_str());
#endif
    }

    SECTION("cdd_to_file")
    {
        auto cdd = cdd_test({"_cdd", "/tmp/z.tmp"}, "", "/current/working/dir", "");
        cdd._is_regular_file = true;
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd \"/tmp\"\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: /tmp\n" == cdd.get_err_str());
    }

    //----------------------------------------------------------------------

    SECTION("back_one_dash")
    {
        auto cdd = cdd_test({"_cdd", "-"}, "", "/tmp/a", R"(
            /tmp/a
            /tmp/b
            /tmp/c
        )");
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd \"/tmp/b\"\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp/b'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: /tmp/b\n" == cdd.get_err_str());
    }

    SECTION("back_dash_one")
    {
        auto cdd = cdd_test({"_cdd", "-1"}, "", "/tmp/a", R"(
            /tmp/a
            /tmp/b
            /tmp/c
        )");
        cdd.process();
        // cdd.get_options().output();
#ifdef WIN32
        REQUIRE("pushd \"/tmp/b\"\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp/b'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: /tmp/b\n" == cdd.get_err_str());
    }

    SECTION("back_two_dash")
    {
        auto cdd = cdd_test({"_cdd", "--"}, "", "/tmp/a", R"(
            /tmp/a
            /tmp/b
            /tmp/c
        )");
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd \"/tmp/c\"\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp/c'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: /tmp/c\n" == cdd.get_err_str());
    }

    SECTION("back_dash_two")
    {
        auto cdd = cdd_test({"_cdd", "-2"}, "", "/tmp/a", R"(
            /tmp/a
            /tmp/b
            /tmp/c
        )");
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd \"/tmp/c\"\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp/c'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: /tmp/c\n" == cdd.get_err_str());
    }

    SECTION("back_dash_three")
    {
        auto cdd = cdd_test({"_cdd", "-3"}, "", "/tmp/a", R"(
            /tmp/a
            /tmp/b
            /tmp/c
        )");
        cdd.process();
        REQUIRE(cdd.get_out_str().empty());
        REQUIRE("No directory at -3\n" == cdd.get_err_str());
    }

    //----------------------------------------------------------------------

    SECTION("forward_zero")
    {
        auto cdd = cdd_test({"_cdd", "0"}, "", "/tmp/a",
                            {
                                "/tmp/b", // fourth visited
                                "/tmp/c", // third visited
                                "/tmp/b", // second visited
                                "/tmp/a", // first visited
                            });

        cdd.process();
#ifdef WIN32
        REQUIRE("pushd \"/tmp/a\"\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp/a'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: /tmp/a\n" == cdd.get_err_str());
    }

    SECTION("forward_plus_zero")
    {
        auto cdd = cdd_test({"_cdd", "+0"}, "", "/tmp/a",
                            {
                                "/tmp/b", // fourth visited
                                "/tmp/c", // third visited
                                "/tmp/b", // second visited
                                "/tmp/a", // first visited
                            });

        cdd.process();
#ifdef WIN32
        REQUIRE("pushd \"/tmp/a\"\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp/a'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: /tmp/a\n" == cdd.get_err_str());
    }

    SECTION("forward_one_plus")
    {
        auto cdd = cdd_test({"_cdd", "+"}, "", "/tmp/a",
                            {
                                "/tmp/b", // fourth visited
                                "/tmp/c", // third visited
                                "/tmp/b", // second visited
                                "/tmp/a", // first visited
                            });

        cdd.process();
#ifdef WIN32
        REQUIRE("pushd \"/tmp/a\"\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp/a'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: /tmp/a\n" == cdd.get_err_str());
    }

    SECTION("forward_one")
    {
        auto cdd = cdd_test({"_cdd", "1"}, "", "/tmp/a",
                            {
                                "/tmp/b", // fourth visited
                                "/tmp/c", // third visited
                                "/tmp/b", // second visited
                                "/tmp/a", // first visited
                            });
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd \"/tmp/b\"\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp/b'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: /tmp/b\n" == cdd.get_err_str());
    }

    SECTION("forward_plus_one")
    {
        auto cdd = cdd_test({"_cdd", "+1"}, "", "/tmp/a",
                            {
                                "/tmp/b", // fourth visited
                                "/tmp/c", // third visited
                                "/tmp/b", // second visited
                                "/tmp/a", // first visited
                            });
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd \"/tmp/b\"\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp/b'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: /tmp/b\n" == cdd.get_err_str());
    }

    SECTION("forward_two_plus")
    {
        auto cdd = cdd_test({"_cdd", "++"}, "", "/tmp/a",
                            {
                                "/tmp/b", // fourth visited
                                "/tmp/c", // third visited
                                "/tmp/b", // second visited
                                "/tmp/a", // first visited
                            });

        cdd.process();
#ifdef WIN32
        REQUIRE("pushd \"/tmp/b\"\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp/b'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: /tmp/b\n" == cdd.get_err_str());
    }

    SECTION("forward_plus_two")
    {
        auto cdd = cdd_test({"_cdd", "+2"}, "", "/tmp/a",
                            {
                                "/tmp/b", // fourth visited
                                "/tmp/c", // third visited
                                "/tmp/b", // second visited
                                "/tmp/a", // first visited
                            });
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd \"/tmp/c\"\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp/c'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: /tmp/c\n" == cdd.get_err_str());
    }

    SECTION("forward_three_plus")
    {
        auto cdd = cdd_test({"_cdd", "+++"}, "", "/tmp/a",
                            {
                                "/tmp/b", // fourth visited
                                "/tmp/c", // third visited
                                "/tmp/b", // second visited
                                "/tmp/a", // first visited
                            });
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd \"/tmp/c\"\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp/c'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: /tmp/c\n" == cdd.get_err_str());
    }

    SECTION("forward_plus_three")
    {
        auto cdd = cdd_test({"_cdd", "+3"}, "", "/tmp/a",
                            {
                                "/tmp/b", // fourth visited
                                "/tmp/c", // third visited
                                "/tmp/b", // second visited
                                "/tmp/a", // first visited
                            });
        cdd.process();
        REQUIRE("" == cdd.get_out_str());
        REQUIRE("No directory at +3\n" == cdd.get_err_str());
    }

    SECTION("forward_four_plus")
    {
        auto cdd = cdd_test({"_cdd", "++++"}, "", "/tmp/a",
                            {
                                "/tmp/b", // fourth visited
                                "/tmp/c", // third visited
                                "/tmp/b", // second visited
                                "/tmp/a", // first visited
                            });
        cdd.process();
        REQUIRE("" == cdd.get_out_str());
        REQUIRE("No directory at +3\n" == cdd.get_err_str());
    }

    //----------------------------------------------------------------------

    SECTION("common_comma_and_zero")
    {
        auto cdd = cdd_test({"_cdd", ","}, "", "/tmp/a",
                            {
                                "/tmp/b", // fourth visited
                                "/tmp/c", // third visited
                                "/tmp/b", // second visited
                                "/tmp/a", // first visited
                            });
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd /tmp/b\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp/b'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: ( 2) /tmp/b\n" == cdd.get_err_str());
    }

    SECTION("common_comma_zero")
    {
        auto cdd = cdd_test({"_cdd", ",0"}, "", "/tmp/a",
                            {
                                "/tmp/b", // fourth visited
                                "/tmp/c", // third visited
                                "/tmp/b", // second visited
                                "/tmp/a", // first visited
                            });
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd /tmp/b\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp/b'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: ( 2) /tmp/b\n" == cdd.get_err_str());
    }

    SECTION("common_one_comma")
    {
        auto cdd = cdd_test({"_cdd", ","}, "", "/tmp/a",
                            {
                                "/tmp/b", // fourth visited
                                "/tmp/c", // third visited
                                "/tmp/b", // second visited
                                "/tmp/a", // first visited
                            });
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd /tmp/b\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp/b'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: ( 2) /tmp/b\n" == cdd.get_err_str());
    }

    SECTION("common_comma_one")
    {
        auto cdd = cdd_test({"_cdd", ",1"}, "", "/tmp/a",
                            {
                                "/tmp/b", // fourth visited
                                "/tmp/c", // third visited
                                "/tmp/b", // second visited
                                "/tmp/a", // first visited
                            });
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd /tmp/c\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp/c'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: ( 1) /tmp/c\n" == cdd.get_err_str());
    }

    SECTION("common_two_commas")
    {
        auto cdd = cdd_test({"_cdd", ",,"}, "", "/tmp/a",
                            {
                                "/tmp/b", // fourth visited
                                "/tmp/c", // third visited
                                "/tmp/b", // second visited
                                "/tmp/a", // first visited
                            });
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd /tmp/c\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp/c'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: ( 1) /tmp/c\n" == cdd.get_err_str());
    }

    SECTION("common_comma_two")
    {
        auto cdd = cdd_test({"_cdd", ",2"}, "", "/tmp/a",
                            {
                                "/tmp/b", // fourth visited
                                "/tmp/c", // third visited
                                "/tmp/b", // second visited
                                "/tmp/a", // first visited
                            });
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd /tmp/a\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp/a'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: ( 1) /tmp/a\n" == cdd.get_err_str());
    }

    SECTION("common_three_commas")
    {
        auto cdd = cdd_test({"_cdd", ",,,"}, "", "/tmp/a",
                            {
                                "/tmp/b", // fourth visited
                                "/tmp/c", // third visited
                                "/tmp/b", // second visited
                                "/tmp/a", // first visited
                            });
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd /tmp/a\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp/a'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: ( 1) /tmp/a\n" == cdd.get_err_str());
    }

    SECTION("common_comma_three")
    {
        auto cdd = cdd_test({"_cdd", ",3"}, "", "/tmp/a",
                            {
                                "/tmp/b", // fourth visited
                                "/tmp/c", // third visited
                                "/tmp/b", // second visited
                                "/tmp/a", // first visited
                            });
        cdd.process();
        REQUIRE("" == cdd.get_out_str());
        REQUIRE("No directory at ,3\n" == cdd.get_err_str());
    }

    SECTION("common_four_commas")
    {
        auto cdd = cdd_test({"_cdd", ",,,,"}, "", "/tmp/a",
                            {
                                "/tmp/b", // fourth visited
                                "/tmp/c", // third visited
                                "/tmp/b", // second visited
                                "/tmp/a", // first visited
                            });
        cdd.process();
        REQUIRE("" == cdd.get_out_str());
        REQUIRE("No directory at ,3\n" == cdd.get_err_str());
    }

    SECTION("list_forward")
    {
        auto cdd = cdd_test({"_cdd", "--list", "--direction=+"}, "", "/tmp/a", //
                            {"/d", "/b", "/e", "/d", "/c", "/b", "/a"});
        cdd.process();
        // cdd.get_options().output();
        REQUIRE(cdd.get_out_str().empty());
        REQUIRE(cdd.get_err_str() == "  0: /a\n"
                                     "  1: /b\n"
                                     "  2: /c\n"
                                     "  3: /d\n"
                                     "  4: /e\n");
    }

    SECTION("list_forward_pattern")
    {
        auto cdd = cdd_test({"_cdd", "--list", "--direction=+", "b"}, "", "/tmp/a", //
                            {"/d", "/b", "/e", "/d", "/c", "/b", "/a"});
        cdd.process();
        REQUIRE(cdd.get_out_str().empty());
        REQUIRE(cdd.get_err_str() == "  1: /b\n");
    }

    SECTION("list_backwards")
    {
        auto cdd = cdd_test({"_cdd", "--list", "-d", "-"}, "", "/tmp/a", //
                            {"/d", "/b", "/e", "/d", "/c", "/b", "/a"});
        cdd.process();
        // cdd.get_options().output();
        REQUIRE(cdd.get_out_str().empty());
        REQUIRE(cdd.get_err_str() == " -1: /d\n"
                                     " -2: /b\n"
                                     " -3: /e\n"
                                     " -4: /c\n"
                                     " -5: /a\n");
    }

    SECTION("list_backwards_pattern")
    {
        auto cdd = cdd_test({"_cdd", "--list", "-d", "-", "e"}, "", "/tmp/a", //
                            {"/d", "/b", "/e", "/d", "/c", "/b", "/a"});
        cdd.process();
        REQUIRE(cdd.get_out_str().empty());
        REQUIRE(cdd.get_err_str() == " -3: /e\n");
    }

    SECTION("list_common")
    {
        auto cdd = cdd_test({"_cdd", "-l", "--direction", ","}, "", "/tmp/a", //
                            {"/d", "/b", "/e", "/d", "/c", "/b", "/a"});
        cdd.process();
        REQUIRE(cdd.get_out_str().empty());
        REQUIRE(cdd.get_err_str() == " ,0: ( 2) /d\n"
                                     " ,1: ( 2) /b\n"
                                     " ,2: ( 1) /e\n"
                                     " ,3: ( 1) /c\n"
                                     " ,4: ( 1) /a\n");
    }

    SECTION("list_common_pattern")
    {
        auto cdd = cdd_test({"_cdd", "--list", "-d,", "b"}, "", "/tmp/a", //
                            {"/d", "/b", "/e", "/d", "/c", "/b", "/a"});
        cdd.process();
        REQUIRE(cdd.get_out_str().empty());
        REQUIRE(cdd.get_err_str() == " ,1: ( 2) /b\n");
    }

    SECTION("list_upwards")
    {
        auto cdd = cdd_test({"_cdd", "--list", "--direction=.."}, "", "/tmp/a/b/c", "");
        cdd.process();
        REQUIRE(cdd.get_out_str().empty());
        REQUIRE(cdd.get_err_str() == "..1: /tmp/a/b\n"
                                     "..2: /tmp/a\n"
                                     "..3: /tmp\n");
    }

    SECTION("list_upwards_pattern")
    {
        auto cdd = cdd_test({"_cdd", "--list", "--direction=..", "a"}, "", "/tmp/a/aa/b/c", "");
        cdd.process();
        REQUIRE(cdd.get_out_str().empty());
        REQUIRE(cdd.get_err_str() == "..2: /tmp/a/aa\n"
                                     "..3: /tmp/a\n");
    }
}

//----------------------------------------------------------------------

static vector<string> arr_test_dirs = {
    "/cc/dd", // sixth visited
    "/bb/ee", // fifth visited
    "/aa/bb", // fourth visited
    "/cc/dd", // third visited
    "/bb/cc", // second visited
    "/aa/bb", // first visited
};

TEST_CASE("match_test")
{
    SECTION("simple_match_backwards")
    {
        auto cdd = cdd_test({"_cdd", "ee"}, "", "/current/dir", arr_test_dirs);
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd /bb/ee\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/bb/ee'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: /bb/ee\n" == cdd.get_err_str());
    }

    SECTION("simple_multimatch_backwards")
    {
        auto cdd = cdd_test({"_cdd", "cc$"}, "", "/current/dir", arr_test_dirs);
        cdd.process();
#ifdef WIN32
        REQUIRE(0 == 1); // TODO
#else
        REQUIRE("pushd '/cc/dd'\n" == cdd.get_out_str());
#endif
        REQUIRE(cdd.get_err_str() == "cdd: /cc/dd\n"
                                     " -4: /bb/cc\n");
    }

    SECTION("simple_match_tail_backwards")
    {
        auto cdd = cdd_test({"_cdd", "bb/"}, "", "/current/dir", arr_test_dirs);
        cdd.process();
#ifdef WIN32
        REQUIRE(0 == 1); // TODO
#else
        REQUIRE("pushd '/aa/bb'\n" == cdd.get_out_str());
#endif
        REQUIRE(cdd.get_err_str() == "cdd: /aa/bb\n");
    }

    SECTION("simple_match_forward")
    {
        auto cdd = cdd_test({"_cdd", "dd"}, "", "/current/dir", arr_test_dirs);
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd /cc/dd\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/cc/dd'\n" == cdd.get_out_str());
#endif
        REQUIRE("cdd: /cc/dd\n" == cdd.get_err_str());
    }

    SECTION("simple_match_common")
    {
        auto cdd = cdd_test({"_cdd", "-d,", "cc$"}, "", "/current/dir", arr_test_dirs);
        cdd.process();
#ifdef WIN32
        REQUIRE(0 == 1); // TODO
#else
        REQUIRE("pushd '/cc/dd'\n" == cdd.get_out_str());
#endif
        REQUIRE(cdd.get_err_str() == "cdd: ( 2) /cc/dd\n"
                                     " ,3: ( 1) /bb/cc\n");
    }
}

TEST_CASE("upwards_test")
{
    SECTION("up_a")
    {
        auto cdd = cdd_test({"_cdd", "-d..", "a"}, "", "/tmp/a/b/c", "");
        cdd._is_directory = true;
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd /tmp/a\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/tmp/a'\n" == cdd.get_out_str());
#endif
    }

    SECTION("up_b")
    {
        auto cdd = cdd_test({"_cdd", "--direction=..", "b$"}, "", "/a/b/bc/d/e/f/g", "");
        cdd._is_directory = true;
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd /a/b\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/a/b'\n" == cdd.get_out_str());
#endif
    }
}

TEST_CASE("adhoc_tests")
{
    SECTION("adhoc_last_to_first_list_default")
    {
        auto cdd = cdd_test({"_cdd"}, "", "/a", {"/a", "/b", "/c"});
        cdd.process();
        REQUIRE(cdd.get_out_str().empty());
        REQUIRE(cdd.get_err_str() == " -1: /b\n"
                                     " -2: /c\n");
    }

    SECTION("adhoc_last_to_first_list_specified")
    {
        auto cdd = cdd_test({"_cdd", "-d-"}, "", "/a", {"/a", "/b", "/c"});
        cdd.process();
        REQUIRE(cdd.get_out_str().empty());
        REQUIRE(cdd.get_err_str() == " -1: /b\n"
                                     " -2: /c\n");
    }

    SECTION("adhoc_last_to_first_changed_specified")
    {
        auto cdd = cdd_test({"_cdd", "-d-", "d"}, "", "/a", {"/a", "/bc", "/cd", "/de"});
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd /cd\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/cd'\n" == cdd.get_out_str());
#endif
        REQUIRE(cdd.get_err_str() == "cdd: /cd\n"
                                     " -3: /de\n");
    }

    SECTION("adhoc_last_to_first_changed_shorthand")
    {
        auto cdd = cdd_test({"_cdd", "-", "d"}, "", "/a", {"/a", "/bc", "/cd", "/de"});
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd /cd\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/cd'\n" == cdd.get_out_str());
#endif
        REQUIRE(cdd.get_err_str() == "cdd: /cd\n"
                                     " -3: /de\n");
    }

    SECTION("adhoc_first_to_last_changed_shorthand")
    {
        auto cdd = cdd_test({"_cdd", "+", "d"}, "", "/a", {"/a", "/bc", "/cd", "/de"});
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd /de\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/de'\n" == cdd.get_out_str());
#endif
        REQUIRE(cdd.get_err_str() == "cdd: /de\n"
                                     "  1: /cd\n");
    }

    SECTION("adhoc_most_common_changed_shorthand")
    {
        auto cdd = cdd_test({"_cdd", ",", "d"}, "", "/a", {"/a", "/bc", "/cd", "/de", "/de"});
        cdd.process();
#ifdef WIN32
        REQUIRE("pushd /de\n" == cdd.get_out_str());
#else
        REQUIRE("pushd '/de'\n" == cdd.get_out_str());
#endif
        REQUIRE(cdd.get_err_str() == "cdd: ( 2) /de\n"
                                     " ,3: ( 1) /cd\n");
    }

    SECTION("adhoc_upwards_changed_shorthand")
    {
        {
            auto cdd = cdd_test({"_cdd", "..", "c"}, "", "/ab/bc/cd/ef/fg", "");
            cdd._is_directory = true;
            cdd.process();
#ifdef WIN32
            REQUIRE("pushd /ab/bc/cd\n" == cdd.get_out_str());
#else
            REQUIRE("pushd '/ab/bc/cd'\n" == cdd.get_out_str());
#endif
        }
        {
            auto cdd = cdd_test({"_cdd", "..", "c$"}, "", "/ab/bc/cd/ef/fg", "");
            cdd._is_directory = true;
            cdd.process();
#ifdef WIN32
            REQUIRE("pushd /ab/bc\n" == cdd.get_out_str());
#else
            REQUIRE("pushd '/ab/bc'\n" == cdd.get_out_str());
#endif
        }
    }
}
