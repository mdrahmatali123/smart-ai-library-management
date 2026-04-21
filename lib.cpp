#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <windows.h> // For colors
#include <ctime>
#include <chrono>
#include <map>
#include <cmath>
#include <set>

using namespace std;

// Structure to track search and request analytics
struct BookAnalytics {
    int bookId;
    int searchCount;
    int requestCount;
    int issueCount;
    double popularityScore;
    
    BookAnalytics() : bookId(0), searchCount(0), requestCount(0), issueCount(0), popularityScore(0.0) {}
    BookAnalytics(int id) : bookId(id), searchCount(0), requestCount(0), issueCount(0), popularityScore(0.0) {}
    
    string toString() const {
        stringstream ss;
        ss << bookId << "," << searchCount << "," << requestCount << "," << issueCount << "," << popularityScore;
        return ss.str();
    }
    
    static BookAnalytics fromString(const string& str) {
        stringstream ss(str);
        string token;
        vector<string> parts;
        while (getline(ss, token, ',')) {
            parts.push_back(token);
        }
        if (parts.size() >= 5) {
            BookAnalytics ba;
            ba.bookId = stoi(parts[0]);
            ba.searchCount = stoi(parts[1]);
            ba.requestCount = stoi(parts[2]);
            ba.issueCount = stoi(parts[3]);
            ba.popularityScore = stod(parts[4]);
            return ba;
        }
        return BookAnalytics();
    }
};

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void resetColor() {
    setColor(7);
}

string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    stringstream ss;
    ss << 1900 + ltm->tm_year << "-" << 1 + ltm->tm_mon << "-" << ltm->tm_mday;
    return ss.str();
}

class Student {
public:
    int rollNo;
    string name;
    string email;
    string phone;
    vector<int> searchHistory;
    vector<int> readingPreferences;
    vector<pair<int, string>> issuedBooks;
    vector<pair<int, int>> fines;

    Student(int r = 0, string n = "", string e = "", string p = "")
        : rollNo(r), name(n), email(e), phone(p) {}

    string toString() const {
        stringstream ss;
        ss << rollNo << "," << name << "," << email << "," << phone;
        
        for (int bookId : searchHistory) {
            ss << ";SH:" << bookId;
        }
        
        for (int pref : readingPreferences) {
            ss << ";RP:" << pref;
        }
        
        for (const auto& book : issuedBooks) {
            ss << ";B:" << book.first << ":" << book.second;
        }
        
        for (const auto& fine : fines) {
            ss << ";F:" << fine.first << ":" << fine.second;
        }
        return ss.str();
    }

    static Student fromString(const string& str) {
        vector<string> parts;
        stringstream ss(str);
        string item;
        
        while (getline(ss, item, ',')) {
            parts.push_back(item);
        }
        
        if (parts.size() < 4) return Student();
        
        int rollNo = stoi(parts[0]);
        string name = parts[1];
        string email = parts[2];
        string phone = parts[3];
        Student s(rollNo, name, email, phone);
        
        if (parts.size() <= 4) return s;
        
        string remaining = parts[4];
        for (size_t i = 5; i < parts.size(); i++) {
            remaining += "," + parts[i];
        }
        
        stringstream dataStream(remaining);
        string segment;
        
        while (getline(dataStream, segment, ';')) {
            if (segment.empty()) continue;
            
            if (segment.substr(0, 3) == "SH:") {
                string bookIdStr = segment.substr(3);
                if (!bookIdStr.empty()) {
                    s.searchHistory.push_back(stoi(bookIdStr));
                }
            }
            else if (segment.substr(0, 3) == "RP:") {
                string prefStr = segment.substr(3);
                if (!prefStr.empty()) {
                    s.readingPreferences.push_back(stoi(prefStr));
                }
            }
            else if (segment.substr(0, 2) == "B:") {
                string data = segment.substr(2);
                size_t colonPos = data.find(':');
                if (colonPos != string::npos) {
                    string bookIdStr = data.substr(0, colonPos);
                    string date = data.substr(colonPos + 1);
                    if (!bookIdStr.empty() && date.length() >= 10) {
                        s.issuedBooks.push_back({stoi(bookIdStr), date.substr(0, 10)});
                    }
                }
            }
            else if (segment.substr(0, 2) == "F:") {
                string data = segment.substr(2);
                size_t colonPos = data.find(':');
                if (colonPos != string::npos) {
                    string bookIdStr = data.substr(0, colonPos);
                    string fineStr = data.substr(colonPos + 1);
                    if (!bookIdStr.empty() && !fineStr.empty()) {
                        s.fines.push_back({stoi(bookIdStr), stoi(fineStr)});
                    }
                }
            }
        }
        
        return s;
    }
};

