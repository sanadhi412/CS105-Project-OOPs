#ifndef File_Z
#define File_Z

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <ctime>
#include <mutex>
#include <sstream>
#include <functional>
#include <conio.h>
#include <limits>

using namespace std;

namespace TwoCli {

    // Base class for records (renamed for clarity)
    class stbase {
    public:
        virtual ~stbase() = default; // Default destructor for proper cleanup
        virtual string toString() const = 0; // Pure virtual function for string conversion

        // Get current timestamp
        static string getCurrentTimeMark() {
            auto nowTime = chrono::system_clock::now();
            time_t currentTime = chrono::system_clock::to_time_t(nowTime);

            char buffer[26]; // Need 26 chars for proper date/time format

#ifdef _WIN32
            ctime_s(buffer, sizeof(buffer), &currentTime);
#else
            ctime_r(&currentTime, buffer);
#endif

            string timestamp(buffer);
            if (!timestamp.empty() && timestamp[timestamp.length() - 1] == '\n') {
                timestamp.pop_back(); // Remove newline character
            }
            return timestamp;
        }
    };

    // LiveMonitor class - standalone monitoring functionality
    class LiveMonitor {
    private:
        static void clearScreen() {
#ifdef _WIN32
            system("cls");
#else
            system("clear");
#endif
        }

    public:
        // Generic monitor function that works with any data source and display function
        template<typename DataType>
        static void startMonitor(
            function<vector<DataType>()> dataProvider,              // Function to get data
            function<void(const vector<DataType>&)> displayFunc,    // Function to display data
            function<bool(const vector<DataType>&, const vector<DataType>&)> hasChangesFunc, // Change detection
            int refreshInterval = 3
        ) {
            clearScreen();
            cout << "Starting live monitoring. Screen will refresh every "
                << refreshInterval << " seconds when changes detected." << endl;
            cout << "Press Enter to return to menu." << endl;

            vector<DataType> lastData;
            string lastDisplayTimestamp;
            bool needsRefresh = true;  // Force initial display
            bool running = true;

            // Ensure input buffer is clear
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            while (running) {
                // Get current data
                vector<DataType> currentData = dataProvider();
                string currentTimestamp = stbase::getCurrentTimeMark();

                // Check for changes
                bool hasChanges = needsRefresh || hasChangesFunc(currentData, lastData);

                if (hasChanges) {
                    clearScreen();
                    cout << "===== LIVE MONITOR =====" << endl;
                    cout << "Refreshing every " << refreshInterval << " seconds when changes detected" << endl;
                    cout << "Last updated: " << currentTimestamp << endl;
                    cout << "(Press Enter to return to menu)" << endl << endl;

                    // Display notifications about changes if needed
                    if (lastData.size() < currentData.size() && !lastData.empty()) {
                        cout << "*** NEW DATA RECEIVED! ***" << endl << endl;
                    }

                    // Call display function
                    displayFunc(currentData);

                    // Update last known state
                    lastData = currentData;
                    lastDisplayTimestamp = currentTimestamp;
                    needsRefresh = false;
                }

                // More efficient polling with longer sleep intervals
                auto startTime = chrono::steady_clock::now();
                while (chrono::duration_cast<chrono::seconds>(
                    chrono::steady_clock::now() - startTime).count() < refreshInterval) {
                    if (_kbhit()) {
                        char ch = _getch();
                        if (ch == '\r' || ch == '\n') {
                            running = false;
                            break;
                        }
                    }
                    // Sleep between checks
                    this_thread::sleep_for(chrono::milliseconds(250));
                }
            }

            cout << "Exiting monitor mode..." << endl;
            this_thread::sleep_for(chrono::milliseconds(500));
        }

        // Simple count-based change detector
        template<typename T>
        static bool countChangeDetector(const vector<T>& current, const vector<T>& previous) {
            return current.size() != previous.size();
        }
    };

    // Forward declaration
    template<typename R>
    class FileManager;

    // File manager implementation
    template<typename R>
    class FileManager {
    private:
        string filename;
        mutex file_mutex;

    public:
        FileManager(const string& file) : filename(file) {
            // Create empty file if it doesn't exist
            ifstream checkFile(filename);
            if (!checkFile.good()) {
                ofstream newFile(filename);
                newFile.close();
            }
        }

        // Read all records from file
        vector<R> readRecords() {
            lock_guard<mutex> lock(file_mutex);
            vector<R> records;

            ifstream file(filename);
            if (file.is_open()) {
                string line;
                while (getline(file, line)) {
                    if (!line.empty()) {
                        try {
                            records.push_back(R::fromString(line));
                        }
                        catch (...) {
                            cerr << "Warning: Skipped invalid line in file" << endl;
                        }
                    }
                }
                file.close();
            }

            return records;
        }

        // Write records to file
        bool writeRecords(const vector<R>& records) {
            lock_guard<mutex> lock(file_mutex);

            ofstream file(filename);
            if (file.is_open()) {
                for (const auto& record : records) {
                    file << record.toString() << endl;
                }
                file.close();
                return true;
            }
            else {
                cerr << "Error: Could not open file for writing: " << filename << endl;
                return false;
            }
        }

        // Add a single record
        bool addRecord(const R& record) {
            auto records = readRecords();
            records.push_back(record);
            return writeRecords(records);
        }

        // Get next available ID for new records
        int getNextId() {
            auto records = readRecords();
            if (records.empty()) {
                return 1;
            }

            int maxId = 0;
            for (const auto& record : records) {
                if (record.id > maxId) {
                    maxId = record.id;
                }
            }
            return maxId + 1;
        }

        // Basic display function for records
        static void basicDisplayFunction(const vector<R>& records) {
            for (const auto& record : records) {
                cout << record.toString() << endl;
            }
        }

        // Watch the file with live updates
        void watch(int refreshInterval = 3) {
            // Create a data provider lambda that returns records from this file
            auto dataProvider = [this]() {
                return this->readRecords();
                };

            // Start monitoring with default display and change detection functions
            LiveMonitor::startMonitor<R>(
                dataProvider,
                basicDisplayFunction,
                LiveMonitor::countChangeDetector<R>,
                refreshInterval
            );
        }
    };

} // namespace TwoCli

#endif // File_Z