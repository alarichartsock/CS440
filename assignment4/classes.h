#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <bitset>
using namespace std;

class Record {
public:
    int id, manager_id;
    std::string bio, name;

    Record(vector<std::string> fields) {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }

    void print() {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }

    // Calculate size of record
    int size() {
        return 8 + 8 + bio.length() + name.length() + 4;
    }

    void writeRecord(fstream &indexFile, int location) {
        indexFile.seekp(location);

        int64_t paddedId = (int64_t)id;
        int64_t paddedManagerId = (int64_t)manager_id;

        // cout << "Size of padded ID: " << sizeof(paddedId) << " byte(s)" << endl;
        // cout << "Size of padded manager ID: " << sizeof(paddedManagerId) << " byte(s)" << endl;

        std::string strid = std::to_string(id);

        indexFile << "~";
        indexFile << strid;

        // Cast int members to int64_t to write 8 bytes to index file
        // indexFile.write(reinterpret_cast<const char *>(&paddedId), sizeof(paddedId));
        indexFile << "~";
        indexFile << name;
        indexFile << "~";
        indexFile << bio;
        indexFile << "~";
        indexFile.write(reinterpret_cast<const char *>(&paddedManagerId), sizeof(paddedManagerId));
        indexFile << "~";
    }
};


class LinearHashIndex {

private:
    const int BLOCK_SIZE = 4096;

    vector<int> blockDirectory; // Map the least-significant-bits of h(id) to a bucket location in EmployeeIndex (e.g., the jth bucket)
                                // can scan to correct bucket using j*BLOCK_SIZE as offset (using seek function)
								// can initialize to a size of 256 (assume that we will never have more than 256 regular (i.e., non-overflow) buckets)
    int n;  // The number of indexes in blockDirectory currently being used
    int i;	// The number of least-significant-bits of h(id) to check. Will need to increase i once n > 2^i
    int numRecords = 0;    // Records currently in index. Used to test whether to increase n
    int nextFreeBlock; // Next place to write a bucket. Should increment it by BLOCK_SIZE whenever a bucket is written to EmployeeIndex
    float usedpercentage;
    int capacity = 0;
    int usedcapacity = 0;

    string fName;

    // Get record from input file (convert .csv row to Record data structure)
    Record getRecord(fstream &empin) {

        string line, word;

        // Make vector of strings (fields of record)
        // to pass to constructor of Record struct
        vector<std::string> fields;

        // grab entire line
        if (getline(empin, line, '\n'))
        {
            // turn line into a stream
            stringstream s(line);

            // gets everything in stream up to comma
            getline(s, word, ',');
            fields.push_back(word);
            getline(s, word, ',');
            fields.push_back(word);
            getline(s, word, ',');
            fields.push_back(word);
            getline(s, word, ',');
            fields.push_back(word);

            return Record(fields);
        }
        else
        {
            fields.push_back("-1");
            fields.push_back("-1");
            fields.push_back("-1");
            fields.push_back("-1");
            return Record(fields);

        }

    }

    // Hash IDs
    int hash(int id) {
        return (id % (int)pow(2,8));
    }

    int getLastBits(int hash, int i) {
        return hash & ((1 << i) - 1);
    }

    int initBucket(fstream &indexFile) {
        indexFile.seekp(nextFreeBlock);

        const char* buf = "0";
        capacity = capacity + BLOCK_SIZE;


        for(int i=0; i<(BLOCK_SIZE-10); i++) {
            // Open brackets are used in place of excess space within the index file
            // Reserve 7 characters for overflow pointer
            indexFile << '{';
        }
        
        indexFile << '$';
        indexFile << "00000000";
        indexFile << '$';

        blockDirectory.push_back(nextFreeBlock);

        return nextFreeBlock;
    }

    void writeRecordToIndexFile(Record record, fstream &indexFile, int blockStart, int blockEnd) {
        indexFile.seekp(blockStart);

        char ch;

        int localLocation = 0;
        int recordSize = record.size();
        int overflow;

        while(indexFile >> noskipws >> ch) {
            
            int currentSpot = blockStart + localLocation;
            
            // If we have space then write record to open bucket
            if ((ch == '{') && ((blockEnd - currentSpot - 10) >= recordSize)) {
                record.writeRecord(indexFile,currentSpot);
                usedcapacity = usedcapacity + recordSize;
                break;
            } 
            else if (ch == '$'){

                string overflowptr;
                int intptr;

                getline(indexFile,overflowptr,'$');
                intptr = stoi(overflowptr);

                if (intptr > 0) {
                    printf("Non-zero overflow pointer read, writing to overflow offset\n");
                    writeRecordToIndexFile(record,indexFile,intptr,intptr+BLOCK_SIZE);
                    break;
                } else {
                    printf("Reaching overflow status, at position %i\n", currentSpot);

                    overflow = initBucket(indexFile);
                    nextFreeBlock = nextFreeBlock + BLOCK_SIZE;

                    std::string overflowstr = to_string(overflow);

                    int zeroes = 8 - overflowstr.length();

                    for (int i=0;i<zeroes;i++) {
                        overflowstr.insert(0,"0");
                    }

                    const char * c = overflowstr.data();

                    indexFile.seekp(blockEnd - 10);
                    indexFile.put('$');
                    indexFile.write(reinterpret_cast<const char *>(c), sizeof(c));
                    indexFile.put('$');
                    writeRecordToIndexFile(record,indexFile,overflow,overflow+BLOCK_SIZE);
                    break;
                }
            }
            localLocation++;
        }
    }

