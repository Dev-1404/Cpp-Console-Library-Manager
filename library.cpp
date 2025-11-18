#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<sstream>
#include<map>
#include<limits> 
using namespace std;


class BookRepository;
class UserRepository;
class TransactionManager;

class Book{
private:
    string bookId;
    string title;
    string author;
    int quantity;

public:
    
    Book(const string& id = "",
         const string& t = "",
         const string& a ="",
         int q = 0) : bookId(id),title(t),author(a),quantity(q){}


    string getBookId() const{
        return bookId;
    }

    string getTitle() const{
        return title;
    }

    string getAuthor() const{
        return author;
    }

    int getQuantity() const{
        return quantity;
    }

    void decQuantity(){
        if(quantity>0)
            quantity--;
    }

    void incQuantity(){
        quantity++;
    }

    void setQuantity(int q) {
        quantity = q;
    }

    void display() const{
        cout<<"ID : "<<bookId<<", Title : "<<title<<", Author : "<<author<<", Quantity : "<<quantity<<endl;
    }

    string toString() const{
        stringstream ss;
        ss<<bookId<<","<<title<<","<<author<<","<<quantity;
        return ss.str();
    }

    static Book fromString(const string& line){
        stringstream ss(line);
        string id,t,a,qstr;
        getline(ss,id,',');
        getline(ss,t,',');
        getline(ss,a,',');
        getline(ss,qstr);
        return Book(id,t,a,stoi(qstr));     }
};


class User{
protected:
    string userId;
    string pwd;
    string name;

public:
    User(const string& id, const string& pd, const string& n):userId(id),pwd(pd),name(n){}

    virtual void displayMenu() const = 0 ;
    virtual void handleChoice(int choice, BookRepository& bookRepo, TransactionManager& transMngr) = 0;

    
    string getUserId() const{
        return userId;
    }
    string getName() const{
        return name;
    }
    string getPwd() const{
        return pwd;
    }
    virtual ~User() ;
};

User::~User() {}

class Librarian : public User{
private:
    int workingHours;
    float salary;

public:
 
    Librarian(const string& id, const string& pd,const string& n, int hrs, float sal):
    User(id,pd,n),workingHours(hrs),salary(sal){}

    void displayMenu() const override{
        cout<<"\nLibrarian Menu:\n"<<"1. Add Book\n"
                            <<"2. Delete Book\n"
                            <<"3. View All Books\n"
                            <<"4. View Customers\n"
                            <<"5. View Issued Log\n"
                            <<"6. Logout\n"
                            <<"Enter your choice : \n"; 
    }

    void handleChoice(int choice, BookRepository& bookRepo, TransactionManager& transMngr) override;


    string toString() const{
        stringstream ss;
        ss<<userId<<","<<pwd<<","<<name<<","<<workingHours<<","<<salary;
        return ss.str();
    }

    static Librarian fromString(const string& line){
        stringstream ss(line);
        string id,pass,n,hrstr,salstr;
        getline(ss,id,',');
        getline(ss,pass,',');
        getline(ss,n,',');
        getline(ss,hrstr,',');
        getline(ss,salstr); 

        return Librarian(id,pass,n,stoi(hrstr),stof(salstr));
    }
};


class Customer : public User{
private:
    string email;
public:
    
    Customer(const string& id, const string& pd, const string& n,const string& e):
    User(id,pd,n),email(e){}

    
    void displayMenu() const override{
        cout<<"\nCustomer Menu:\n"<<"1. View Books\n"
                            <<"2. Issue Book\n"
                            <<"3. Return Book\n"
                            <<"4. Logout\n"
                            <<"Enter your choice : \n"; 
    }

    
    void handleChoice(int choice, BookRepository& bookRepo, TransactionManager& transMngr) override;

  
    string getEmail() const{
        return email;
    }

   
    string toString() const{
        stringstream ss;
        ss<<userId<<","<<pwd<<","<<name<<","<<email;
        return ss.str();
    }


    static Customer fromString(const string& line){
        stringstream ss(line);
        string id,pd,n,e;
        getline(ss,id,',');
        getline(ss,pd,',');
        getline(ss,n,',');
        getline(ss,e); 
        return Customer(id,pd,n,e);
    }
};