class Book {
public:
    int id;
    string title;
    string author;
    string category;
    bool isIssued;
    int issuedToRollNo;
    string issueDate;
    string dueDate;

    Book(int i = 0, string t = "", string a = "", string c = "", bool issued = false, int rollNo = -1, string iDate = "", string dDate = "")
        : id(i), title(t), author(a), category(c), isIssued(issued), issuedToRollNo(rollNo), issueDate(iDate), dueDate(dDate) {}

    string toString() const {
        stringstream ss;
        ss << id << "," << title << "," << author << "," << category << "," << (isIssued ? 1 : 0);
        if (isIssued) {
            ss << "," << issuedToRollNo << "," << issueDate << "," << dueDate;
        }
        return ss.str();
    }

    static Book fromString(const string& str) {
        stringstream ss(str);
        string token;
        vector<string> parts;
        while (getline(ss, token, ',')) {
            parts.push_back(token);
        }
        if (parts.size() >= 5) {
            int id = stoi(parts[0]);
            string title = parts[1];
            string author = parts[2];
            string category = parts[3];
            bool issued = parts[4] == "1";
            int rollNo = parts.size() > 5 ? stoi(parts[5]) : -1;
            string issueDate = parts.size() > 6 ? parts[6] : "";
            string dueDate = parts.size() > 7 ? parts[7] : "";
            return Book(id, title, author, category, issued, rollNo, issueDate, dueDate);
        }
        return Book();
    }
};

class Library {
private:
    vector<Book> books;
    vector<Student> students;
    map<int, BookAnalytics> analytics;
    string bookFile = "lib.txt";
    string studentFile = "student.txt";
    string adminFile = "admin.txt";
    string analyticsFile = "analytics.txt";

    void saveBooksToFile() {
        ofstream file(bookFile);
        if (file.is_open()) {
            for (const auto& book : books) {
                file << book.toString() << endl;
            }
            file.close();
        }
    }

    void loadBooksFromFile() {
        ifstream file(bookFile);
        if (file.is_open()) {
            string line;
            books.clear();
            while (getline(file, line)) {
                if (!line.empty()) {
                    books.push_back(Book::fromString(line));
                }
            }
            file.close();
        }
    }

    void saveStudentsToFile() {
        ofstream file(studentFile);
        if (file.is_open()) {
            for (const auto& student : students) {
                file << student.toString() << endl;
            }
            file.close();
        }
    }

    void loadStudentsFromFile() {
        ifstream file(studentFile);
        if (file.is_open()) {
            string line;
            students.clear();
            while (getline(file, line)) {
                if (!line.empty()) {
                    students.push_back(Student::fromString(line));
                }
            }
            file.close();
        }
    }
    
    void saveAnalyticsToFile() {
        ofstream file(analyticsFile);
        if (file.is_open()) {
            for (const auto& entry : analytics) {
                file << entry.second.toString() << endl;
            }
            file.close();
        }
    }
    
    void loadAnalyticsFromFile() {
        ifstream file(analyticsFile);
        if (file.is_open()) {
            string line;
            analytics.clear();
            while (getline(file, line)) {
                if (!line.empty()) {
                    BookAnalytics ba = BookAnalytics::fromString(line);
                    analytics[ba.bookId] = ba;
                }
            }
            file.close();
        }
        
        for (const auto& book : books) {
            if (analytics.find(book.id) == analytics.end()) {
                analytics[book.id] = BookAnalytics(book.id);
            }
        }
    }
    
    void calculatePopularityScores() {
        int maxSearch = 1, maxRequest = 1, maxIssue = 1;
        
        for (auto& entry : analytics) {
            maxSearch = max(maxSearch, entry.second.searchCount);
            maxRequest = max(maxRequest, entry.second.requestCount);
            maxIssue = max(maxIssue, entry.second.issueCount);
        }
        
        for (auto& entry : analytics) {
            double searchWeight = (double)entry.second.searchCount / maxSearch;
            double requestWeight = (double)entry.second.requestCount / maxRequest;
            double issueWeight = (double)entry.second.issueCount / maxIssue;
            
            entry.second.popularityScore = (searchWeight * 0.4) + (requestWeight * 0.35) + (issueWeight * 0.25);
        }
    }
    
