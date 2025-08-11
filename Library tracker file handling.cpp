#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

// Book data model
class Book {
public:
    int id;
    string title;
    string author;
    int quantity;
    bool isAvailable;
};

// Library Management Class
class Library {
private:
    vector<Book> books;
    const string fileName = "library.csv";

    // Save books to CSV file
    void saveToFile() {
        try {
            ofstream outFile(fileName);
            if (!outFile) throw runtime_error("Error opening file for writing.");

            outFile << "ID,Title,Author,Quantity,IsAvailable\n";
            for (const auto& b : books) {
                outFile << b.id << "," << b.title << "," << b.author << ","
                    << b.quantity << "," << b.isAvailable << "\n";
            }
            outFile.close();
            cout << "Books saved to file successfully!" << endl;
        }
        catch (const exception& ex) {
            cerr << ex.what() << endl;
        }
    }

public:
    // Load books from CSV file
    void loadFromFile() {
        ifstream inFile(fileName);
        if (!inFile) {
            cerr << "Error opening file for reading." << endl;
            return;
        }

        string line;
        getline(inFile, line); // Skip header

        books.clear();
        while (getline(inFile, line)) {
            stringstream ss(line);
            string field;
            Book b;

            getline(ss, field, ',');
            b.id = stoi(field);
            getline(ss, b.title, ',');
            getline(ss, b.author, ',');
            getline(ss, field, ',');
            b.quantity = stoi(field);
            getline(ss, field, ',');
            b.isAvailable = (field == "1");

            books.push_back(b);
        }
        inFile.close();
    }

    // Add a new book
    void addBook() {
        Book b;

        cout << "Enter ID: ";
        cin >> b.id;
        if (cin.fail()) {
            cout << "Invalid input. Please enter a number for ID.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        for (const auto& bk : books) {
            if (bk.id == b.id) {
                cout << "Error: Book with this ID already exists.\n";
                return;
            }
        }

        cin.ignore();
        cout << "Enter Title: ";
        getline(cin, b.title);
        cout << "Enter Author: ";
        getline(cin, b.author);

        cout << "Enter Quantity: ";
        cin >> b.quantity;
        if (cin.fail() || b.quantity < 0) {
            cout << "Error: Quantity must be a positive number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        b.isAvailable = (b.quantity > 0);
        books.push_back(b);
        saveToFile();
        cout << "Book added successfully!\n";
    }

    // Borrow a book
    void borrowBook() {
        int borrowId;
        cout << "Enter book ID to borrow: ";
        cin >> borrowId;

        for (auto& b : books) {
            if (b.id == borrowId) {
                if (b.quantity > 0) {
                    b.quantity--;
                    b.isAvailable = (b.quantity > 0);
                    saveToFile();
                    cout << "You have borrowed \"" << b.title << "\".\n";
                }
                else {
                    cout << "Sorry, this book is currently not available.\n";
                }
                return;
            }
        }
        cout << "Book not found.\n";
    }

    // Return a book
    void returnBook() {
        int returnId;
        cout << "Enter book ID to return: ";
        cin >> returnId;

        for (auto& b : books) {
            if (b.id == returnId) {
                b.quantity++;
                b.isAvailable = true;
                saveToFile();
                cout << "You have returned \"" << b.title << "\".\n";
                return;
            }
        }
        cout << "Book not found.\n";
    }

    // Search for a book
    void searchBook() {
        cin.ignore();
        string search;
        cout << "Enter book ID or Title to search: ";
        getline(cin, search);

        bool found = false;
        for (const auto& b : books) {
            if (to_string(b.id) == search || b.title == search) {
                cout << "Book found:\n";
                cout << "ID: " << b.id << "\nTitle: " << b.title
                    << "\nAuthor: " << b.author
                    << "\nQuantity: " << b.quantity
                    << "\nAvailable: " << (b.isAvailable ? "Yes" : "No") << endl;
                found = true;
                break;
            }
        }
        if (!found) cout << "Book not found.\n";
    }

    // Update book details
    void updateBook() {
        int updateId;
        cout << "Enter book ID to update: ";
        cin >> updateId;
        if (cin.fail()) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        for (auto& b : books) {
            if (b.id == updateId) {
                cout << "\nBook found:\n";
                cout << "ID: " << b.id << "\nTitle: " << b.title
                    << "\nAuthor: " << b.author
                    << "\nQuantity: " << b.quantity
                    << "\nAvailable: " << (b.isAvailable ? "Yes" : "No") << endl;

                bool updated = false;
                while (true) {
                    cout << "\nWhat do you want to update?\n"
                        << "1. Title\n2. Author\n3. Quantity\n4. Cancel\n"
                        << "Enter choice: ";

                    int choice;
                    cin >> choice;
                    cin.ignore();

                    if (choice == 1) {
                        cout << "Enter new Title: ";
                        getline(cin, b.title);
                        updated = true;
                    }
                    else if (choice == 2) {
                        cout << "Enter new Author: ";
                        getline(cin, b.author);
                        updated = true;
                    }
                    else if (choice == 3) {
                        cout << "Enter new Quantity: ";
                        cin >> b.quantity;
                        if (b.quantity >= 0) {
                            b.isAvailable = (b.quantity > 0);
                            updated = true;
                        }
                        else {
                            cout << "Quantity cannot be negative.\n";
                        }
                    }
                    else if (choice == 4) {
                        cout << "Update cancelled.\n";
                        return;
                    }
                    else {
                        cout << "Invalid choice.\n";
                    }
                    break;
                }
                if (updated) {
                    saveToFile();
                    cout << "Book updated successfully!\n";
                }
                return;
            }
        }
        cout << "Book not found.\n";
    }

    // Display all books sorted by ID ascending
    void displayAllBooks() {
        if (books.empty()) {
            cout << "No books in the library.\n";
            return;
        }

        sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
            return a.id < b.id;
            });

        cout << left << setw(5) << "ID" << setw(25) << "Title"
            << setw(20) << "Author" << setw(8) << "Qty"
            << setw(12) << "Available" << endl;
        cout << string(70, '-') << endl;

        for (const auto& b : books) {
            cout << left << setw(5) << b.id
                << setw(25) << b.title
                << setw(20) << b.author
                << setw(8) << b.quantity
                << setw(12) << (b.isAvailable ? "Yes" : "No") << endl;
        }
    }
};

// Menu UI
void welcomeScreenFunc() {
    cout << "=======================================\n";
    cout << "    Welcome to the Library Tracker!\n";
    cout << "=======================================\n";
}

void menu() {
    welcomeScreenFunc();
    cout << "\nLibrary Book Tracker System\n";
    cout << "1. Add Book\n2. Display All Books\n3. Borrow Book\n4. Return Book\n";
    cout << "5. Search Book\n6. Update Book\n7. Exit\n";
    cout << "Choose option: ";
}

// Main
int main() {
    Library lib;
    lib.loadFromFile();

    int choice;
    do {
        menu();
        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = 0;
        }

        switch (choice) {
        case 1: lib.addBook(); break;
        case 2: lib.displayAllBooks(); break;
        case 3: lib.borrowBook(); break;
        case 4: lib.returnBook(); break;
        case 5: lib.searchBook(); break;
        case 6: lib.updateBook(); break;
        case 7: cout << "Goodbye!\n"; break;
        default: cout << "Invalid choice. Try again.\n"; break;
        }

        cout << "\nPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        system("cls"); // Windows clear
    } while (choice != 7);

    return 0;
}