class BookRepository{
private:
    map<string,Book> bks; 
    string fname = "books.txt"; 

    
    void loadBooks(){
        ifstream file(fname);
        if(!file.is_open()){
            cerr<<"Warning! 'books.txt' not found. Starting with empty book list."<<endl;
            return;
        }
        string line;

        while(getline(file,line)){
            Book b = Book::fromString(line);
            bks[b.getBookId()] = b;
        }

        file.close();
    }

   
    void saveBooks() const{
        ofstream file(fname);
        if(!file.is_open()){
            cerr<<"Error: Could not open 'books.txt' for writing "<<endl;
            return;
        }
        for(const auto& pair : bks){
            file<<pair.second.toString()<<endl;
        }
        file.close();
    }

public:
    
    BookRepository(){
        loadBooks();
    }

   
    void addBook(const Book& newBook){
        if(bks.count(newBook.getBookId())){
            cout<<"Error : Book with Id "<<newBook.getBookId()<<" already exists.\n";
            return;
        }

        bks[newBook.getBookId()] = newBook;
        saveBooks(); 
        cout<<"Book "<<newBook.getTitle()<<" added successfully\n";
    }

   
    bool deleteBook(const string& bid){
        if(bks.erase(bid)){ 
            saveBooks(); 
            cout<<"Book with Id "<<bid<<" deleted successfully.\n";
            return true;
        }
        cout<<"Error: Book with Id "<<bid<<" not found\n";
        return false;
    }

 
    Book* findBook(const string& bid){
        auto it = bks.find(bid);
        if(it != bks.end()){
            return &(it->second); 
        }
        return nullptr;
    }

    
    void displayAllBooks() const{
        if(bks.empty()){
            cout<<"No books available in the library.\n";
            return;
        }
        cout<<"\n===== Available Books =====\n";
        for(const auto& pair : bks){
            pair.second.display();
        }
        cout<<"===============================\n";
    }

   
    void updateBkQty(const string& bid, int delta){
        Book* bk = findBook(bid);
        if(bk){
            if(delta<0){ 
                if(bk->getQuantity() >= -delta){ 
                    bk->decQuantity(); 
                }
                else{
                    cout<<"Error : Not enough quantity for Book Id "<<bid<<endl;
                    return;
                }
            }
            else{ 
                bk->incQuantity(); 
            }
            saveBooks(); 
        }
        else{
            cout<<"Error : Book with Id "<<bid<<" not found for quantity update.\n";
        }
    }
};


class UserRepository{
private:
    map<string,Librarian> librarians; 
    map<string,Customer> customers;   

    const string libFName = "librarians.txt"; 
    const string custFName = "customers.txt"; 

    
    void loadLibrarians(){
        ifstream file(libFName);
        if(!file.is_open()){
            cerr<<"Warning : 'librarians.txt' not found. Starting with no librarians."<<endl;
            return;
        }
        string line;
        while(getline(file,line)){
            Librarian l = Librarian::fromString(line);
            librarians[l.getUserId()] = l;
        }
        file.close();
    }

   
    void loadCustomers(){
        ifstream file(custFName);
        if(!file.is_open()){
            cerr<<"Warning : 'customers.txt' not found. Starting with no customers."<<endl;
            return;
        }
        string line;
        while(getline(file,line)){
            Customer c = Customer::fromString(line);
            customers[c.getUserId()] = c;
        }
        file.close();
    }


    void saveLibrarians() const{
        ofstream file(libFName);
        if(!file.is_open()){
            cerr<<"Error: Could not open 'librarians.txt' for writing"<<endl;
            return;
        }
        for(const auto& pair : librarians){
            file<<pair.second.toString()<<endl;
        }
        file.close();
    }

  
    void saveCustomers() const{
        ofstream file(custFName);
        if(!file.is_open()){
            cerr<<"Error: Could not open 'customers.txt' for writing"<<endl;
            return;
        }
        for(const auto& pair : customers){
            file<<pair.second.toString()<<endl;
        }
        file.close();
    }

public:
  
