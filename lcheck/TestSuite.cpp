#include <string>
#include <iostream>
#include <TestSuite.h>

using namespace std;


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

 
void TestSuite::failUnless( bool result, string testname ) {
    m_tests_run++;

    if (!result) {
        cerr << " -- failed test number [" << m_tests_run << "], named [" << testname << "]" << endl;
        m_tests_failed++;
    }
}


void TestSuite::failUnless( unsigned char have[], unsigned char expect[], int compare_bytes, string testname ) {
    m_tests_run++;

    for (int i = 0; i < compare_bytes; i++) {
        if (have[i] != expect[i]) {
            cerr << " -- failed test number [" << m_tests_run << "]: " << testname << ", at byte [" << i << "], expected [" << hex << expect[i] << "], got [" << hex << have[i] << "]" << endl;
            m_tests_failed++;
            return;
        }
    }
}


int TestSuite::doneTesting() {
    cerr << "++ TEST [" << m_name << "] COMPLETE: Ran [" << m_tests_run << "] Tests: [" << (m_tests_run - m_tests_failed) << "] Succeeded, [" << m_tests_failed << "] Failed  -- " << (m_tests_failed > 0 ? "NOT OK" : "OK") << " -- " << endl;

    return m_tests_failed;
}

