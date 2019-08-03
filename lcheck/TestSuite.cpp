#include <string>
#include <iostream>
#include <TestSuite.h>

using namespace std;

BufferDescriptor::BufferDescriptor() : m_elements(0) {
}


BufferDescriptor* BufferDescriptor::AddRepeatingValue( unsigned int count, unsigned int value ) {
    m_elements += count;

    descriptor* d = new descriptor;
    d->type = Repeating;
    d->length = count;
    d->value = value;
    
    m_descriptors.push_back(d);
    
    return this;
}


BufferDescriptor* BufferDescriptor::AddExplicitValues( unsigned int count, unsigned int* values ) {
    m_elements += count;
    
    descriptor* d = new descriptor;
    d->type = Explicit;
    d->length = count;
    d->pvalue = values;
    
    m_descriptors.push_back(d);

    return this;
}


BufferDescriptor* BufferDescriptor::ValuesFromStringImage( std::string image, unsigned int character_map_entry_count, BDImageValue* image_character_to_int_map ) {
    unsigned int map[256] = {};

    for (unsigned int i = 0; i < character_map_entry_count; i++) {
        BDImageValue v = image_character_to_int_map[i];
        map[(int)(v.CharacterInImage)] = v.ValueForCharacterInBuffer;
    }

    for (unsigned int i = 0; i < image.length(); i++) {
        unsigned int buf_value = map[(int)(image[i])];
        AddExplicitValues( 1, new uint32_t[1] { buf_value } );
    }

    return this;
}


unsigned int* BufferDescriptor::Inflate() {
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

void BufferDescriptor::Reset() {
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

 
void TestSuite::AssertBool( bool result, string testname ) {
    m_tests_run++;

    if (!result) {
        cerr << " -- failed test number [" << m_tests_run << "], named [" << testname << "]" << endl;
        m_tests_failed++;
    }
}


void TestSuite::AssertArraysEqual( unsigned char have[], unsigned char expect[], int compare_bytes, string testname ) {
    m_tests_run++;

    for (int i = 0; i < compare_bytes; i++) {
        if (have[i] != expect[i]) {
            cerr << " -- failed test number [" << m_tests_run << "]: " << testname << ", at element [" << i << "], expected [" << hex << expect[i] << "], got [" << hex << have[i] << "]" << endl;
            m_tests_failed++;
            return;
        }
    }
}

void TestSuite::AssertArraysEqual( unsigned int*  have, unsigned int*  expect, int compare_bytes, std::string testname ) {
    m_tests_run++;

    for (int i = 0; i < compare_bytes; i++) {
        if (have[i] != expect[i]) {
            cerr << " -- failed test number [" << m_tests_run << "]: " << testname << ", at element [" << i << "], expected [" << hex << expect[i] << "], got [" << hex << have[i] << "]" << endl;
            m_tests_failed++;
            return;
        }
    }
}


void TestSuite::AssertArrayRepeats( unsigned int ar[], unsigned int expect, int ar_len, std::string testname ) {
    m_tests_run++;
    
    for (int i = 0; i < ar_len; i++) {
        if (ar[i] != expect) {
            cerr << " -- failed test number [" << m_tests_run << "]: " << testname << ", at byte [" << i << "], expected [" << hex << expect << "], got [" << hex << ar[i] << "]" << endl;
            m_tests_failed++;
            return;
        }
    }
}

int TestSuite::DoneTesting() {
    cerr << "++ TEST [" << m_name << "] COMPLETE: Ran [" << m_tests_run << "] Tests: [" << (m_tests_run - m_tests_failed) << "] Succeeded, [" << m_tests_failed << "] Failed  -- " << (m_tests_failed > 0 ? "NOT OK" : "OK") << " -- " << endl;

    return m_tests_failed;
}


void TestSuite::PrintTwoValue2dArray( unsigned int* array, unsigned int columns, unsigned int rows, unsigned int v1 ) {
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