    UserRepository(){
        loadLibrarians();
        loadCustomers();
    }

    
    void registerLib(const Librarian& newLib){
        if(librarians.count(newLib.getUserId())){
            cout<<"Error: Librarian with ID "<<newLib.getUserId()<<" already exists \n";
            return;
        }

        librarians[newLib.getUserId()] = newLib;
        saveLibrarians();
        cout<<"Librarian "<<newLib.getName()<<" signed up successfully!\n";
    }

    
    void registerCust(const Customer& newCust){
        if(customers.count(newCust.getUserId())){
            cout<<"Error: Customer with ID "<<newCust.getUserId()<<" already exists \n";
            return;
        }

        customers[newCust.getUserId()] = newCust;
        saveCustomers();
        cout<<"Customer "<<newCust.getName()<<" signed up successfully!\n";
    }

   
    User* authenticate(int role, const string& id, const string& pd){
        if(role == 1){ 

            auto it = librarians.find(id);
            if(it != librarians.end() && it->second.getPwd() == pd) {
                cout<<"Librarian Login successful\n";
                return &(it->second);
            }
        }
        else if(role == 2){ 

            auto it = customers.find(id);
            if(it != customers.end() && it->second.getPwd() == pd) {
                cout<<"Customer Login successful\n";
                return &(it->second);
            }
        }
        cout<<"Invalid Credentials or role\n";
        return nullptr;
    }


    void displayAllCust() const{
        if(customers.empty()){
            cout<<"No customers registered.\n";
            return;
        }
        cout<<"===== Customers Registered =====\n"; 
        for(const auto& pair : customers){
            cout<<"Id : "<<pair.second.getUserId()<<", Name : "<<pair.second.getName()
                <<", Email : "<<pair.second.getEmail()<<endl;
        }
        cout<<"===========================================\n";
    }
};



class TransactionManager{
private:
    const string logFName = "issued_books.txt"; 

    
    void logTransaction(const string& uid, const string& bid, const string& type){

        ofstream file(logFName,ios::app); 

        if(!file.is_open()){
            cerr<<"Error: Could not open 'issued_books.txt' for logging."<<endl;
            return;
        }
        file<<uid<<","<<type<<","<<bid<<endl;
        file.close();
    }

public:
    
    void issueBook(const string& uid, const string& bid, BookRepository& bookRepo){

        Book* bk = bookRepo.findBook(bid);

        if(!bk){
            cout<<"Error : Book with Id "<<bid<<" not found\n";
            return;
        }

        if(bk->getQuantity()>0){
            bookRepo.updateBkQty(bid, -1); 
            logTransaction(uid,bid,"Issued");
            cout<<"Book "<<bk->getTitle()<<" issued to "<<uid<<" successfully.\n";
        }else{
            cout<<"Book "<<bk->getTitle()<<" is currently out of stock.\n";
        }
    }

  
    void returnBook(const string& uid, const string& bid, BookRepository& bookRepo){

        Book* bk = bookRepo.findBook(bid);

        if(!bk){
            cout<<"Error : Book with Id "<<bid<<" not found\n";
            return;
        }

        bookRepo.updateBkQty(bid,1);
        logTransaction(uid,bid,"Returned");
        cout<<"Book "<<bk->getTitle()<<" returned by "<<uid<<" successfully.\n"; 
    }


    void viewIssuedLog() const{
        ifstream file(logFName);
        if(!file.is_open()){
            cout<<"No issued books log available\n";
            return;
        }
        string line;
        cout<<"===== Issued Book Log =====\n";
        while(getline(file,line)){
            cout<<line<<endl;
        }
        cout<<"==============================\n";
        file.close();
    }
};


void Librarian::handleChoice(int choice, BookRepository& bookRepo,TransactionManager& transMngr){

    string bkidip,titleip,authorip;
    int qtyip;

    switch(choice){
    case 1: {
            cout<<"Enter Book Id : ";
            cin>>bkidip;
            cout<<"Enter Book Title : ";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(cin,titleip);
            cout<<"Enter Author : ";
            getline(cin,authorip);
            cout<<"Enter Quantity : ";
            cin>>qtyip;
            Book newBook(bkidip,titleip,authorip,qtyip);
            bookRepo.addBook(newBook);
        break;
    }

    case 2: 
            cout<<"Enter the Book Id to delete : ";
            cin>>bkidip;
            bookRepo.deleteBook(bkidip);
        break;

    case 3: 
            bookRepo.displayAllBooks();
        break;

    case 4: 
            cout << "Access to customer list needs UserRepository. This is managed by the main application logic.\n";
        break;

    case 5: 
            transMngr.viewIssuedLog();
        break;

    case 6:
        break;

    default:
            cout<<"Invalid Choice. Please try again\n";
        break;
    }
}


void Customer :: handleChoice(int choice, BookRepository& bookRepo,TransactionManager& transMngr){

    string bkidip;

    switch(choice){
        case 1: 
                bookRepo.displayAllBooks();
            break;

        case 2: 
                cout<<"Enter Book Id to issue : ";
                cin>>bkidip;
                transMngr.issueBook(this->getUserId(),bkidip,bookRepo);
            break;

        case 3: 
                cout<<"Enter Book Id to return : ";
                cin>>bkidip;
                transMngr.returnBook(this->getUserId(),bkidip,bookRepo);
            break;

        case 4: 
               break;

        default:
                cout<<"Invalid choice. Try again\n";
            break;
    }
}


