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
    int run_id;
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
        getline(s, word, ',');
        emp.run_id = stoi(word);
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

    //print_buffers(cur_size);
}

void export_buffers(int cur_size, int runs_idx) {
    ofstream myfile;

    string str = to_string(runs_idx);

    myfile.open(str);

    for (int i=0; i<cur_size; i++) {
        buffers[i].run_id = runs_idx;

        myfile << buffers[i].eid;
        myfile << ",";
        myfile << buffers[i].ename;
        myfile << ",";
        myfile << buffers[i].age;
        myfile << ",";
        myfile << buffers[i].salary;
        myfile << ",";
        myfile << buffers[i].run_id;
        myfile << "\n";
    }

    myfile.close();
}

void write_to_final(EmpRecord record) {
    ofstream f;
    f.open("EmpSorted.csv", ios::out | ios::app);

    f << record.eid;
    f << ",";
    f << record.ename;
    f << ",";
    f << record.age;
    f << ",";
    f << record.salary;
    f << "\n";

    f.close();
}

int count_lines(int j) {
    ifstream f;
    string s;
    int count=0; 
    f.open(to_string(j));
    while(!f.eof()) {
        getline(f, s);
        count++;	
    }
    return count-1;
}

EmpRecord get_item(int run_file, int j) {
    fstream run;
    EmpRecord temp_record;
    run.open(to_string(run_file), ios::in);

    for (int i=0; i<j; i++) {
        temp_record = grab_emp_record(run);
    }
    //printf("%d record in %d : %d \n", j, run_file, temp_record.eid);
    run.close();
    return temp_record;
}

// Merges your M-1 runs (from disk) using the buffers in main memory and stores
// them in a sorted file called 'EmpSorted.csv'(The Final Output File). You can
// change the return type and arguments as you see fit.
void merge_runs(int cur_size, int sort_size) { 
    int current_file=0;
    int runs_idx[cur_size];
    for (int k=0; k<cur_size; k++)
        runs_idx[k] = 1;

    bool complete=false;

    // add first element of each run
    for (int j=0; j<cur_size; j++) {
        fstream run;
        run.open(to_string(j), ios::in);
        EmpRecord temp_record = grab_emp_record(run);
        buffers[j] = temp_record;
        runs_idx[j]++;
        //printf("%d ", temp_record.eid);
        run.close();
    }

    // get the lowest id and add it to the sorted file
    while (!complete) {
        sort_main(sort_size);
        printf("writing %d ", buffers[0].eid);
        write_to_final(buffers[0]);
        int temp = buffers[0].run_id, x=0;

        // fill the spot with the next item in the run
        if (runs_idx[temp] <= 22) {
            buffers[0] = get_item(temp, runs_idx[temp]); 
            runs_idx[temp]++;
            //printf("in run #%d ", current_file);
        }
        // else, pick the next run with items left
        else {
            bool get=false;
            while (get==false) {
                if (runs_idx[x] <= 22) {
                    buffers[0] = get_item(x, runs_idx[x]); 
                    runs_idx[x]++;
                    get=true;
                }
                x++;
            }        
        }
        int c=0;
        for (int i=0; i<cur_size; i++) {
            if (runs_idx[i]==23) 
                c++;
        }
        if (c==cur_size) {
            complete=true;
            break;
        }
    }
    for (int i=0; i<22; i++) {
        printf("buf[%d]: %d \n", i, buffers[i].eid);
        sort_main(22);
        write_to_final(buffers[i]);
    }
    cout << endl;
    
}

int main() {
    // open file streams to read and write
    fstream input_file;
    input_file.open("Emp.csv", ios::in);
    remove("EmpSorted.csv");

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
            //print_buffers(cur_size); // The main_memory is not filled up but there are some leftover tuples that need to be sorted.
        }
        if (cur_size + 1 <= buffer_size) {
            // Memory is not full. Store the current record into a buffer.
            buffers[cur_size] = single_EmpRecord;
            cur_size += 1;
        } else {
            // Memory is now full. Sort the tuples in Main Memory and store them in a
            // temporary file (runs)
            //cout << "Main memory is full. Time to sort and store sorted blocks in a temporary file" << endl;
            // Print_Buffers(cur_size);
            sort_main(cur_size);
            //print_buffers(cur_size);
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

    merge_runs(runs_idx, 22);
    
    // You can delete the temporary sorted files (runs) after you're done in order
    // to keep things clean and tidy.

    return 0;
}
