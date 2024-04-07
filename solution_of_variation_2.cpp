#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <ctime>
#include <map>
#include <set>

using namespace std;

// Function to check if a file exists
bool fileExists(const string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

// Function to check if a file is empty
bool isFileEmpty(const string& filename) {
    ifstream file(filename);
    return file.peek() == ifstream::traits_type::eof();
}

// Function to check if a file is older than N months
bool isFileOlderThanNMonths(const string& filename, int N) {
    struct stat fileStat;
    if (stat(filename.c_str(), &fileStat) != 0) {
        cerr << "Error getting file status for: " << filename << endl;
        return false;
    }

    time_t currentTime = time(nullptr);
    time_t fileModifiedTime = fileStat.st_mtime;
    double diffSeconds = difftime(currentTime, fileModifiedTime);
    double diffMonths = diffSeconds / (60 * 60 * 24 * 30); // Assuming 30 days per month

    return diffMonths > N;
}

// Function to delete a file
bool deleteFile(const string& filename, const string& reason) {
    if (remove(filename.c_str()) != 0) {
        cerr << "Error deleting file: " << filename << " (" << reason << ")" << endl;
        return false;
    }
    cout << "Deleted file: " << filename << " (" << reason << ")" << endl;
    return true;
}

void cleanFolder(const string& folderPath, const vector<string>& filenames, const vector<int>& numbers) {
    int N, m;
    cout << "Enter value of minimum months" << endl;
    cin >> N;
    cout << "Enter value for minimum no. of access" << endl;
    cin >> m;

    cout<<"Which files do you want to delete?"<<endl;
    cout<<"1. Empty files \n2. Redundant files \n3. Files older than "<<N<< " months \n4. Files not accessed atleast "<<m<<" times \nEnter nummber of operation(s). when you done please enter 0 !"<<endl;

   set<int> choice;
    int k=5;
    while(k!=0){
        cin>>k;
        choice.insert(k);
    }
    
    vector<string> filePaths; 
    for (size_t i = 0; i < filenames.size(); ++i) {
        string filePath = folderPath + "/" + filenames[i];
        if (!fileExists(filePath)) {
            cerr << "File does not exist: " << filePath << endl;
            continue;
        }
        filePaths.push_back(filePath);
    }

    map<string, string> contentMap;
    vector<string> duplicateFiles;

    for (size_t i = 0; i < filePaths.size(); ++i) {
        const string& filePath = filePaths[i];
        const int& accessFreq = numbers[i];

        if (isFileEmpty(filePath) && choice.find(1)!=choice.end()) {
            if (!deleteFile(filePath, "empty file")) {
                cerr << "Failed to delete empty file: " << filePath << endl;
            }
            continue;
        }
 if (isFileOlderThanNMonths(filePath, N) && choice.find(3)!=choice.end()) {
            if (!deleteFile(filePath, "older than " + to_string(N) + " months")) {
                cerr << "Failed to delete file older than " << N << " months: " << filePath << endl;
            }
            continue;
        }

        if (accessFreq < m && choice.find(4)!=choice.end()) {
            if (!deleteFile(filePath, "Access count of the file is less than " + to_string(m))) {
                cerr << "Failed to delete file: " << filePath << endl;
            }
            continue;
        }

       if(choice.find(2)!=choice.end()){
         ifstream file(filePath);
        stringstream buffer;
        buffer << file.rdbuf();
        string fileContent = buffer.str();
  if (contentMap.find(fileContent) == contentMap.end()) {
            contentMap[fileContent] = filePath;
        } else {
            duplicateFiles.push_back(filePath);
        }
    }
    
   }
   for (const string& duplicateFile : duplicateFiles) {
        if (!deleteFile(duplicateFile, "duplicate file")) {
            cerr << "Failed to delete duplicate file: " << duplicateFile << endl;
        }
    }
}

int main() {
    ifstream inputFile("hello[1].txt");
    string folderName, filename;
    int number;
    vector<string> filenames;
    vector<int> numbers;
    vector<string> dates;
    string date;
    

    while (inputFile >> folderName) {
        filenames.clear();
        numbers.clear();
        while (inputFile >> filename >> number >> date) {
            if (filename == "-" ) {
                break;
            }
            filenames.push_back(filename);
            numbers.push_back(number);
            dates.push_back(date);
        }
        cleanFolder(folderName, filenames, numbers);
    }

    return 0;
}