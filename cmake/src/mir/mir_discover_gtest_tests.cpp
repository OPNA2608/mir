#include <algorithm>
#include <cassert>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <istream>
#include <ostream>
#include <fstream>
#include <iterator>
#include <iostream>
#include <libgen.h>

#include <getopt.h>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace std;

namespace
{
enum DescriptorType
{
    test_case,
    test_suite
};

DescriptorType check_line_for_test_case_or_suite(const string& line)
{
    if (line.find("  ") == 0)
        return test_case;

    return test_suite;
}

int get_terminal_width()
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    return w.ws_col;
}

std::string& ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

const char* ordinary_cmd_line_pattern()
{
    static const char* pattern = "ADD_TEST(\"%s.%s\" \"%s\" \"--gtest_filter=%s\")\n";
    return pattern;
}

const char* memcheck_cmd_line_pattern()
{
    static const char* pattern = "ADD_TEST(\"memcheck(%s.%s)\" \"valgrind\" \"--trace-children=yes\" \"%s\" \"--gtest_death_test_use_fork\" \"--gtest_filter=%s\")\n";

    return pattern;
}

std::string elide_string_right(const std::string& in, std::size_t max_size)
{
    assert(max_size >= 3);

    std::string result(in.begin(), in.begin() + max_size);

    if (in.size() >= max_size)
    {
        *(result.end()-1) = '.';
        *(result.end()-2) = '.';
        *(result.end()-3) = '.';
    }

    return result;
}

std::string elide_string_left(const std::string& in, std::size_t max_size)
{
    assert(max_size >= 3);

    if (in.size() <= max_size)
        return in;

    std::string result(in.begin() + (in.size() - max_size), in.end());

    *(result.begin()) = '.';
    *(result.begin()+1) = '.';
    *(result.begin()+2) = '.';

    return result;
}

struct Configuration
{
    Configuration() : executable(NULL),
                      enable_memcheck(0)
    {
    }

    const char* executable;
    int enable_memcheck;
};

bool parse_configuration_from_cmd_line(int argc, char** argv, Configuration& config)
{
    static struct option long_options[] = {
        {"executable", required_argument, NULL, 'e'},
        {"enable-memcheck", no_argument, &config.enable_memcheck, 1},
        {0, 0, 0, 0}
    };

    static const int executable_option_index = 0;
    static const int enable_memcheck_option_index = 1;

    while(1)
    {
        int option_index = 0;
        int c = getopt_long(
            argc,
            argv,
            "e:",
            long_options,
            &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c)
        {
            case 0:
                if (enable_memcheck_option_index == option_index)
                    break;
                else if (executable_option_index == option_index)
                    config.executable = optarg;
                else
                    return false;

                break;
            case 'e':
                config.executable = optarg;
                break;
        }
    }

    return true;
}
}

int main (int argc, char **argv)
{
    int terminal_width = get_terminal_width();
    
    cin >> noskipws;

    Configuration config;
    if (!parse_configuration_from_cmd_line(argc, argv, config) || config.executable == NULL)
    {
        cout << "Usage: PATH_TO_TEST_BINARY --gtest_list_tests | ./build_test_cases --executable PATH_TO_TEST_BINARY [--enable-memcheck]";
        return 1;
    }

    set<string> tests;
    string line;
    string current_test;

    while (getline (cin, line))
    {
        switch(check_line_for_test_case_or_suite(line))
        {
            case test_case:
                tests.insert(current_test + "*");
                break;
            case test_suite:
                current_test = line;
                break;
        }
    }

    ofstream testfilecmake;
    char *base = basename(argv[1]);
    string   test_suite(base);

    testfilecmake.open(string(test_suite  + "_test.cmake").c_str(), ios::out | ios::trunc);
    if (testfilecmake.is_open())
    {
        for (auto test = tests.begin(); test != tests.end(); ++ test)
        {
            static char cmd_line[1024] = "";
            snprintf(
                cmd_line,
                sizeof(cmd_line),
                config.enable_memcheck ? memcheck_cmd_line_pattern() : ordinary_cmd_line_pattern(),
                test_suite.c_str(),
                elide_string_left(*test, terminal_width/2).c_str(),
                config.executable,
                test->c_str());

            if (testfilecmake.good())
            {
                testfilecmake << cmd_line;
            }
        }

        testfilecmake.close();
    }

    ifstream CTestTestfile("CTestTestfile.cmake", ifstream::in);
    bool need_include = true;
    line.clear();

    string includeLine = string ("INCLUDE (") +
                         test_suite  +
                         string ("_test.cmake)");

    if (CTestTestfile.is_open())
    {
        while (CTestTestfile.good())
        {
            getline(CTestTestfile, line);

            if (line == includeLine)
                need_include = false;
        }

        CTestTestfile.close();
    }

    if (need_include)
    {
        ofstream CTestTestfileW ("CTestTestfile.cmake", ofstream::app | ofstream::out);

        if (CTestTestfileW.is_open())
        {
            CTestTestfileW << includeLine << endl;
            CTestTestfileW.close();
        }
    }

    return 0;
}
