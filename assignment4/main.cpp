/*
Skeleton code for linear hash indexing
*/

#include <string>
#include <ios>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include "classes.h"
using namespace std;


int main(int argc, char* const argv[]) {

    // Create the index
    LinearHashIndex emp_index("EmployeeIndex");
    emp_index.createFromFile("Employee.csv");
    
    // Loop to lookup IDs until user is ready to quit
    while (1==1) {
        printf("Enter ID to search\n");
        printf("Enter 0 or negative number to quit\n");

        int input;

        cin >> input;

            if (input > 0) {
                emp_index.findRecordById(input);
                // retrievedRecord.print();
            } else {
                break;
            }
    }

    return 0;
}
