/* This is a skeleton code for Optimized Merge Sort, you can make modifications as 
long as you meet 
   all question requirements*/  
#include <bits/stdc++.h>
#include "record_class.h"
using namespace std;
//defines how many blocks are available in the Main Memory 
#define buffer_size 22
Records buffers[buffer_size]; //use this class object of size 22 as your main memory
/***You can change return type and arguments as you want.***/

//Sorting the buffers in main_memory
void sort_emp_main(int cur_size) {

    int i, key, j;

    for (i=0; i<cur_size; i++) {
        key = buffers[i].emp_record.eid;

        j = i - 1;

        while (j >= 0 && buffers[j].emp_record.eid > key) {
            std::swap(buffers[j+1],buffers[j]);
            j = j - 1;
        }
        buffers[j + 1].emp_record.eid = key;
    }
}

// Exports the sorted buffers to runs on disk
// storing the sorted records into a temporary file (runs) 
void export_emp_buffers(int cur_size, int runs_idx) {
    ofstream myfile;

    std::string emp("emp");

    string str = to_string(runs_idx);

    str.insert(0, emp);

    myfile.open(str);

    for (int i=0; i<cur_size; i++) {
        // buffers[i].run_id = runs_idx;

        myfile << buffers[i].emp_record.eid;
        myfile << ",";
        myfile << buffers[i].emp_record.ename;
        myfile << ",";
        myfile << buffers[i].emp_record.age;
        myfile << ",";
        myfile << buffers[i].emp_record.salary;
        myfile << "\n";
    }

    myfile.close();
}

int Sort_Emp_Runs(fstream &infile) {

    bool flag = true;
    int cur_size = 0;
    int runs_idx = 0;

    while(flag) {
        Records rec = Grab_Emp_Record(infile);

        if (rec.no_values == -1) {
            flag = false;

            sort_emp_main(cur_size);
            export_emp_buffers(cur_size, runs_idx);
            //break
        } else {
            // printEmp(rec);
        }

        if (cur_size + 1 <= buffer_size) {
            buffers[cur_size] = rec;
            cur_size += 1;
        } else {
            sort_emp_main(cur_size);
            export_emp_buffers(cur_size, runs_idx);
            runs_idx += 1;

            // After sorting, start again. Clear memory and put the current tuple into
            // main memory.
            cur_size = 0;
            buffers[cur_size] = rec;
            cur_size += 1;
        }
    }

    return runs_idx + 1;
}

//Sorting the buffers in main_memory
void sort_dep_main(int cur_size) {

    int i, key, j;

    for (i=0; i<cur_size; i++) {
        key = buffers[i].dept_record.managerid;

        j = i - 1;

        while (j >= 0 && buffers[j].dept_record.managerid > key) {
            std::swap(buffers[j+1],buffers[j]);
            j = j - 1;
        }
        buffers[j + 1].dept_record.managerid = key;
    }
}

// Exports the sorted buffers to runs on disk
// storing the sorted records into a temporary file (runs) 
void export_dep_buffers(int cur_size, int runs_idx) {
    ofstream myfile;

    std::string emp("dep");

    string str = to_string(runs_idx);

    str.insert(0, emp);

    myfile.open(str);

    for (int i=0; i<cur_size; i++) {
        // buffers[i].run_id = runs_idx;

        myfile << buffers[i].dept_record.did;
        myfile << ",";
        myfile << buffers[i].dept_record.dname;
        myfile << ",";
        myfile << buffers[i].dept_record.budget;
        myfile << ",";
        myfile << buffers[i].dept_record.managerid;
        myfile << "\n";
    }

    myfile.close();
}

int Sort_Dep_Runs(fstream &infile) {

    bool flag = true;
    int cur_size = 0;
    int runs_idx = 0;

    while(flag) {
        Records rec = Grab_Dept_Record(infile);

        if (rec.no_values == -1) {
            flag = false;

            sort_dep_main(cur_size);
            export_dep_buffers(cur_size, runs_idx);
            //break
        } else {
            // printEmp(rec);
        }

        if (cur_size + 1 <= buffer_size) {
            buffers[cur_size] = rec;
            cur_size += 1;
        } else {
            sort_dep_main(cur_size);
            export_dep_buffers(cur_size, runs_idx);
            runs_idx += 1;

            // After sorting, start again. Clear memory and put the current tuple into
            // main memory.
            cur_size = 0;
            buffers[cur_size] = rec;
            cur_size += 1;
        }
    }

    return runs_idx + 1;
}


//Prints out the attributes from empRecord and deptRecord when a join condition is met 
//and puts it in file Join.csv
void PrintJoin() {
    
    return;
}