    // Insert new record into index
    void insertRecord(Record record, fstream &indexFile) {

        // No records written to index yet
        if (numRecords == 0) {
            // Initialize index with first blocks (start with 4)
            for(int i=0; i<4; i++) {
                initBucket(indexFile);
                nextFreeBlock = nextFreeBlock + BLOCK_SIZE;
                printf("Initializing bucket\n");
            }
            // 2 bits for addressing 4 buckets/blocks
            i = 2;
        }

        int bucketLoc = getLastBits(hash(record.id),i);

        if (bucketLoc >= n) {
            printf("Set bucket index MSB to 0\n");
            bucketLoc &= ~(1 << (i-1));
        }

        writeRecordToIndexFile(record, indexFile, blockDirectory[bucketLoc], blockDirectory[bucketLoc] + BLOCK_SIZE);
        numRecords += 1;

        usedpercentage = (float)usedcapacity / capacity;

        printf(" Inserted record of length %i, into bucket %i capacity is %i / %i = %f full", record.size(), bucketLoc, usedcapacity, capacity, usedpercentage);
        cout << "\t " << record.name << " " << record.id << "\n";

        if (usedpercentage >= .70) {
            printf("    Capacity condition met. Allocating new bucket. \n");

            // Self explanatory. We need a new bucket to be allocated because we've met our capacity condition.
            int newBucket = initBucket(indexFile);
            
            // Num digits that we need to address the new bucket
            int digits = (int)ceil(log2(n));

            // Bucket that we need to rehash
            int bucketToMoveFrom = newBucket;
            bucketToMoveFrom &= ~(1 << (digits - 1));

            // Location of bucket that we need to rehash
            int bucketToMoveFromLoc = blockDirectory[bucketToMoveFrom];

            int newOldBucketLoc = initBucket(indexFile);

            char ch;
            int intptr;
            string overflowptr;

            // indexFile.seekp(bucketToMoveFromLoc);
            indexFile.seekp(0);

            vector<Record> recordvec;

            while(indexFile >> noskipws >> ch) {

                if (ch == '$') {
                    // If we have a $ char then we've reached the end of the block
                    // If it's a valid overflow pointer, then go to it
                    getline(indexFile,overflowptr,'$');
                    intptr = stoi(overflowptr);
                    printf("Overflow Pointer Read, value: %i\n", intptr);

                    if (intptr > 0) {
                        // indexFile.seekp(intptr);
                    } else {
                        break;
                    }
                }
                if (ch != '{'){
                    Record movingrecord = readRecord(indexFile);

                    recordvec.push_back(movingrecord);
                }
            }

            for(auto & element : recordvec) {
                int movingBucketLoc = getLastBits(hash(record.id),digits);

                int newBucketIdx = newBucket / BLOCK_SIZE;

                if (movingBucketLoc == newBucketIdx) {
                    writeRecordToIndexFile(element, indexFile, blockDirectory[newBucketIdx],blockDirectory[newBucketIdx]+BLOCK_SIZE );
                } else {
                    writeRecordToIndexFile(element, indexFile, newOldBucketLoc, newOldBucketLoc + BLOCK_SIZE);
                }
            }

            i = digits;
        }
    }

    Record readRecord(fstream &inputFile) {
        int64_t paddedId;
        int64_t paddedManagerId;
        string name, bio, id;

        getline(inputFile, id, '~');
        getline(inputFile, name, '~');
        getline(inputFile, bio, '~');
        inputFile.read(reinterpret_cast<char *>(&paddedManagerId), sizeof(paddedManagerId));
        inputFile.ignore(1, '~');

        int regularManagerId = (int)paddedManagerId;

        vector<string> fields{id, name, bio, to_string(regularManagerId)};

        return Record(fields);
    }


public:
    LinearHashIndex(string indexFileName) {
        n = 4;
        i = 2;
        numRecords = 0;
        nextFreeBlock = 0;
        fName = indexFileName;
    }

    // Read csv file and add records to the index
    void createFromFile(string csvFName) {
        fstream indexFile(fName, ios::in | ios::out | ios::trunc | ios::binary);
        fstream inputFile(csvFName, ios::in);

        bool remainingRecords = true;

        while (remainingRecords) {
            Record r = getRecord(inputFile);

            if (r.id != -1) {
                insertRecord(r, indexFile);
            } else {
                printf("All records read in\n");
                remainingRecords = false;
            }
        }

        indexFile.close();
        inputFile.close();
        
    }

    // Given an ID, find the relevant record and print it
    void findRecordById(int id) {

        fstream indexFile(fName,ios::in);

        int bucketLoc = getLastBits(hash(id),i);

        if (bucketLoc >= n) {
            printf("Set bucket index MSB to 0\n");
            bucketLoc &= ~(1 << (i-1));
        }

        printf("Searching bucket %i for id %i\n", bucketLoc, id);

        int location = blockDirectory[bucketLoc];

        string overflowptr;
        int intptr;

        char ch;

        indexFile.seekp(location);

        while(indexFile >> noskipws >> ch) {

            if (ch == '$') {
                // If ch == '$' then we've reached the end of the block
                getline(indexFile,overflowptr,'$');
                intptr = stoi(overflowptr);
                printf("Overflow Pointer Read, value: %i\n", intptr);

                if (intptr > 0) {
                    indexFile.seekp(intptr);
                } else {
                    break;
                }
                    

            }
            else if (ch != '{'){
                Record record = readRecord(indexFile);
                if (record.id == id) {
                    record.print();
                }
            }
        }

        indexFile.close();
    }
};