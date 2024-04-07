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

void cleanFolder(const string& folderPath, const vector<string>& filenames, const vector<int>& numbers) {
    int N, m;
    cout << "Enter value of minimum months" << endl;
    cin >> N;
    cout << "Enter value for minimum no. of access" << endl;
    cin >> m;

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

        if (isFileEmpty(filePath)) {
            if (!deleteFile(filePath, "empty file")) {
                cerr << "Failed to delete empty file: " << filePath << endl;
            }
            continue;
        }