//Use main memory to Merge and Join tuples
//which are already sorted in 'runs' of the relations Dept and Emp
void Merge_Join_Runs(int numemp, int numdep) {                 // <--------- note to self: use num_dept_files here!
    using t_FileStreams = ::std::array<::std::fstream, 22>;

    t_FileStreams depfstream;
    t_FileStreams empfstream;

    // Opening filestreams for dept runs
    for(int i=0; i<numdep;i++) {
        std::string dep("dep");
        string str = to_string(i);
        str.insert(0, dep);
        depfstream[i].open(str,ios::in);
    }

    // Opening filestreams for emp runs
    for(int i=0; i<numemp;i++) {
        std::string emp("emp");
        string str = to_string(i);
        str.insert(0, emp);
        empfstream[i].open(str,ios::in);
    }

    // Initialize dep and emp holding arrays
    Records cur_dep[numdep];
    Records cur_emp[numemp];

    // Fill first dep records
    for(int i=0; i<numdep; i++) {
        Records r = Grab_Dept_Record(depfstream[i]);
        cur_dep[i] = r;
    }

    // Fill first dep records
    for(int i=0; i<numemp; i++) {
        Records r = Grab_Dept_Record(empfstream[i]);
        cur_emp[i] = r;
    }

    // open the output file
    ofstream outfile("Join.csv");


    while(true) {
        int min_index = -1;
        int min_did = INT_MAX;
        for(int i = 0; i < numdep; i++){
            if(cur_dep[i].no_values != -1 && cur_dep[i].dept_record.did < min_did){ // if the record is not empty and the eid is smaller than the current smallest eid
                // update the smallest eid and the index of the file that contains the smallest record
                min_did = cur_dep[i].dept_record.did;
                min_index = i;
            }
        }
        if(min_index == -1){
            // if the index is -1, then all the records have been written to the output file
            break;
        }
        int managerid = cur_dep[min_index].dept_record.managerid;
        outfile << cur_dep[min_index].dept_record.did << "," << cur_dep[min_index].dept_record.dname << "," << cur_dep[min_index].dept_record.budget << "," << cur_dep[min_index].dept_record.managerid;
        cout << cur_dep[min_index].dept_record.did << "," << cur_dep[min_index].dept_record.dname << "," << cur_dep[min_index].dept_record.budget << "," << cur_dep[min_index].dept_record.managerid;
        cur_dep[min_index] = Grab_Dept_Record(depfstream[min_index]);

        bool found = false;

        while(found == false) {
            int min_index = -1;
            int min_eid = INT_MAX;
            for(int i = 0; i < numemp; i++){
                if(cur_emp[i].no_values != -1 && cur_emp[i].emp_record.eid < min_eid){ // if the record is not empty and the eid is smaller than the current smallest eid
                    // update the smallest eid and the index of the file that contains the smallest record
                    min_eid = cur_emp[i].emp_record.eid;
                    min_index = i;
                }
            }
            if(min_index == -1){
                // if the index is -1, then all the records have been written to the output file
                break;
            }
            int eid = cur_emp[min_index].emp_record.eid;
            if (eid == managerid) {
                found = true;
                cout << "," << cur_emp[min_index].emp_record.ename << "," << cur_emp[min_index].emp_record.age << "," << cur_emp[min_index].emp_record.salary << endl;
                outfile << "," << cur_emp[min_index].emp_record.ename << "," << cur_emp[min_index].emp_record.age << "," << cur_emp[min_index].emp_record.salary << endl;
            } else {
                cur_emp[min_index] = Grab_Emp_Record(empfstream[min_index]);
            }
        }
    }

    // Closing filestreams for dept runs
    for(int i=0; i<numdep;i++) {
        depfstream[i].close();
    }

    // Closing filestreams for emp runs
    for(int i=0; i<numemp;i++) {
        empfstream[i].close();
    }

    // Opening filestreams for dept runs
    for(int i=0; i<numdep;i++) {
        std::string dep("dep");
        string str = to_string(i);
        str.insert(0, dep);
        remove(str.c_str());
    }

    // Opening filestreams for emp runs
    for(int i=0; i<numemp;i++) {
        std::string emp("emp");
        string str = to_string(i);
        str.insert(0, emp);
        remove(str.c_str());
    }


}



int main() {
    //Open file streams to read and write
    //Opening out two relations Emp.csv and Dept.csv which we want to join
    fstream empin;
    fstream deptin;
    empin.open("Emp.csv", ios::in);
    deptin.open("Dept.csv", ios::in);

    int i = Sort_Emp_Runs(empin);
    int j = Sort_Dep_Runs(deptin);

    Merge_Join_Runs(i,j);

    //Creating the Join.csv file where we will store our joined results
    fstream joinout;
    joinout.open("Join.csv", ios::out | ios::app);
    //1. Create runs for Dept and Emp which are sorted using Sort_Buffer()
    //2. Use Merge_Join_Runs() to Join the runs of Dept and Emp relations 
    //Please delete the temporary files (runs) after you've joined both Emp.csv and Dept.csv
    return 0;
}