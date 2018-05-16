#pragma once

#include <string>
#include <list>


class BufferDescriptor {
    enum descriptorType { Repeating, Explicit };
    
    struct descriptor {
        descriptorType type;
        unsigned int length;
        unsigned int* pvalue;
        unsigned int value;
    };
    
    public:
        BufferDescriptor();
        BufferDescriptor* addRepeatingValue( unsigned int count, unsigned int value );
        BufferDescriptor* addExplicitValues( unsigned int length, unsigned int* values );
        unsigned int* inflate();
        void reset();

    private:
        std::list<descriptor*> m_descriptors;
        unsigned int m_elements;
};


class TestSuite {
    public:
        TestSuite();
        TestSuite( std::string name );  
        void assertBool( bool result, std::string testname );
        void assertArraysEqual( unsigned char* have, unsigned char* expect, int compare_bytes, std::string testname );
        void assertArraysEqual( unsigned int*  have, unsigned int*  expect, int compare_bytes, std::string testname );
        void assertArrayRepeats( unsigned int ar[], unsigned int expect, int ar_len, std::string testname );

        void printTwoValue2dArray( unsigned int* array, unsigned int columns, unsigned int rows, unsigned int v1 );
        
        int doneTesting();

    private:
        std::string m_name;
        int         m_tests_run;
        int         m_tests_failed;
};
