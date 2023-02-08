/* This is skeleton code for the Two-Pass Multi-Way Merge Sort algorithm, you
   can make modifications as long as you meet all question requirements*/
// Currently, this program will just load the records into buffers (main memory)
// and print the records once the buffers are filled. This process will continue
// until all of Emp.csv is read.

#include <bits/stdc++.h>
#include <cstdio>
#include <iostream>
#include <fstream>

using namespace std;

// defines how many buffers are available in the Main Memory
#define buffer_size 22

struct EmpRecord {
    int eid;
    string ename;
    int age;
    double salary;
};
EmpRecord buffers[buffer_size]; // this structure contains 22 buffers; available
                                // as your main memory.

// Grab a single block from the Emp.csv file. In theory, if a block was larger
// than one tuple, this function would return an array of EmpRecords (one block
// worth) and the entire if-then-else statement would be wrapped in a loop that
// would read tuples in until the block size was hit.
EmpRecord grab_emp_record(fstream &empin) {
    string line, word;
    EmpRecord emp;
    // grab entire line
    if (getline(empin, line, '\n')) {
        // turn line into a stream
        stringstream s(line);
        // gets everything in stream up to comma
        getline(s, word, ',');
        emp.eid = stoi(word);
        getline(s, word, ',');
        emp.ename = word;
        getline(s, word, ',');
        emp.age = stoi(word);
        getline(s, word, ',');
        emp.salary = stod(word);
        return emp;
    } else {
        emp.eid = -1;
        return emp;
    }
}

// Print whatever is stored in the buffers of Main Memory
// Can come in handy to see if you've sorted the records in your main memory
// properly.
void print_buffers(int cur_size) {
    for (int i = 0; i < cur_size; i++) {
        cout << i << " " << buffers[i].eid << " " << buffers[i].ename << " " << buffers[i].age << " " << buffers[i].salary << endl;
    } 
}

void sort_main(int cur_size) {
    // for (int i=0; i<cur_size; i++) {
    //   printf("%d \n", i);
    // }

    int i, key, j;

    for (i=1; i<cur_size; i++) {
        key = buffers[i].eid;

        j = i - 1;

        while (j >= 0 && buffers[j].eid > key) {
            std::swap(buffers[j+1],buffers[j]);
            // buffers[j + 1] = buffers[j];
            j = j - 1;
        }
        buffers[j + 1].eid = key;
    }

    print_buffers(cur_size);
}

void export_buffers(int cur_size, int runs_idx) {
    std::ofstream myfile;

    std::string str = std::to_string(runs_idx);

    myfile.open(str);

    for (int i=0; i<cur_size; i++) {
        myfile << buffers[i].eid;
        myfile << ",";
        myfile << buffers[i].ename;
        myfile << ",";
        myfile << buffers[i].age;
        myfile << ",";
        myfile << buffers[i].salary;
        myfile << "\n";
    }

    myfile.close();
}

void write_final(int cur_size) {
    printf("now writing the final output file\n");

    ofstream f;
    f.open("EmpSorted.csv", ios::out | ios::trunc);

    for (int i=0; i<cur_size; i++) {
        f << buffers[i].eid;
        f << ",";
        f << buffers[i].ename;
        f << ",";
        f << buffers[i].age;
        f << ",";
        f << buffers[i].salary;
        f << "\n";
    }

    f.close();
}

// Merges your M-1 runs (from disk) using the buffers in main memory and stores
// them in a sorted file called 'EmpSorted.csv'(The Final Output File). You can
// change the return type and arguments as you see fit.
void merge_runs() { 
    printf("wow we are merging the runs\n");

}

int main() {
    // open file streams to read and write
    fstream input_file;
    input_file.open("Emp.csv", ios::in);

    // flags to check when relations are done being read
    bool flag = true;
    int cur_size = 0;
    int runs_idx = 0;

    /*First Pass: The following loop will read each block, put it into
        main_memory, sort them, and then will put write them to a temporary file (as
        a run) for 2nd pass */
    while (flag) {
        // FOR BLOCK IN RELATION EMP

        // grabs a block
        EmpRecord single_EmpRecord = grab_emp_record(input_file);
        // checks if filestream is empty
        if (single_EmpRecord.eid == -1) {
            flag = false;
            print_buffers(cur_size); // The main_memory is not filled up but there are
                                    // some leftover tuples that need to be sorted.
        }
        if (cur_size + 1 <= buffer_size) {
            // Memory is not full. Store the current record into a buffer.
            buffers[cur_size] = single_EmpRecord;
            cur_size += 1;
        } else {
            // Memory is now full. Sort the tuples in Main Memory and store them in a
            // temporary file (runs)
            cout << "Main memory is full. Time to sort and store sorted blocks in a temporary file"
                << endl;
            // Print_Buffers(cur_size);
            sort_main(cur_size);
            print_buffers(cur_size);
            export_buffers(cur_size,runs_idx);
            runs_idx += 1;

            // After sorting, start again. Clear memory and put the current tuple into
            // main memory.
            cur_size = 0;
            buffers[cur_size] = single_EmpRecord;
            cur_size += 1;
        }
    }
    input_file.close();

    /* Implement 2nd Pass: Read the temporary sorted files (runs) and sort them as
        discussed in class. Write the final output (fully sorted Emp relation) to
        EmpSorted.csv*/

    // Uncomment when you are ready to store the sorted relation
    // fstream sorted_file;
    // sorted_file.open("EmpSorted.csv", ios::out | ios::app);

    bool flag_sorting_done = false;
    while (!flag_sorting_done) {
        merge_runs();
        break;
    }

    // You can delete the temporary sorted files (runs) after you're done in order
    // to keep things clean and tidy.

    return 0;
}