    vector<int> collaborativeFiltering(int studentRollNo) {
        Student* currentStudent = findStudent(studentRollNo);
        if (!currentStudent || currentStudent->searchHistory.empty()) {
            return vector<int>();
        }
        
        map<int, double> bookScores;
        
        for (const auto& student : students) {
            if (student.rollNo == studentRollNo) continue;
            
            int commonInterests = 0;
            for (int bookId : currentStudent->searchHistory) {
                if (find(student.searchHistory.begin(), student.searchHistory.end(), bookId) != student.searchHistory.end()) {
                    commonInterests++;
                }
            }
            
            if (commonInterests > 0) {
                for (const auto& issued : student.issuedBooks) {
                    if (find(currentStudent->searchHistory.begin(), currentStudent->searchHistory.end(), issued.first) == currentStudent->searchHistory.end()) {
                        bookScores[issued.first] += commonInterests;
                    }
                }
            }
        }
        
        vector<pair<int, double>> sortedScores(bookScores.begin(), bookScores.end());
        sort(sortedScores.begin(), sortedScores.end(), 
             [](const pair<int, double>& a, const pair<int, double>& b) {
                 return a.second > b.second;
             });
        
        vector<int> recommendations;
        for (int i = 0; i < min(5, (int)sortedScores.size()); i++) {
            recommendations.push_back(sortedScores[i].first);
        }
        
        return recommendations;
    }
    
    int calculateFine(const string& dueDate) {
        return 0;
    }

public:
    Library() {
        loadBooksFromFile();
        loadStudentsFromFile();
        loadAnalyticsFromFile();
    }
    
    bool adminLogin() {
        string username, password;
        cout << "Admin Username: ";
        cin >> username;
        cout << "Admin Password: ";
        cin >> password;
        
        ifstream file(adminFile);
        string fileUser, filePass;
        if (file.is_open()) {
            getline(file, fileUser);
            getline(file, filePass);
            file.close();
            return (username == fileUser && password == filePass);
        }
        return false;
    }
    
    void addBook() {
        int id;
        string title, author, category;
        cout << "Enter Book ID: ";
        cin >> id;
        cin.ignore();
        cout << "Enter Title: ";
        getline(cin, title);
        cout << "Enter Author: ";
        getline(cin, author);
        cout << "Enter Category (e.g., Fiction, Science, Math, History): ";
        getline(cin, category);
        
        books.push_back(Book(id, title, author, category));
        analytics[id] = BookAnalytics(id);
        cout << "Book added successfully!" << endl;
        saveBooksToFile();
        saveAnalyticsToFile();
    }
    
    void removeBook(int id) {
        auto it = remove_if(books.begin(), books.end(), [id](const Book& b) { return b.id == id; });
        if (it != books.end()) {
            books.erase(it, books.end());
            analytics.erase(id);
            cout << "Book removed successfully!" << endl;
        } else {
            cout << "Book not found." << endl;
        }
        saveBooksToFile();
        saveAnalyticsToFile();
    }
    
    void addStudent() {
        int rollNo;
        string name, email, phone;
        cout << "Enter Roll Number: ";
        cin >> rollNo;
        cin.ignore();
        cout << "Enter Name: ";
        getline(cin, name);
        cout << "Enter Email: ";
        getline(cin, email);
        cout << "Enter Phone: ";
        getline(cin, phone);
        
        students.push_back(Student(rollNo, name, email, phone));
        cout << "Student added successfully!" << endl;
        saveStudentsToFile();
    }
    
    void removeStudent(int rollNo) {
        auto it = remove_if(students.begin(), students.end(), [rollNo](const Student& s) { return s.rollNo == rollNo; });
        if (it != students.end()) {
            if (!it->issuedBooks.empty()) {
                cout << "Cannot remove student. Please return all books first." << endl;
                return;
            }
            students.erase(it, students.end());
            cout << "Student removed successfully!" << endl;
        } else {
            cout << "Student not found." << endl;
        }
        saveStudentsToFile();
    }
    
