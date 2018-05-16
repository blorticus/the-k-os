#include <string>
#include <iostream>
#include <TestSuite.h>

using namespace std;

BufferDescriptor::BufferDescriptor() : m_elements(0) {
}


BufferDescriptor* BufferDescriptor::addRepeatingValue( unsigned int count, unsigned int value ) {
    m_elements += count;

    descriptor* d = new descriptor;
    d->type = Repeating;
    d->length = count;
    d->value = value;
    
    m_descriptors.push_back(d);
    
    return this;
}


BufferDescriptor* BufferDescriptor::addExplicitValues( unsigned int count, unsigned int* values ) {
    m_elements += count;
    
    descriptor* d = new descriptor;
    d->type = Explicit;
    d->length = count;
    d->pvalue = values;
    
    m_descriptors.push_back(d);

    return this;
}


unsigned int* BufferDescriptor::inflate() {
    unsigned int* inflated = new unsigned int[m_elements];
    
    unsigned int i = 0;
    for (std::list<descriptor*>::iterator it = m_descriptors.begin(); it != m_descriptors.end(); it++) {
        descriptor* d = *it;
        switch (d->type) {
            case Repeating:
                for (unsigned int j = 0; j < d->length; j++)
                    inflated[i++] = d->value;            
                break;
            
            case Explicit:
                for (unsigned int j = 0; j < d->length; j++)
                    inflated[i++] = (d->pvalue)[j];
                break;
        }
    }
    
    return inflated;
}

void BufferDescriptor::reset() {
    m_descriptors.clear();
    m_elements = 0;
}

TestSuite::TestSuite()
    :m_name("TestSuite"),
     m_tests_run(0),
     m_tests_failed(0) {

}


TestSuite::TestSuite( string name )
    :m_name(name),
     m_tests_run(0),
     m_tests_failed(0) {

}

 
void TestSuite::assertBool( bool result, string testname ) {
    m_tests_run++;

    if (!result) {
        cerr << " -- failed test number [" << m_tests_run << "], named [" << testname << "]" << endl;
        m_tests_failed++;
    }
}


void TestSuite::assertArraysEqual( unsigned char have[], unsigned char expect[], int compare_bytes, string testname ) {
    m_tests_run++;

    for (int i = 0; i < compare_bytes; i++) {
        if (have[i] != expect[i]) {
            cerr << " -- failed test number [" << m_tests_run << "]: " << testname << ", at element [" << i << "], expected [" << hex << expect[i] << "], got [" << hex << have[i] << "]" << endl;
            m_tests_failed++;
            return;
        }
    }
}

void TestSuite::assertArraysEqual( unsigned int*  have, unsigned int*  expect, int compare_bytes, std::string testname ) {
    m_tests_run++;

    for (int i = 0; i < compare_bytes; i++) {
        if (have[i] != expect[i]) {
            cerr << " -- failed test number [" << m_tests_run << "]: " << testname << ", at element [" << i << "], expected [" << hex << expect[i] << "], got [" << hex << have[i] << "]" << endl;
            m_tests_failed++;
            return;
        }
    }
}


void TestSuite::assertArrayRepeats( unsigned int ar[], unsigned int expect, int ar_len, std::string testname ) {
    m_tests_run++;
    
    for (int i = 0; i < ar_len; i++) {
        if (ar[i] != expect) {
            cerr << " -- failed test number [" << m_tests_run << "]: " << testname << ", at byte [" << i << "], expected [" << hex << expect << "], got [" << hex << ar[i] << "]" << endl;
            m_tests_failed++;
            return;
        }
    }
}

int TestSuite::doneTesting() {
    cerr << "++ TEST [" << m_name << "] COMPLETE: Ran [" << m_tests_run << "] Tests: [" << (m_tests_run - m_tests_failed) << "] Succeeded, [" << m_tests_failed << "] Failed  -- " << (m_tests_failed > 0 ? "NOT OK" : "OK") << " -- " << endl;

    return m_tests_failed;
}


void TestSuite::printTwoValue2dArray( unsigned int* array, unsigned int columns, unsigned int rows, unsigned int v1 ) {
    cout << "<<<<<<<<<<" << endl;
    for (unsigned int i = 0; i < rows; i++) {
        for (unsigned int j = 0; j < columns; j++) {
            if (array[i*columns + j] == v1)
                cout << ".";
            else
                cout << "+";
        }
        cout << endl;
    }
    cout << ">>>>>>>>>>" << endl;
}