class LibraryApplication{
private:
    BookRepository bookRepo;
    UserRepository userRepo;
    TransactionManager transMngr;
    User* currUser = nullptr; // Pointer to the currently logged-in user

    // Handles user sign-up process
    void handleSignUp(){
        int role;
        cout<<"Sign Up as :\n1. Librarian\n2. Customer\nEnter choice : ";
        cin>>role;
        string id,pwd,name,email;
        cout<<"Enter Id: ";
        cin>>id;
        // Clearing the input buffer after cin>>id to prevent getline from reading leftover newline.
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout<<"Enter Name: ";
        getline(cin,name);
        cout<<"Enter Password: ";
        cin>>pwd;
        // Clearing the input buffer after cin>>pwd. This is crucial if 'email'
        // input for Customer follows, as its getline would otherwise read this newline.
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if(role == 1){ // Librarian sign-up
            int hrs;
            float sal;
            cout<<"Enter working hours : ";
            cin>>hrs;
            cout<<"Enter salary: ";
            cin>>sal;
            Librarian newLib(id,pwd,name,hrs,sal);
            userRepo.registerLib(newLib);
        }
        else if(role == 2){ // Customer sign-up
            cout<<"Enter Email: ";
            getline(cin,email);
            Customer newCust(id,pwd,name,email);
            userRepo.registerCust(newCust);
        }
        else{
            cout<<"Invalid role choice\n";
        }
    }

    // Handles user login process
    void handleLogin(){
        int role;
        cout<<"Login as:\n1. Librarian\n2. Customer\nEnter choice : ";
        cin>>role;
        // Clearing the input buffer after cin>>role.
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string id,pwd;
        cout<<"Enter Id: ";
        cin>>id;
        //Clearing the input buffer after cin>>id.
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout<<"Enter Password: ";
        cin>>pwd;
        //Clearing the input buffer after cin>>pwd.
        cin.ignore(numeric_limits<streamsize>::max(), '\n');


        currUser = userRepo.authenticate(role,id,pwd); // Attempt to authenticate user

        if(currUser){ // If authentication is successful
            int userChoice;
            do{
                currUser->displayMenu(); // Display user-specific menu
                cout << "Enter your choice: "; // Explicit prompt for user choice
                cin>>userChoice;
                // Clearing the input buffer after cin>>userChoice.
                // This is important before any potential getline calls within handleChoice methods.
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                // Dynamic cast to determine user type and call appropriate handler
                if(Librarian* lib = dynamic_cast<Librarian*>(currUser)){
                    if(userChoice == 4){
                        // Specific handling for Librarian's "View Customers" as it needs UserRepository
                        userRepo.displayAllCust();
                    }
                    else if(userChoice!=6){ // If not logout
                        lib->handleChoice(userChoice,bookRepo,transMngr);
                    }
                }
                else if(Customer* cust = dynamic_cast<Customer*>(currUser)){
                    if(userChoice != 4){ // If not logout (Customer's logout is choice 4)
                        cust->handleChoice(userChoice,bookRepo,transMngr);
                    }
                }
            }while((userChoice!=6 && dynamic_cast<Librarian*>(currUser))||(userChoice!=4 && dynamic_cast<Customer*>(currUser)));

            currUser=nullptr; // Clear current user on logout
            cout<<"Logged out successfully.\n";
        }
    }

public:
    // Main application loop
    void run(){
        int choice;
        do{
            cout<<"\n===== Library Management System =====\n"
                                        <<"1. Login\n"
                                        <<"2. Sign Up\n"
                                        <<"3. Exit\n"
                                        <<"Enter Choice : \n";
            cin>>choice;
            // Clearing the input buffer after cin>>choice from main menu.
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice)
            {
            case 1:
                    handleLogin();
                break;

            case 2:
                    handleSignUp();
                break;

            case 3: // Exit option
                break; // No action needed, loop condition handles exit.

            default:
                    cout<<"Invalid main menu choice. Please try again.\n"; 
                break;
            }
        }while(choice != 3); // Loop until user chooses to exit
    }
};


int main(){
    LibraryApplication app;
    app.run(); 
    return 0; 
}