    void displayAllStudents() {
        cout << "\n--- Registered Students ---\n";
        if (students.empty()) {
            setColor(12);
            cout << "No students registered." << endl;
            resetColor();
            return;
        }
        for (const auto& student : students) {
            setColor(11);
            cout << "Roll No: " << student.rollNo << ", Name: " << student.name 
                 << ", Email: " << student.email << ", Phone: " << student.phone;
            if (!student.issuedBooks.empty()) {
                cout << " [Has " << student.issuedBooks.size() << " issued book(s)]";
            }
            resetColor();
            cout << endl;
        }
    }
    
    Student* findStudent(int rollNo) {
        for (auto& student : students) {
            if (student.rollNo == rollNo) {
                return &student;
            }
        }
        return nullptr;
    }
    
    void searchBook(const string& query, int studentRollNo = -1) {
        bool found = false;
        
        for (auto& book : books) {
            if (to_string(book.id) == query || book.title.find(query) != string::npos || book.author.find(query) != string::npos) {
                cout << "ID: " << book.id << ", Title: " << book.title << ", Author: " << book.author
                     << ", Category: " << book.category << ", Status: ";
                
                if (analytics.find(book.id) != analytics.end()) {
                    analytics[book.id].searchCount++;
                }
                
                if (book.isIssued) {
                    setColor(12);
                    cout << "Issued to Roll No: " << book.issuedToRollNo;
                } else {
                    setColor(10);
                    cout << "Available";
                }
                resetColor();
                cout << endl;
                found = true;
            }
        }
        
        if (studentRollNo != -1 && found) {
            Student* student = findStudent(studentRollNo);
            if (student) {
                for (const auto& book : books) {
                    if (to_string(book.id) == query || book.title.find(query) != string::npos) {
                        if (find(student->searchHistory.begin(), student->searchHistory.end(), book.id) == student->searchHistory.end()) {
                            student->searchHistory.push_back(book.id);
                            saveStudentsToFile();
                        }
                        break;
                    }
                }
            }
        }
        
        if (!found) {
            setColor(12);
            cout << "No books found matching the query." << endl;
            resetColor();
        }
        
        saveAnalyticsToFile();
    }
    
    void showPopularBookSuggestions() {
        calculatePopularityScores();
        
        vector<pair<int, double>> scoredBooks;
        for (const auto& entry : analytics) {
            for (const auto& book : books) {
                if (book.id == entry.first) {
                    scoredBooks.push_back({book.id, entry.second.popularityScore});
                    break;
                }
            }
        }
        
        sort(scoredBooks.begin(), scoredBooks.end(), 
             [](const pair<int, double>& a, const pair<int, double>& b) {
                 return a.second > b.second;
             });
        
        cout << "\n";
        setColor(14);
        cout << "========================================================\n";
        cout << "      [ML] POPULAR BOOK RECOMMENDATIONS\n";
        cout << "========================================================\n";
        resetColor();
        
        if (scoredBooks.empty()) {
            setColor(12);
            cout << "No data available yet. Start searching and issuing books!" << endl;
            resetColor();
            return;
        }
        
        int rank = 1;
        for (int i = 0; i < min(10, (int)scoredBooks.size()); i++) {
            for (const auto& book : books) {
                if (book.id == scoredBooks[i].first) {
                    setColor(rank <= 3 ? 10 : 11);
                    cout << "#" << rank << " ";
                    if (rank == 1) cout << "[TOP] ";
                    else if (rank == 2) cout << "[2ND] ";
                    else if (rank == 3) cout << "[3RD] ";
                    
                    cout << "\"" << book.title << "\" by " << book.author;
                    cout << " [Score: " << (scoredBooks[i].second * 100) << "%]";
                    
                    if (analytics.find(book.id) != analytics.end()) {
                        cout << "\n     Searched: " << analytics[book.id].searchCount 
                             << "x | Requested: " << analytics[book.id].requestCount 
                             << "x | Issued: " << analytics[book.id].issueCount << "x";
                    }
                    resetColor();
                    cout << endl;
                    rank++;
                    break;
                }
            }
        }
        
        cout << "\n";
        setColor(14);
        cout << "[TIP] Popularity = Search(40%) + Request(35%) + Issue(25%)\n";
        resetColor();
    }
    
