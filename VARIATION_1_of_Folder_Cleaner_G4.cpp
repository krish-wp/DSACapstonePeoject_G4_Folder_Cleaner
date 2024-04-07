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

using namespace std;

// Function to check if a file exists
bool fileExists(const string& filename) {
    struct stat buffer; 
    return (stat(filename.c_str(), &buffer) == 0);
}

// Function to check if a file is empty
bool isFileEmpty(const string& filename) {
    ifstream file(filename); //open file stream
    return file.peek() == ifstream::traits_type::eof();  //check if file is empty
}

// Function to check if a file is older than N months
bool isFileOlderThanNMonths(const string& filename, int N) {
    struct stat fileStat;
    if (stat(filename.c_str(), &fileStat) != 0) { //check file status using stat function
        cerr << "Error getting file status for: " << filename << endl << endl;
        return false;
    }

    time_t currentTime = time(nullptr); //store current time
    time_t fileModifiedTime = fileStat.st_mtime; //get last modified time
    double diffSeconds = difftime(currentTime, fileModifiedTime);
    double diffMonths = diffSeconds / (60 * 60 * 24 * 30); // Assuming 30 days per month

    return diffMonths > N;
}

// Function to delete a file
bool deleteFile(const string& filename, const string& reason) {
    cout << "Do you want to delete the file: " << filename << "? (Y/N) " << endl; //if yes than input is Y
    string response;
    cin >> response;
    if (response == "y" || response == "Y") {
        if (remove(filename.c_str()) != 0) {
            cerr << "Error deleting file: " << filename << " (" << reason << ")" << endl << endl;
            return false;
        }
        cout << "Deleted file: " << filename << " (" << reason << ")" << endl << endl;
    } else {
        cout << "File: " << filename  << " is not deleted." << endl << endl;
    }
    return true;
}

void cleanFolder(const string& folderPath, const vector<string>& filenames, const vector<int>& numbers) {
    int N, m;
    cout << "Enter value of minimum months" << endl;
    cin >> N; //reads minimume number of months
    cout << "Enter value for minimum no. of access" << endl;
    cin >> m; //reads minimum number of acces


    vector<string> filePaths;
    for (size_t i = 0; i < filenames.size(); ++i) {
        string filePath = folderPath + "/" + filenames[i]; //Concatenate folderpath and filename to get full filepath
        if (!fileExists(filePath)) {
            cerr << "File does not exist: " << filePath << endl << endl;
            continue;
        }
        filePaths.push_back(filePath); //add filepath to vector
    } 

    map<string, string> contentMap; //maping filepath with their content
    vector<string> duplicateFiles; //store filepath of duplicate files

    for (size_t i = 0; i < filePaths.size(); ++i) {
        const string& filePath = filePaths[i];
        const int& accessFreq = numbers[i]; //get access freq from the vector number.

        if (isFileEmpty(filePath)) {
            if (!deleteFile(filePath, "empty file")) {
                cerr << "Failed to delete empty file: " << filePath << endl << endl;
            }
            continue;
        }
 if (isFileOlderThanNMonths(filePath, N)) {
            if (!deleteFile(filePath, "older than " + to_string(N) + " months")) {  //attempt to delete the file and assign a reason to delete the file.
                cerr << "Failed to delete file older than " << N << " months: " << filePath << endl << endl;
            }
            continue;
        }

        if (accessFreq < m) {
            if (!deleteFile(filePath, "Access count of the file is less than " + to_string(m))) {
                cerr << "Failed to delete file: " << filePath << endl << endl;
            }
            continue;
        }

        ifstream file(filePath);
        stringstream buffer; // Create stringstream to store file content
        buffer << file.rdbuf();
        string fileContent = buffer.str();  // Convert stringstream to string to get file content
        
  if (contentMap.find(fileContent) == contentMap.end()) {   // Check if file content already exists
            contentMap[fileContent] = filePath;  // If not found, store file content with filename in map
        } else {
            duplicateFiles.push_back(filePath);  //else add the file to duplicate vector
        }
    }

    for (const string& duplicateFile : duplicateFiles) { //delete all the duplicate files.
        if (!deleteFile(duplicateFile, "duplicate file")) {
            cerr << "Failed to delete duplicate file: " << duplicateFile << endl << endl;
        }
    }
}

int main() {
    ifstream inputFile("hello[1].txt"); //read input file
    string folderName, filename;
    int number;  //variable to store access count for each file
    vector<string> filenames;
    vector<int> numbers;
    vector<string> dates;
    string date;

    while (inputFile >> folderName) {
        filenames.clear();
        numbers.clear();
        while (inputFile >> filename >> number >> date) { // Read filenames, numbers, and dates until end of folder entry
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
