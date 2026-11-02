// IITH KRC MANAGEMENT SYSTEM //
// DONE BY LALITKUMAR ELAIYARAJA - BM25BTECH11011
// DONE BY TARUN V - BM25BTECH11028

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
using namespace std;

struct Book
{
    string id, title, author, category;
    int totalCopies = 0;
    int availableCopies = 0;
};

struct Student
{
    string id, name, department;
};

struct Transaction
{
    int transactionID = 0;
    string studentID, bookID, issueDate, returnDate;
    bool active = true;
};

struct Reservation
{
    string studentID, bookID, requestDate;
};

// HELPING FUNCTIONS // 

string readLine(const string &prompt)
{
    cout << prompt;
    string value;
    getline(cin, value);
    return value;
}

int readInt(const string& prompt)
{
    while (true)
    {
        string s = readLine(prompt);
        stringstream ss(s);
        int value;
        char extra;
        if (ss >> value && !(ss >> extra))
        {
             return value;
        }
        cout << "Enter a valid integer.\n";
    }
}

void pauseScreen()
{
    cout << "\nPress Enter to continue...";
    string dummy;
    getline(cin, dummy);
}

string todayPrompt()
{
    string date = readLine("Date (YYYY-MM-DD, or blank for 2026-10-29): ");
    return date;
}

// LINKED LIST APPLICATION IN TRANSACTION FUNCTIONS //
class TransactionList
{
    struct Node
    {
        Transaction data;
        Node* next;
        Node(const Transaction &t)
        {
            data = t;
            next = nullptr;
        }
    };
    Node* head;

    public:
    TransactionList() // RUNS WHEN OBJECT CREATED -- CONSTRUCTOR 
    {
        head = nullptr;
    }