    void showTrendingAuthors() {
        calculatePopularityScores();
        
        map<string, double> authorScores;
        
        for (const auto& book : books) {
            if (analytics.find(book.id) != analytics.end()) {
                authorScores[book.author] += analytics[book.id].popularityScore;
            }
        }
        
        vector<pair<string, double>> sortedAuthors(authorScores.begin(), authorScores.end());
        sort(sortedAuthors.begin(), sortedAuthors.end(),
             [](const pair<string, double>& a, const pair<string, double>& b) {
                 return a.second > b.second;
             });
        
        cout << "\n";
        setColor(14);
        cout << "=================================================\n";
        cout << "         TRENDING AUTHORS (ML ANALYSIS)\n";
        cout << "=================================================\n";
        resetColor();
        
        if (sortedAuthors.empty()) {
            setColor(12);
            cout << "No data available yet!" << endl;
            resetColor();
            return;
        }
        
        for (int i = 0; i < min(5, (int)sortedAuthors.size()); i++) {
            setColor(10);
            cout << (i+1) << ". " << sortedAuthors[i].first;
            cout << " [Score: " << (sortedAuthors[i].second * 100) << "%]\n";
            resetColor();
        }
    }
    
    void personalizedRecommendations(int studentRollNo) {
        Student* student = findStudent(studentRollNo);
        if (!student) {
            cout << "Student not found!" << endl;
            return;
        }
        
        cout << "\n";
        setColor(14);
        cout << "=================================================\n";
        cout << "     PERSONALIZED RECOMMENDATIONS FOR " << student->name << "\n";
        cout << "=================================================\n";
        resetColor();
        
        vector<int> recommendations = collaborativeFiltering(studentRollNo);
        
        if (recommendations.empty()) {
            setColor(12);
            cout << "Not enough data for personalized recommendations.\n";
            cout << "Try searching and issuing more books to improve recommendations!\n";
            resetColor();
        } else {
            int count = 0;
            for (int bookId : recommendations) {
                for (const auto& book : books) {
                    if (book.id == bookId && !book.isIssued) {
                        setColor(10);
                        cout << "[BOOK] " << book.title << " by " << book.author;
                        cout << " [Category: " << book.category << "]\n";
                        resetColor();
                        count++;
                        break;
                    }
                }
                if (count >= 5) break;
            }
            if (count == 0) {
                setColor(12);
                cout << "No available books to recommend right now!\n";
                resetColor();
            }
        }
    }
    
    void suggestByAuthor() {
        string author;
        cout << "Enter author name: ";
        cin.ignore();
        getline(cin, author);
        
        bool found = false;
        cout << "\n--- Books by " << author << " ---\n";
        for (const auto& book : books) {
            if (book.author.find(author) != string::npos && !book.isIssued) {
                setColor(10);
                cout << "Available: " << book.title << " (ID: " << book.id << ")" << endl;
                found = true;
                resetColor();
            }
        }
        if (!found) {
            setColor(12);
            cout << "No available books by " << author << " found." << endl;
            resetColor();
        }
    }
    
    void issueBook(int id, int rollNo) {
        Student* student = findStudent(rollNo);
        if (!student) {
            cout << "Student not found! Please register the student first." << endl;
            return;
        }
        
        if (analytics.find(id) != analytics.end()) {
            analytics[id].requestCount++;
        }
        
        for (auto& book : books) {
            if (book.id == id && !book.isIssued) {
                string currentDate = getCurrentDate();
                book.isIssued = true;
                book.issuedToRollNo = rollNo;
                book.issueDate = currentDate;
                book.dueDate = currentDate;
                
                student->issuedBooks.push_back({id, currentDate});
                
                if (analytics.find(id) != analytics.end()) {
                    analytics[id].issueCount++;
                }
                
                cout << "Book issued successfully to " << student->name << " (Roll No: " << rollNo << ")!" << endl;
                cout << "Due Date: 14 days from today" << endl;
                saveBooksToFile();
                saveStudentsToFile();
                saveAnalyticsToFile();
                return;
            }
        }
        cout << "Book not available or not found." << endl;
    }
    
