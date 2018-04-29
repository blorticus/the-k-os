#ifndef INCLUDE_GATE__TEST_SUITE_H
#define INCLUDE_GATE__TEST_SUITE_H

#include <string>

class TestSuite {
    public:
        TestSuite();
        TestSuite( std::string name );  
        void assertBool( bool result, std::string testname );
        void assertArraysEqual( unsigned char* have, unsigned char* expect, int compare_bytes, std::string testname );
        void assertArrayRepeats( unsigned int ar[], unsigned int expect, int ar_len, std::string testname );
        int doneTesting();

    private:
        std::string m_name;
        int         m_tests_run;
        int         m_tests_failed;
};

#endif