    ~TransactionList() // RUNS WHEN OBJECT DELETED -- DESTRUCTOR 
    {
        while (head != nullptr)
        {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void add(const Transaction& t)
    {
        Node* node = new Node(t);
        if (!head)
        {
            head = node;
            return;
        }
        Node* current = head;
        while (current->next)
        {
            current = current->next;
        }
        current->next = node;
    }

    bool hasActiveIssue(const string& studentID, const string& bookID)  // TO CHECK IF STUDENT HAS ALREADY BORROWED THIS BOOK
    {
        Node* current = head;
        while (current)
        {
            if (current->data.studentID == studentID && current->data.bookID == bookID && current->data.active)
                return true;
            current = current->next;
        }
        return false;
    }

    bool returnBook(const string& studentID, const string& bookID, const string& date) // CHECKS IF STUDENT RETURNED SOME BOOK
    {
        Node* current = head;

        while (current)
        {
            if (current->data.studentID == studentID && current->data.bookID == bookID && current->data.active)
            {
                current->data.active = false;
                current->data.returnDate = date;
                return true;
            }
            current = current->next;
        }
        return false;
    }

    void displayForStudent(const string& studentID)
    {
        bool found = false;
        Node* current = head;
        cout << "\nTransaction history for " << studentID << ":\n";
        cout << "Txn ID" << "  " << "Book ID"
             << "  " << "Issue date" << "  " << "Return date"
             << "Status\n";
        while (current)
        {
            const Transaction &t = current->data;
            if (t.studentID == studentID)
            {
                found = true;
                cout << t.transactionID << "  " << t.bookID
                     << "  " << t.issueDate << "  " << (t.returnDate.empty() ? "-" : t.returnDate)
                     << (t.active ? "ISSUED" : "RETURNED") << "\n";
            }
            current = current->next;
        }
        if (!found) cout << "No transactions found.\n";
    }

    void displayAll()
    {
        Node* current = head;
        cout << "\nAll transactions:\n";
        cout << "Txn ID" << "  " << "Student"
             << "  " << "Book" << "  " << "Issue date"
             << "  " << "Return date" << "Status\n";
        while (current)
        {
            const Transaction& t = current->data;
            cout << t.transactionID << "  " << t.studentID
                 << "  " << t.bookID << "  " << t.issueDate
                 << "  " << (t.returnDate.empty() ? "-" : t.returnDate)
                 << (t.active ? "ISSUED" : "RETURNED") << "\n";
            current = current->next;
        }
    }
};

// APPLICATION OF QUEUE - IN RESERVATION OF BOOKS FUNCTIONS //

class ReservationQueue {
    vector<Reservation> items;
    public:
    void enqueue(const Reservation& r)
    {
        items.push_back(r);
    }
    bool empty()
    {
        return items.empty();
    }
    Reservation front()
    {
        return items.front();
    }
    Reservation dequeue()
    {
        Reservation r = items.front();
        items.erase(items.begin()); 
        return r;
    }
    bool containsStudent(const string& studentID)
    {
        for (const Reservation& r : items)
        {
            if (r.studentID == studentID)
                return true;
        }
        return false;
    }
    bool removeStudent(const string& studentID)
     {
        for (auto it = items.begin(); it != items.end(); ++it)
        {
            if (it->studentID == studentID)
            {
                items.erase(it);
                return true;
            }
        }
        return false;
    }
    int size() const
    {
        return (int)items.size();
    }
    const vector<Reservation>& all() const
    {
        return items;
    }
};

// APPLICATION OF HASH TABLES -> CONVERTING BOOK ID TO SOME INDEX, THEN STORING FOR EASE OF ACCESS
// SUPPOSE SOME INDEX HAS TWO OR MORE BOOK IDs, THEN WE USED LINKED LIST TO STORE EACH BOOK ID FOR THAT INDEX AND SEARCH THROUGH IT WHENEVER NEEDED
class BookHashTable 
{
    struct Node 
    {
        string key;
        int index;
        Node* next;

        Node(const string& k, int i)
        {
            key = k;
            index = i;
            next = nullptr;
        }
    };

    vector<Node*> table;
    int capacity;

    int hashFunction(const string& key) const 
    {
        long long hash = 0;

        for (unsigned char ch : key)
        {
            hash = (hash * 31 + ch) % capacity; 
        }

        return (int)hash;
    }

    public:

    BookHashTable(int size = 101)
    {
        table = vector<Node*>(size, nullptr);
        capacity = size;
    }

    ~BookHashTable()
    {
        clear();
    }

    void clear()
    {
        for (Node*& head : table)
        {
            while (head)
            {
                Node* temp = head;
                head = head->next;
                delete temp;
            }
        }
    }

    void insert(const string& key, int index)
    {
        int bucket = hashFunction(key);
        Node* current = table[bucket];

        while (current)
        {
            if (current->key == key)
            {
                current->index = index;
                return;
            }

            current = current->next;
        }

        Node* node = new Node(key, index);
        node->next = table[bucket];
        table[bucket] = node;
    }

    int search(const string& key) const
    {
        int bucket = hashFunction(key);
        Node* current = table[bucket];

        while (current)
        {
            if (current->key == key)
                return current->index;

            current = current->next;
        }

        return -1;
    }

    void remove(const string& key)
    {
        int bucket = hashFunction(key);
        Node* current = table[bucket];
        Node* previous = nullptr;

        while (current)
        {
            if (current->key == key)
            {
                if (previous)
                    previous->next = current->next;
                else
                    table[bucket] = current->next;

                delete current;
                return;
            }

            previous = current;
            current = current->next;
        }
    }
};

// MAIN CONTROL CENTER //

class Library 
{
    vector<Book> books;
    map<string, Student> students;
    map<string, ReservationQueue> reservations;   
    TransactionList transactions;
    BookHashTable hashTable;
    int nextTransactionID = 1;

    int findBook(const string& id) // SEARCHES FOR BOOK ID IN BOOKS VECTOR HASH TABLE THEN RETURNS ITS INDEX IN THAT VECTOR
    {
        return hashTable.search(id);
    }

    void rebuildIndexes() // INDEXES WILL HAVE TO BE REBUILT
    {
        hashTable.clear();
        for (int i = 0; i < (int)books.size(); ++i)
        {
            hashTable.insert(books[i].id, i);
        }
    }

    bool studentExists(const string& id)
    {
        return students.find(id) != students.end(); // FIND WILL TRY TO FIND STUDENT ID AND RETURNS AN ITERATOR POINTING
                                                    // BACK TO THAT STUDENT BUT IF NOT FOUND RETURNS students.end()
    }

    void showBook(const Book& b)
    {
        cout << "\nBook ID: " << b.id << "\nTitle: " << b.title
             << "\nAuthor: " << b.author << "\nCategory: " << b.category
             << "\nAvailable copies: " << b.availableCopies << "/" << b.totalCopies << "\n";
    }

    public:
    Library() 
    {
        rebuildIndexes();
    }

    void addBook()
     {
        Book b;
        b.id = readLine("Book ID: ");
        if (b.id.empty() || findBook(b.id) != -1)
        {
            cout << "Invalid or duplicate Book ID.\n"; return;
        }
        b.title = readLine("Title (do not use commas in this simple CSV version): ");
        b.author = readLine("Author: ");
        b.category = readLine("Category: ");
        b.totalCopies = readInt("Total copies: ");
        if (b.totalCopies <= 0)
        {
            cout << "Copies must be positive.\n";
            return;
        }
        b.availableCopies = b.totalCopies;
        books.push_back(b);
        rebuildIndexes();
        cout << "Book added.\n";
    }

    void deleteBook()
    {
        string id = readLine("Book ID to delete: ");
        int index = findBook(id);
        if (index == -1)
        {
            cout << "Book not found.\n"; return;
        }
        if (books[index].availableCopies != books[index].totalCopies)
        {
            cout << "Cannot delete: some copies are currently issued.\n"; return;
        }
        if (reservations.find(id) != reservations.end() && !reservations[id].empty())
        {
            cout << "Cannot delete: reservations are pending.\n"; return;
        }
        books.erase(books.begin() + index);
        rebuildIndexes();
        cout << "Book deleted.\n";
    }

    void updateBook()
    {
        string id = readLine("Book ID to update: ");
        int index = findBook(id);
        if (index == -1)
        {
            cout << "Book not found.\n";
            return;
        }
        Book before = books[index];
        cout << "Enter new details (press Enter to keep existing value).\n";
        string value;
        value = readLine("Title [" + books[index].title + "]: ");
        if (!value.empty()) books[index].title = value;
        value = readLine("Author [" + books[index].author + "]: ");
        if (!value.empty()) books[index].author = value;
        value = readLine("Category [" + books[index].category + "]: ");
        if (!value.empty()) books[index].category = value;
        int newTotal = readInt("New total copies [" + to_string(books[index].totalCopies) + "]: ");
        if (newTotal < books[index].totalCopies - books[index].availableCopies)
        {
            cout << "Total copies cannot be less than copies currently issued. Update cancelled.\n";
            books[index] = before; return;
        }
        books[index].totalCopies = newTotal;
        books[index].availableCopies = newTotal - (before.totalCopies - before.availableCopies);
        rebuildIndexes();
        cout << "Book updated.\n";
    }

    void displayBooks()
    {
        if (books.empty())
        {
            cout << "No books in inventory.\n";
            return;
        }
        cout << "\nBook inventory:\n";
        cout << "Book ID" << "  " << "Title"
             << "  " << "Author" << "  " << "Category"
             << "  " << "Available/Total\n";
        for (int i = 0; i < (int)books.size(); i++)
        {
            cout << books[i].id << "  " << books[i].title
                << "  " << books[i].author << "  " << books[i].category << "  "
                << books[i].availableCopies << "/" << books[i].totalCopies << "\n";
        }
    }

    void searchBook()
    {
        string id = readLine("Book ID: ");
        int index = findBook(id);
        if (index == -1)
            cout << "Book not found.\n";
        else
            showBook(books[index]);
    }

    void registerStudent()
    {
        Student s;
        s.id = readLine("Student ID: ");
        if (s.id.empty() || studentExists(s.id)) { cout << "Invalid or duplicate Student ID.\n"; return; }
        s.name = readLine("Student name: ");
        s.department = readLine("Department: ");
        students[s.id] = s;
        cout << "Student registered.\n";
    }

    void displayStudents()
    {
        if (students.empty())
        {
            cout << "No students registered.\n";
            return;
        }
        cout << "\nStudent registry:\n";
        cout << "Student ID" << "  " << "Name" << "Department\n";
        map<string, Student>::iterator it;

        for (it = students.begin(); it != students.end(); it++)
        {
            cout << it->second.id << "  " << it->second.name
                << "  " << it->second.department << "\n";
        }
    }

    void issueBook() 
    {
        string sid = readLine("Student ID: ");
        string bid = readLine("Book ID: ");
        if (!studentExists(sid))
        {
            cout << "Student not registered.\n";
            return;
        }
        int index = findBook(bid);
        if (index == -1)
        {
            cout << "Book not found.\n";
            return;
        }
        if (transactions.hasActiveIssue(sid, bid))
        {
            cout << "This student already has an active issue for this book.\n"; return;
        }
        if (books[index].availableCopies <= 0)
        {
            cout << "No copy is currently available.\n";
            string answer = readLine("Add a reservation? (y/n): ");
            if (answer == "y" || answer == "Y")
            {
                reserveBook(sid, bid);
            }
            return;
        }
 
        auto qit = reservations.find(bid);
        if (qit != reservations.end() && !qit->second.empty() && qit->second.front().studentID != sid)
        {
            cout << "A reservation queue exists. Please Make a reservation";
            return;
        }
        if (qit != reservations.end() && !qit->second.empty() && qit->second.front().studentID == sid)
        {
            qit->second.dequeue();
        }
        books[index].availableCopies--;
        Transaction t{nextTransactionID++, sid, bid, todayPrompt(), "", true};
        transactions.add(t);
        cout << "Book issued successfully. Transaction ID : " << t.transactionID << "\n";
    }

    void returnBookie() 
    {
        string sid = readLine("Student ID: ");
        string bid = readLine("Book ID: ");
        int index = findBook(bid);
        if (!studentExists(sid))
        {
            cout << "Student not registered.\n";
            return;
        }
        if (index == -1)
        {
            cout << "Book not found.\n";
            return;
        }
        if (!transactions.returnBook(sid, bid, todayPrompt()))
        {
            cout << "No active issue found for this student and book.\n"; return;
        }
        
        books[index].availableCopies++;

        cout << "Book returned successfully.\n";
    }

    void reserveBook() 
    {
        string sid = readLine("Student ID: ");
        string bid = readLine("Book ID: ");
        reserveBook(sid, bid);
    }

    void reserveBook(const string& sid, const string& bid)
    {
        if (!studentExists(sid))
        {
            cout << "Student not registered.\n";
            return;
        }
        if (findBook(bid) == -1)
        {
            cout << "Book not found.\n";
            return;
        }
        if (transactions.hasActiveIssue(sid, bid))
        {
            cout << "You already have this book issued.\n";
            return;
        }
        ReservationQueue &q = reservations[bid];
        if (q.containsStudent(sid))
        {
            cout << "Student already has a reservation in this queue.\n";
            return;
        }
        q.enqueue({sid, bid, todayPrompt()});
        cout << "Reservation added to the queue.\n";
    }

    void showHistory()
    {
        string sid = readLine("Student ID (blank for all transactions): ");

        if (sid.empty())
            transactions.displayAll();
        else
            transactions.displayForStudent(sid);
    }

    void run() 
    {
        while (true) 
        {
            cout << "\n========== IITH KRC BOOK MANAGEMENT SYSTEM ==========\n"
                 << "1. Add book\n"
                 << "2. Delete book\n"
                 << "3. Update book details/copies\n"
                 << "4. Display all books\n"
                 << "5. Search book by ID (hash table)\n"
                 << "6. Register student\n"
                 << "7. Display students\n"
                 << "8. Issue book\n"
                 << "9. Return book\n"
                 << "10. Reserve unavailable book\n"
                 << "11. Show transaction history\n"
                 << "0. Exit\n";
            int choice = readInt("Choice: ");
            switch (choice) 
            {
                case 1: addBook(); break; // DONE
                case 2: deleteBook(); break; // DONE
                case 3: updateBook(); break; // DONE
                case 4: displayBooks(); break; // DONE
                case 5: searchBook(); break; // DONE
                case 6: registerStudent(); break; // DONE
                case 7: displayStudents(); break; // DONE
                case 8: issueBook(); break; // DONE
                case 9: returnBookie(); break; // DONE
                case 10: reserveBook(); break; // DONE
                case 11: showHistory(); break; // DONE
                case 0: cout << "Exiting. All data will be cleared. Goodbye!! \n"; return;
                default: cout << "Invalid choice.\n";
            }
            pauseScreen();
        }
    }
};

int main() {
    Library library;
    library.run();
    return 0;
}