    void returnBook(int id, int rollNo) {
        Student* student = findStudent(rollNo);
        if (!student) {
            cout << "Student not found!" << endl;
            return;
        }
        
        for (auto& book : books) {
            if (book.id == id && book.isIssued && book.issuedToRollNo == rollNo) {
                int fineAmount = calculateFine(book.dueDate);
                
                book.isIssued = false;
                book.issuedToRollNo = -1;
                book.issueDate = "";
                book.dueDate = "";
                
                auto it = remove_if(student->issuedBooks.begin(), student->issuedBooks.end(), 
                    [id](const pair<int, string>& p) { return p.first == id; });
                student->issuedBooks.erase(it, student->issuedBooks.end());
                
                if (fineAmount > 0) {
                    student->fines.push_back({id, fineAmount});
                    cout << "Book returned. Late fine of Rs. " << fineAmount << " added." << endl;
                } else {
                    cout << "Book returned successfully by " << student->name << "!" << endl;
                }
                
                saveBooksToFile();
                saveStudentsToFile();
                return;
            }
        }
        cout << "Book not issued to this student or not found." << endl;
    }
    
    void viewStudentFines(int rollNo) {
        Student* student = findStudent(rollNo);
        if (!student) {
            cout << "Student not found!" << endl;
            return;
        }
        
        if (student->fines.empty()) {
            cout << "No fines for this student." << endl;
        } else {
            setColor(12);
            cout << "--- Fines for " << student->name << " ---" << endl;
            int total = 0;
            for (const auto& fine : student->fines) {
                cout << "Book ID " << fine.first << ": Rs. " << fine.second << endl;
                total += fine.second;
            }
            cout << "Total Fine: Rs. " << total << endl;
            resetColor();
        }
    }
    
    void countIssuedBooks() const {
        int count = 0;
        for (const auto& book : books) {
            if (book.isIssued) count++;
        }
        setColor(11);
        cout << "Issued book count: " << count << endl;
        resetColor();
    }
    
    void displayAll() {
        cout << "\n--- Library Inventory ---\n";
        if (books.empty()) {
            setColor(12);
            cout << "No books in the library." << endl;
            resetColor();
            return;
        }
        for (const auto& book : books) {
            cout << "ID: " << book.id << ", Title: " << book.title << ", Author: " << book.author
                 << ", Category: " << book.category << ", Status: ";
            if (book.isIssued) {
                setColor(12);
                cout << "Issued to Roll No: " << book.issuedToRollNo;
            } else {
                setColor(10);
                cout << "Available";
            }
            resetColor();
            cout << endl;
        }
    }
    
    void showMLDashboard() {
        cout << "\n";
        setColor(14);
        cout << "========================================================\n";
        cout << "|               ML ANALYTICS DASHBOARD                |\n";
        cout << "========================================================\n";
        resetColor();
        
        int totalSearches = 0, totalRequests = 0, totalIssues = 0;
        for (const auto& entry : analytics) {
            totalSearches += entry.second.searchCount;
            totalRequests += entry.second.requestCount;
            totalIssues += entry.second.issueCount;
        }
        
        setColor(11);
        cout << "\n[SYSTEM STATISTICS]\n";
        cout << "   - Total Books: " << books.size() << endl;
        cout << "   - Total Students: " << students.size() << endl;
        cout << "   - Total Searches: " << totalSearches << endl;
        cout << "   - Total Issue Requests: " << totalRequests << endl;
        cout << "   - Total Successful Issues: " << totalIssues << endl;
        cout << "   - Issue Success Rate: " << (totalRequests > 0 ? (totalIssues * 100 / totalRequests) : 0) << "%\n";
        resetColor();
        
        showPopularBookSuggestions();
        showTrendingAuthors();
    }
};

