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
        BufferDescriptor* AddRepeatingValue( unsigned int count, unsigned int value );
        BufferDescriptor* AddExplicitValues( unsigned int length, unsigned int* values );
        unsigned int* Inflate();
        void Reset();

    private:
        std::list<descriptor*> m_descriptors;
        unsigned int m_elements;
};


class TestSuite {
    public:
        TestSuite();
        TestSuite( std::string name );  
        void AssertBool( bool result, std::string testname );
        void AssertArraysEqual( unsigned char* have, unsigned char* expect, int compare_bytes, std::string testname );
        void AssertArraysEqual( unsigned int*  have, unsigned int*  expect, int compare_bytes, std::string testname );
        void AssertArrayRepeats( unsigned int ar[], unsigned int expect, int ar_len, std::string testname );

        void PrintTwoValue2dArray( unsigned int* array, unsigned int columns, unsigned int rows, unsigned int v1 );
        
        int DoneTesting();

    private:
        std::string m_name;
        int         m_tests_run;
        int         m_tests_failed;
};