int main() {
    Library lib;
    int choice, adminChoice;
    
    ifstream testAdmin("admin.txt");
    if (!testAdmin.is_open()) {
        ofstream newAdmin("admin.txt");
        newAdmin << "admin\nadmin123";
        newAdmin.close();
    } else {
        testAdmin.close();
    }
    
    cout << "========================================================\n";
    cout << "     AI-POWERED LIBRARY MANAGEMENT SYSTEM\n";
    cout << "========================================================\n";
    cout << "1. Admin Login\n";
    cout << "2. Student Section\n";
    cout << "3. ML Analytics Dashboard\n";
    cout << "Enter choice: ";
    cin >> choice;
    
    if (choice == 1) {
        if (!lib.adminLogin()) {
            setColor(12);
            cout << "Invalid admin credentials!" << endl;
            resetColor();
            cout << "\nPress Enter to exit...";
            cin.ignore();
            cin.get();
            return 0;
        }
        
        do {
            system("cls");
            setColor(14);
            cout << "========================================================\n";
            cout << "                    ADMIN PANEL\n";
            cout << "========================================================\n";
            cout << "1. Add Book\n";
            cout << "2. Remove Book\n";
            cout << "3. Add Student\n";
            cout << "4. Remove Student\n";
            cout << "5. Display All Books\n";
            cout << "6. Display All Students\n";
            cout << "7. Count Issued Books\n";
            cout << "8. View Student Fines\n";
            cout << "9. [ML] Popular Books (Trending)\n";
            cout << "10. [ML] Trending Authors\n";
            cout << "11. [ML] Complete Dashboard\n";
            cout << "12. Exit Admin Panel\n";
            setColor(10);
            cout << "Enter choice: ";
            resetColor();
            cin >> adminChoice;
            cin.ignore();
            
            switch(adminChoice) {
                case 1:
                    lib.addBook();
                    break;
                case 2: {
                    int id;
                    cout << "Enter Book ID: ";
                    cin >> id;
                    lib.removeBook(id);
                    break;
                }
                case 3:
                    lib.addStudent();
                    break;
                case 4: {
                    int rollNo;
                    cout << "Enter Roll Number: ";
                    cin >> rollNo;
                    lib.removeStudent(rollNo);
                    break;
                }
                case 5:
                    lib.displayAll();
                    break;
                case 6:
                    lib.displayAllStudents();
                    break;
                case 7:
                    lib.countIssuedBooks();
                    break;
                case 8: {
                    int rollNo;
                    cout << "Enter Roll Number: ";
                    cin >> rollNo;
                    lib.viewStudentFines(rollNo);
                    break;
                }
                case 9:
                    lib.showPopularBookSuggestions();
                    break;
                case 10:
                    lib.showTrendingAuthors();
                    break;
                case 11:
                    lib.showMLDashboard();
                    break;
                case 12:
                    cout << "Exiting Admin Panel..." << endl;
                    break;
                default:
                    cout << "Invalid choice!" << endl;
            }
            
            if (adminChoice != 12) {
                cout << "\nPress Enter to continue...";
                cin.get();
            }
        } while (adminChoice != 12);
    } 
    else if (choice == 2) {
        int rollNo;
        cout << "Enter your Roll Number: ";
        cin >> rollNo;
        
        int studentChoice;
        do {
            system("cls");
            setColor(11);
            cout << "========================================================\n";
            cout << "                   STUDENT PANEL\n";
            cout << "========================================================\n";
            cout << "1. Search Book\n";
            cout << "2. Issue Book\n";
            cout << "3. Return Book\n";
            cout << "4. View My Fines\n";
            cout << "5. Search by Author\n";
            cout << "6. View All Available Books\n";
            cout << "7. [ML] Popular Books\n";
            cout << "8. [ML] Personalized Recommendations\n";
            cout << "9. Exit\n";
            setColor(10);
            cout << "Enter choice: ";
            resetColor();
            cin >> studentChoice;
            cin.ignore();
            
            switch(studentChoice) {
                case 1: {
                    string query;
                    cout << "Enter search query (ID, title, or author): ";
                    getline(cin, query);
                    lib.searchBook(query, rollNo);
                    break;
                }
                case 2: {
                    int id;
                    cout << "Enter Book ID to issue: ";
                    cin >> id;
                    lib.issueBook(id, rollNo);
                    break;
                }
                case 3: {
                    int id;
                    cout << "Enter Book ID to return: ";
                    cin >> id;
                    lib.returnBook(id, rollNo);
                    break;
                }
                case 4:
                    lib.viewStudentFines(rollNo);
                    break;
                case 5:
                    lib.suggestByAuthor();
                    break;
                case 6:
                    lib.displayAll();
                    break;
                case 7:
                    lib.showPopularBookSuggestions();
                    break;
                case 8:
                    lib.personalizedRecommendations(rollNo);
                    break;
                case 9:
                    cout << "Exiting..." << endl;
                    break;
                default:
                    cout << "Invalid choice!" << endl;
            }
            
            if (studentChoice != 9) {
                cout << "\nPress Enter to continue...";
                cin.get();
            }
        } while (studentChoice != 9);
    }
    else if (choice == 3) {
        lib.showMLDashboard();
        cout << "\nPress Enter to exit...";
        cin.ignore();
        cin.get();
    }
    
    return 0;
}