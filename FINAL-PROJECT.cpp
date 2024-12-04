/* 
	EXPENSE TRACKER SYSTEM - C2B
	coded by Jeanne Paul Lynne Pinca and Mikayla Loera Marie Buno
*/

#include <iostream>
#include <vector>
#include <memory>
#include <limits>
#include <string>
#include <iomanip>
#include <list>
#include <set>
#include <chrono>
#include <ctime>
#include <sstream>
#include <functional>
#include <stdexcept>
#include <regex>
#include <cctype>
using namespace std;

class InputValidator { //call validations thru exception handlers
public:
    // Validate that input is not empty
    static void validateNotEmpty(const std::string& input) {
        if (input.empty()) {
            throw std::invalid_argument("Input cannot be empty.");
        }
    }

    // Validate that input is numeric
    static void validateIsNumeric(const std::string& input) {
    bool hasDecimal = false;
    
    for (char c : input) {
        if (!isdigit(c)) {
            if (c == '.' && !hasDecimal) {
                hasDecimal = true; // Allow one decimal point
            } else {
                throw std::invalid_argument("Input must be a number.");
            }
        }
    }

    // Ensure input is not empty and does not start with a decimal
	if (input.empty() || input[0] == '.') {
	        throw std::invalid_argument("Input must be a valid number. (no spaces and not start with a decimal.)");
	    }
	}

    // Validate input range
    static void validateRange(int value, int min, int max) {
        if (value < min || value > max) {
            throw std::out_of_range("Input is out of valid range.");
        }
    }
    
    static void validateNoSpaces(const string& input) {
        if (input.find(' ') != string::npos) {
            throw std::invalid_argument("Input cannot contain spaces.");
        }
    }
    
    // Validate username is not taken
    std::set<std::string> usernames; // To store unique usernames

    
    bool isUsernameTaken(const std::string& username) {
        return usernames.find(username) != usernames.end();
    }

    // Add a new username, ensuring no duplicates
    void addUsername(const std::string& username) {
        if (isUsernameTaken(username)) {
            throw std::invalid_argument("Username already exists. Please choose a different username.");
        }
        usernames.insert(username); // Add to the set
    }
    
    // Validate username has no symbols or spaces
	static void validateUsername(const std::string& username) {
	    if (username.empty()) {
	        throw std::invalid_argument("Username cannot be empty.");
	    }
	
	    // Check for leading or trailing spaces
	    if (username.front() == ' ' || username.back() == ' ') {
	        throw std::invalid_argument("Username cannot have leading or trailing spaces.");
	    }
	
	    // Check for invalid characters (only allow alphanumeric)
	    for (char c : username) {
	        if (!isalnum(c)) {
	            throw std::invalid_argument("Username can only contain letters and numbers.");
	        }
	    }
	
	    // Check for spaces inside the username
	    if (username.find(' ') != std::string::npos) {
	        throw std::invalid_argument("Username cannot contain spaces.");
	    }
	}
	
	// Validate password has no spaces
	static void validatePassword(const std::string& password) {
	    if (password.empty()) {
	        throw std::invalid_argument("Password cannot be empty.");
	    }
	
	    if (password.find(' ') != std::string::npos) {
	        throw std::invalid_argument("Password cannot contain spaces.");
	    }
	}
	
	// Validate date format (YYYY-MM-DD)
    static void validateDateFormat(const std::string& date) {
        if (!std::regex_match(date, std::regex(R"(^\d{4}-\d{2}-\d{2}$)"))) {
            throw std::invalid_argument("Date must be in YYYY-MM-DD format.");
        }
	}
	
	// Static helper method to convert a string to lowercase
    static string toLowerCase(const string& str) {
        string lowerStr;
        for (char c : str) {
            lowerStr += tolower(c);
        }
        return lowerStr;
    }
};

// made UserInterface as an Abstract Base Class
class UserInterface {
public:
    virtual void displayScreen() const = 0; 

    void validateNumericInput(int& input, int min, int max) const {
        while (!(cin >> input) || input < min || input > max) {
            cout << "Invalid choice. Please try again: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    void getValidDouble(double& input) const {
        while (!(cin >> input) || input < 0) {
            cout << "Invalid input. Please enter a positive value: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    virtual ~UserInterface() = default;
};

class Expense {
	private:
		int id;
		string category;
		double amount;
		string date;
		
	public:
		Expense(int id, const string& category, double amount, const string& date) : id(id), category(category), amount(amount), date(date) {}

	    virtual ~Expense() = default;
	
	    int getId() const { return id; }
		void setCategory(string newCategory) {category = newCategory;}
	    string getCategory() const { return category; }
		void setAmount(double newAmount) {amount = newAmount;}
	    double getAmount() const { return amount; }
	    void setDate(string newDate) {date = newDate;}
	    string getDate() const { return date; }
	
	    virtual void displayExpense() const = 0; // Abstraction
};

class DetailedExpense : public Expense {
public:
    DetailedExpense(int id, const string& category, double amount, const string& date)
        : Expense(id, category, amount, date) {}

    void displayExpense() const override {
        cout << "ID: " << getId() << ", Category: " << getCategory()
             << ", Amount: " << getAmount() << ", Date: " << getDate() << endl;
    }
};

class User {
private:
    string username;
    string password;
    vector<shared_ptr<Expense>> expenses;
    double budget;

public:
    User(const string& username, const string& password, double budget)
        : username(username), password(password), budget(budget) {}

    string getUsername() const { return username; }
    bool verifyPassword(const string& inputPassword) const { return password == inputPassword; }
    void setBudget(double newBudget) {budget = newBudget;}
    double getBudget() const { return budget; }

    // Non-const version
    vector<shared_ptr<Expense>>& getExpenses() { return expenses; }

    // Const version
    const vector<shared_ptr<Expense>>& getExpenses() const { return expenses; }

    void addExpense(shared_ptr<Expense> expense) { expenses.push_back(expense); }

    void displayExpenses() const {
        if (expenses.empty()) {
            cout << "No expenses to display." << endl;
            return;
        }
        cout << "Expenses for user: " << username << endl;
        for (const auto& expense : expenses) {
            expense->displayExpense();
        }
    }
};

class BudgetManager {
private:
    User& user; // Reference to the User object

public:
    explicit BudgetManager(User& user) : user(user) {}

    double getBudget() const {
        return user.getBudget();
    }

    void updateBudget(double updatedBudget) {
        if (updatedBudget < 0) {
            cout << "\n> Error: Budget cannot be negative!" << endl;
            return;
        }
        user.setBudget(updatedBudget);
        cout << "> Successfully changed the budget!" << endl;
        cout << "\nCURRENT BUDGET: " << user.getBudget() << endl;
    }

    double getRemainingBudget() const {
        double totalExpenses = 0;
        for (const auto& expense : user.getExpenses()) {
            totalExpenses += expense->getAmount();
        }
        return user.getBudget() - totalExpenses;
    }

    void manageBudgetPrompt() {
        char modifyChoice;
        system("cls");
        cout << "================================================" << endl;
        cout << "              MANAGE BUDGET                     " << endl;
        cout << "================================================" << endl;

        cout << "\nCURRENT BUDGET: " << getRemainingBudget() << endl;
        
	    do {
		    cout << "\n> Modify existing budget? (Y/N): ";
        	cin >> modifyChoice;
		    if (tolower(modifyChoice) != 'y' && tolower(modifyChoice) != 'n') {
				cout << "Invalid Answer!\n";
			}
		    if (tolower(modifyChoice) == 'y') {
	            double newBudget;
	            cout << "\n> Input the amount of the new budget: ";
	            while (!(cin >> newBudget) || newBudget < 0) {
	                cin.clear();
	                cin.ignore(numeric_limits<streamsize>::max(), '\n');
	                cout << "Invalid input! Please enter a positive value: ";
	            }
            updateBudget(newBudget);
        	}
		} while (tolower(modifyChoice) != 'y' && tolower(modifyChoice) != 'n');
		
		if(tolower(modifyChoice) == 'n') {
	        cout << endl << "\n> Redirecting to the main menu ..." << endl;
		} else {
            cout << "\n> Successfully updated the budget!" << endl;
        }
        
        
        cout << "> Press any key to continue ...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }
};

// Strategy
class ExpenseViewStrategy {
public: 
	static bool stringToDate(const string& dateStr, struct tm& date) {
    istringstream ss(dateStr);
    ss >> get_time(&date, "%Y-%m-%d");
    return !ss.fail();  // Returns true if conversion is successful
	}
	
    virtual void viewExpenses(const User& user, double& totalExpenses) const = 0; // abstraction
    virtual ~ExpenseViewStrategy() {}
};

class WeeklyViewStrategy : public ExpenseViewStrategy {
public:
	bool isSameWeek(const std::tm& date1, const std::tm& date2) {
    auto time1 = std::mktime(const_cast<std::tm*>(&date1));
    auto time2 = std::mktime(const_cast<std::tm*>(&date2));
    const int SECONDS_IN_A_WEEK = 7 * 24 * 60 * 60;

    return std::abs(std::difftime(time1, time2)) < SECONDS_IN_A_WEEK &&
           date1.tm_wday <= date2.tm_wday; // Ensure they're in the same week
	}
	
    void viewExpenses(const User& user, double& totalExpenses) const override {
        cout << "\n> Viewing expenses for the past week:\n";
        cout << "\n-------------------------------------------------------\n";
        cout << "ID\tAMOUNT\tCATEGORY\tDATE\n";
        cout << "-------------------------------------------------------\n";

        time_t now = time(nullptr);
        tm* currentDate = localtime(&now); // Current date
        tm expenseDate = {};

        bool found = false; // To track if any expenses were found
        for (const auto& expense : user.getExpenses()) {
            if (stringToDate(expense->getDate(), expenseDate)) {
                time_t expenseTime = mktime(&expenseDate);
                double secondsDiff = difftime(now, expenseTime);
                double daysDiff = secondsDiff / (60 * 60 * 24); // Convert seconds to days

                if (daysDiff <= 7) { // Expense is within the last 7 days
                    cout << setw(5) << expense->getId() << "\t"
                         << setw(7) << expense->getAmount() << "\t"
                         << setw(10) << expense->getCategory() << "\t"
                         << expense->getDate() << endl;
                    totalExpenses += expense->getAmount();
                    found = true;
                }
            }
        }

        if (!found) {
            cout << "> No expenses made in the past week.\n";
        }
    }
};

class MonthlyViewStrategy : public ExpenseViewStrategy {
public:
	bool isSameMonth(const std::tm& date1, const std::tm& date2) {
    return date1.tm_year == date2.tm_year && date1.tm_mon == date2.tm_mon;
	}
	
    void viewExpenses(const User& user, double& totalExpenses) const override {
        cout << "\n> Please enter the month (#) you want your expenses to be viewed (1 - 12): ";
        int month;
        cin >> month;

        while (month < 1 || month > 12) {
            cout << "Invalid month! Please enter a valid month (1 - 12): ";
            cin >> month;
        }

        time_t now = time(nullptr);
        tm currentDate = *localtime(&now);
        int currentYear = currentDate.tm_year + 1900;

        cout << "\n> Viewing expenses for the selected month of the current year (" << currentYear << "):\n";
        cout << "\n-------------------------------------------------------\n";
        cout << "ID\tAMOUNT\tCATEGORY\tDATE\n";
        cout << "-------------------------------------------------------\n";

        bool found = false;
        for (const auto& expense : user.getExpenses()) {
            tm expenseDate = {};
            if (stringToDate(expense->getDate(), expenseDate)) {
                int expenseYear = expenseDate.tm_year + 1900;
                if (expenseDate.tm_mon + 1 == month && expenseYear == currentYear) {
                    cout << setw(5) << expense->getId() << "\t"
                         << setw(7) << expense->getAmount() << "\t"
                         << setw(10) << expense->getCategory() << "\t"
                         << expense->getDate() << endl;
                    totalExpenses += expense->getAmount();
                    found = true;
                }
            }
        }

        if (!found) {
            cout << "> No expenses made for this month in " << currentYear << ".\n";
        }
    }
};

class YearlyViewStrategy : public ExpenseViewStrategy {
public:
	bool isSameYear(const std::tm& date1, const std::tm& date2) {
    return date1.tm_year == date2.tm_year;
	}
	
    void viewExpenses(const User& user, double& totalExpenses) const override {
        cout << "\n> Viewing expenses for the current year:\n";
        cout << "\n-------------------------------------------------------\n";
        cout << "ID\tAMOUNT\tCATEGORY\tDATE\n";
        cout << "-------------------------------------------------------\n";

        time_t now = time(nullptr);
        tm currentDate = *localtime(&now);
        int currentYear = currentDate.tm_year + 1900;

        bool found = false;
        for (const auto& expense : user.getExpenses()) {
            tm expenseDate = {};
            if (stringToDate(expense->getDate(), expenseDate)) {
                int expenseYear = expenseDate.tm_year + 1900;
                if (expenseYear == currentYear) {
                    cout << setw(5) << expense->getId() << "\t"
                         << setw(7) << expense->getAmount() << "\t"
                         << setw(10) << expense->getCategory() << "\t"
                         << expense->getDate() << endl;
                    totalExpenses += expense->getAmount();
                    found = true;
                }
            }
        }

        if (!found) {
            cout << "> No expenses made for this year.\n";
        }
    }
};

class CategoryViewStrategy : public ExpenseViewStrategy {
public:
    void viewExpenses(const User& user, double& totalExpenses) const override {
        set<string> categories;
        for (const auto& expense : user.getExpenses()) {
            categories.insert(expense->getCategory());
        }

        cout << "\n> Your available categories:\n";
        cout << "---------------------------------\n";
        for (const auto& category : categories) {
            cout << category << endl;
        }

        string category;
        cout << "\n> Please input the category you wanted the expenses to be viewed.\n";
        cout << "CATEGORY: ";
        cin >> category;

        string categoryLower = InputValidator::toLowerCase(category);
        bool categoryFound = false;

        cout << "\n-------------------------------------------------------\n";
        cout << "ID\tAMOUNT\tCATEGORY\tDATE\n";
        cout << "-------------------------------------------------------\n";

        for (const auto& expense : user.getExpenses()) {
            if (InputValidator::toLowerCase(expense->getCategory()) == categoryLower) {
                cout << expense->getId() << "\t"
                     << expense->getAmount() << "\t"
                     << expense->getCategory() << "\t"
                     << expense->getDate() << endl;
                totalExpenses += expense->getAmount();
                categoryFound = true;
            }
        }

        if (!categoryFound) {
            cout << "\n> No expenses found in this category.\n";
        }
    }
};

class AllViewStrategy : public ExpenseViewStrategy {
    
	void viewExpenses(const User& user, double& totalExpenses) const override {
        cout << "\n-------------------------------------------------------\n";
        cout << "ID\tAMOUNT\tCATEGORY\tDATE\n";
        cout << "-------------------------------------------------------\n";
        //displayExpense()
		for (const auto& expense : user.getExpenses()) {
            cout << expense->getId() << "\t"
                 << setw(7) << expense->getAmount() << "\t"
                 << setw(10) << expense->getCategory() << "\t"
                 << expense->getDate() << endl;
                 totalExpenses += expense->getAmount(); // Accumulate total for all expenses
        }
    }

};

class ExpenseManager { 
private:
    list<shared_ptr<Expense>> expenses; 
	shared_ptr<ExpenseViewStrategy> viewStrategy; 
    string expenseId;
    double amount;
    double totalExpenses;

public:
	
	static bool stringToDate(const string& dateStr, struct tm& date) {
    istringstream ss(dateStr);
    ss >> get_time(&date, "%Y-%m-%d");
    return !ss.fail();  // Returns true if conversion is successful
	}
		
	void addExpense(User& user, BudgetManager& budgetManager) { 
    system("cls"); // Clear the screen
    string menuTitle = "ADD EXPENSE";
    printHeader(menuTitle);
    cout << "> Please enter the following expense details to add." << endl;
    cout << "> Input 'x' to cancel anytime." << endl;

    cout << "CURRENT BUDGET: " << budgetManager.getRemainingBudget() << endl;

    string amountInput, category, date;
    double amount;
    struct tm expenseDate = {};  // Initialize tm struct
    time_t now = time(nullptr); // Get current time
    struct tm* currentDate = localtime(&now); // Convert to local time

    try {
        // Loop until valid expense amount is entered
        while (true) {
            cout << "\nAMOUNT: ";
            cin >> amountInput;
            if (amountInput == "x" || amountInput == "X") {
                cout << "> Operation canceled. Redirecting to main menu..." << endl;
                return;
            }

            try {
                InputValidator::validateIsNumeric(amountInput);
                amount = stod(amountInput); // Convert validated input to double
                if (amount > budgetManager.getBudget()) {
                    throw std::invalid_argument("Insufficient Budget! Cannot exceed the available budget.");
                }
                break; // Exit loop on valid input
            } catch (const std::exception& e) {
                cout << "Error: " << e.what() << "\nPlease try again.\n";
            }
        }

        // Loop until valid category is entered
        while (true) {
            cout << "CATEGORY: ";
            cin >> category;
            if (category == "x" || category == "X") {
                cout << "> Operation canceled. Redirecting to main menu..." << endl;
                return;
            }

            try {
                InputValidator::validateNotEmpty(category);
                break; // Exit loop on valid input
            } catch (const std::exception& e) {
                cout << "Error: " << e.what() << "\nPlease try again.\n";
            }
        }

        // Loop until valid date is entered
        while (true) {
            cout << "DATE (YYYY-MM-DD): ";
            cin >> date;
            if (date == "x" || date == "X") {
                cout << "> Operation canceled. Redirecting to main menu..." << endl;
                return;
            }

            try {
                InputValidator::validateDateFormat(date);
                if (!stringToDate(date, expenseDate)) {
                    throw std::invalid_argument("Invalid date format. Please enter a valid date.");
                }

                // Check if date is in the future
                if (mktime(&expenseDate) > mktime(currentDate)) {
                    throw std::invalid_argument("Date cannot be in the future.");
                }
                break; // Exit loop on valid input
            } catch (const std::exception& e) {
                cout << "Error: " << e.what() << "\nPlease try again.\n";
            }
        }

        // Generate ID and add the expense
        int id = static_cast<int>(user.getExpenses().size()) + 1;
        auto newExpense = make_shared<DetailedExpense>(id, category, amount, date);
        user.addExpense(newExpense);

        // Display success message
        cout << "\n> Expense added successfully!\n" << endl;
        cout << "EXPENSE ID: " << id << endl;
        cout << "AMOUNT: " << amount << endl;
        cout << "CATEGORY: " << category << endl;
        cout << "DATE: " << put_time(&expenseDate, "%Y-%m-%d") << endl;
        cout << "\nREMAINING BUDGET: " << budgetManager.getRemainingBudget() << endl;

    } catch (const std::exception& e) {
        // Handle validation or other errors
        cout << "Error: " << e.what() << endl;
        cout << "> Redirecting to the main menu..." << endl;
        cout << "> Press any key to continue ...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return;
    }

    // Prompt to add another expense
    char choice;
    do {
	    cout << "\n> Add another expense? (Y/N): ";
	    cin >> choice;
	    if (tolower(choice) != 'y' && tolower(choice) != 'n') {
			cout << "Invalid Answer!\n";
		}
	    if (tolower(choice) == 'y') {
	        addExpense(user, budgetManager);
	    }
	} while (tolower(choice) != 'y' && tolower(choice) != 'n');
	
	if(tolower(choice) == 'n') {
        cout << endl << "> Redirecting to the main menu ..." << endl;
        cout << "> Press any key to continue ...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
	} 
}
	
	void setViewStrategy(const shared_ptr<ExpenseViewStrategy>& strategy) {
        viewStrategy = strategy;
    }
	
	void handleExpensesView(const User& user) {
	    int choice;
	    char cancelChoice;
	
	    do {
	        cout << "> Select display type to view your expenses:" << endl;
	        cout << "> Input 'x' to cancel anytime." << endl;
	        cout << "1 - Category\n";
	        cout << "2 - Weekly\n";
	        cout << "3 - Monthly\n";
	        cout << "4 - Yearly\n";
	        cout << "5 - View All\n";
	        cout << "CHOICE: ";
	
	        string input;
	        cin >> input;
	
	        // Check if the user entered 'x' or 'X' to cancel
	        if (input == "x" || input == "X") {
	            return;  // Exit if 'x' is chosen
	        }
	
	        // Try to convert the input to an integer
	        try {
	            choice = stoi(input); // Convert input to integer
	        } catch (const invalid_argument&) {
	            cout << "Invalid choice! Please enter a number between 1 and 5.\n";
	            continue; // Skip to the next loop iteration
	        }
	
	        switch (choice) {
	            case 1:
	                setViewStrategy(make_shared<CategoryViewStrategy>());
	                break;
	            case 2:
	                setViewStrategy(make_shared<WeeklyViewStrategy>());
	                break;
	            case 3:
	                setViewStrategy(make_shared<MonthlyViewStrategy>());
	                break;
	            case 4:
	                setViewStrategy(make_shared<YearlyViewStrategy>());
	                break;
	            case 5:
	                setViewStrategy(make_shared<AllViewStrategy>());
	                break;
	            default:
	                cout << "Invalid choice! Please select a valid option.\n";
	                continue; // Go back to the top of the loop
	        }
	
	        // If a valid choice was made, break out of the loop
	        break;
	    } while (true);
	}


	void expensesView(const User& user, double& totalExpenses) {
        if (!viewStrategy) {
            cout << "No view strategy selected!\n";
            return;
        }
        viewStrategy->viewExpenses(user, totalExpenses);
    }

	void viewExpenses(User& user) {
    int choice;
    string menuTitle = "VIEW EXPENSE";
    system("cls");
    	// header
        printHeader(menuTitle); 
		
		if (user.getExpenses().empty()) {
			cout << "> You do not have any expense entries yet." << endl;
			cout << "> Redirecting to main menu." << endl;
			cout << "> Press any key to continue ...";
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cin.get();
			return;
		}   
		    
		// Let the user select a strategy
	    handleExpensesView(user);
	
	    // Execute the selected strategy
	    expensesView(user, totalExpenses);

		char viewChoice;
	    do {
	        cout << "\n> View in another display type? (Y/N): ";
	        cin >> viewChoice;
	    } while (tolower(viewChoice) != 'y' && tolower(viewChoice) != 'n');  // Loop until valid input
	
	    if (tolower(viewChoice) == 'y') {
	        viewExpenses(user);  // Recursive call to view expenses again
	    } else {
	        cout << endl << "> Redirecting to the main menu ..." << endl;
	        cout << "> Press any key to continue ...";
	        cin.ignore(numeric_limits<streamsize>::max(), '\n');
	        cin.get();
	    }
	}


	void modifyExpense(User& user, BudgetManager& budgetManager) {	
    string menuTitle = "MODIFY EXPENSE";
    system("cls");
    printHeader(menuTitle);
    
    if (user.getExpenses().empty()) {
        cout << "> You do not have any expense entries yet." << endl;
        cout << "> Redirecting to the main menu..." << endl;
        system("pause");
        return;
    }
    
    handleExpensesView(user);
    expensesView(user, totalExpenses);

    int id;
    cout << "\n> Input expense ID to modify (or '0' to cancel): ";
    cin >> id;

    if (id == 0) {
        return;
    }

    // Locate the expense manually using a loop
    auto& expenses = user.getExpenses();
    shared_ptr<Expense> expense = nullptr;

    for (auto& exp : expenses) {
        if (exp->getId() == id) {
            expense = exp;
            break;
        }
    }

    if (!expense) {
        cout << "> Expense ID not found. Returning to main menu..." << endl;
        system("pause");
        return;
    }

    // Display current details
    cout << "\nCurrent Details:" << endl;
    cout << "ID: " << expense->getId() << endl;
    cout << "Amount: " << expense->getAmount() << endl;
    cout << "Category: " << expense->getCategory() << endl;
    cout << "Date: " << expense->getDate() << endl;

    // Modify expense fields
    double newAmount = expense->getAmount();
    string newCategory = expense->getCategory();
    string newDate = expense->getDate();

    cout << "\nEnter new details (leave blank to retain current value):" << endl;

    // Loop until valid amount is entered
    string amountInput;
    cout << "New Amount: ";
    cin.ignore(); // Clear input buffer
    getline(cin, amountInput);
    if (!amountInput.empty()) {
        while (true) {
            try {
                InputValidator::validateIsNumeric(amountInput);
                newAmount = stod(amountInput); // Convert validated input to double
                if (newAmount > budgetManager.getRemainingBudget()) {
                    throw std::invalid_argument("Insufficient Budget! Cannot exceed the available budget.");
                }
                break; // Exit loop on valid input
            } catch (const std::exception& e) {
                cout << "Error: " << e.what() << "\nPlease try again.\n";
                cout << "New Amount: ";
                getline(cin, amountInput);
                if (amountInput.empty()) break;
            }
        }
    }

    // Loop until valid category is entered
    string categoryInput;
    cout << "New Category: ";
    getline(cin, categoryInput);
    if (!categoryInput.empty()) {
        while (true) {
            try {
                InputValidator::validateNotEmpty(categoryInput);
                // Ensure category doesn't contain numbers or symbols
                for (char c : categoryInput) {
                    if (!isalnum(c) && c != ' ') {
                        throw std::invalid_argument("Category can only contain letters and numbers.");
                    }
                }
                newCategory = categoryInput;
                break; // Exit loop on valid input
            } catch (const std::exception& e) {
                cout << "Error: " << e.what() << "\nPlease try again.\n";
                cout << "New Category: ";
                getline(cin, categoryInput);
                if (categoryInput.empty()) break;
            }
        }
    }

    // Loop until valid date is entered
    string dateInput;
    cout << "New Date (YYYY-MM-DD): ";
    getline(cin, dateInput);
    if (!dateInput.empty()) {
        while (true) {
            try {
                InputValidator::validateDateFormat(dateInput);
                struct tm expenseDate = {};
                if (!stringToDate(dateInput, expenseDate)) {
                    throw std::invalid_argument("Invalid date format. Please enter a valid date.");
                }

                // Check if date is in the future
                time_t now = time(nullptr);
                struct tm* currentDate = localtime(&now); // Convert to local time
                if (mktime(&expenseDate) > mktime(currentDate)) {
                    throw std::invalid_argument("Date cannot be in the future.");
                }

                newDate = dateInput;
                break; // Exit loop on valid input
            } catch (const std::exception& e) {
                cout << "Error: " << e.what() << "\nPlease try again.\n";
                cout << "New Date (YYYY-MM-DD): ";
                getline(cin, dateInput);
                if (dateInput.empty()) break;
            }
        }
    }

    // Update expense details
    expense->setAmount(newAmount);
    expense->setCategory(newCategory);
    expense->setDate(newDate);

    cout << "\n> Expense modified successfully!" << endl;
    cout << "Updated Details:" << endl;
    cout << "Amount: " << expense->getAmount() << endl;
    cout << "Category: " << expense->getCategory() << endl;
    cout << "Date: " << expense->getDate() << endl;

    // Display current budget

    // Prompt for additional modifications
    char modChoice;
    do {
        cout << "\n> Modify another expense? (Y/N): ";
        cin >> modChoice;
        if (tolower(modChoice) != 'y' && tolower(modChoice) != 'n') {
            cout << "Invalid Answer!\n";
        }
        if (tolower(modChoice) == 'y') {
            modifyExpense(user, budgetManager); // Recursive call for additional modifications
        }
    } while (tolower(modChoice) != 'y' && tolower(modChoice) != 'n');
    
    if(tolower(modChoice) == 'n') {
        cout << endl << "> Redirecting to the main menu ..." << endl;
        cout << "> Press any key to continue ...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }
}

    void removeExpense(User& user, BudgetManager& budgetManager) {
    	system("cls");
    	int choice;
    	string menuTitle = "REMOVE EXPENSE";
        printHeader(menuTitle);

		if (user.getExpenses().empty()) {
        cout << "\n> You do not have any expense entries yet." << endl;
        cout << "> Redirecting to the main menu..." << endl;
        system("pause");
        return;
    	}
	    
		handleExpensesView(user);
		expensesView(user, totalExpenses);

 		// Prompt for ID
	    int expenseIdToDelete;
	    cout << "\n> Enter the ID of the expense you want to delete (or '0' to cancel): ";
	    cin >> expenseIdToDelete;
	    
	    if (expenseIdToDelete == 0) {
	        return;
	    }
	
	    // Locate thexpense manually using a loop
	    auto& expenses = user.getExpenses();
	    shared_ptr<Expense> expense = nullptr;
	    auto it = expenses.end(); // Iterator to track the expense
	
	    for (auto iter = expenses.begin(); iter != expenses.end(); ++iter) {
	        if ((*iter)->getId() == expenseIdToDelete) {
	            expense = *iter;
	            it = iter;
	            break;
	        }
	    }
	
	    if (!expense) {
	        cout << "\n> Expense ID not found. Returning to main menu..." << endl;
	        system("pause");
	        return;
	    }
	
	    // Display the expense details
	    cout << "\nExpense Details:" << endl;
	    cout << "ID: " << expense->getId() << endl;
	    cout << "Amount: " << expense->getAmount() << endl;
	    cout << "Category: " << expense->getCategory() << endl;
	    cout << "Date: " << expense->getDate() << endl;
	
	    // Confirm deletion
	    char deleteChoice;
	    cout << "\n> Delete this expense? (Y/N): ";
	    cin >> deleteChoice;
	
	    if (tolower(deleteChoice) == 'y') {
	        expenses.erase(it);
	        cout << "\n> Expense deleted successfully!" << endl;
	    } else {
	        cout << "\n> Deletion canceled." << endl;
	    }
	
	    // Display current budget
	    cout << "\nCURRENT BUDGET: " << budgetManager.getRemainingBudget() << endl;
	
	    // Prompt for additional deletions
	    char delChoice;
	    do {
		    cout << "\n> Delete another expense? (Y/N): ";
		    cin >> delChoice;
		    if (tolower(delChoice) != 'y' && tolower(delChoice) != 'n') {
				cout << "Invalid Answer!\n";
			}
		    if (tolower(delChoice) == 'y') {
		        removeExpense(user, budgetManager); // Recursive call for additional deletions
		    }
		} while (tolower(delChoice) != 'y' && tolower(delChoice) != 'n');
		
		if(tolower(delChoice) == 'n') {
	        cout << endl << "\n> Redirecting to the main menu ..." << endl;
	        cout << "> Press any key to continue ...";
	        cin.ignore(numeric_limits<streamsize>::max(), '\n');
	        cin.get();
		} 
	}

    void checkBudget(const User& user) const {}
	
    void generateReport(User& user, BudgetManager& budgetManager){ 
    	system("cls");
    	string menuTitle = "EXPENSE REPORT";
        printHeader(menuTitle);
        
        if (user.getExpenses().empty()) {
        cout << "\n> You do not have any expense entries yet." << endl;
        cout << "> Redirecting to main menu." << endl;
        cout << "> Press any key to continue ...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return;
    	}
    
	    // Display filtered expenses and calculate total
	    double totalExpenses = 0;
	    
	    // Call expensesView and calculate total expenses
	    handleExpensesView(user);
	    expensesView(user, totalExpenses);

	    cout << "\nTOTAL EXPENSE: " << totalExpenses << endl;
	    cout << "CURRENT BUDGET: " << budgetManager.getRemainingBudget() << endl;
	    
	    char mainChoice;
	    do {
		    cout << "\n> Return to main menu? (Y/N): ";
	    	cin >> mainChoice;
		    if (tolower(mainChoice) != 'y' && tolower(mainChoice) != 'n') {
				cout << "\n> Invalid Answer!\n";
			}
		    if (tolower(mainChoice) == 'n') {
		    	generateReport(user, budgetManager);
		    }
		} while (tolower(mainChoice) != 'y' && tolower(mainChoice) != 'n');
		
		if(tolower(mainChoice) == 'y') {
	        cout << endl << "\n> Redirecting to the main menu ..." << endl;
	        cout << "> Press any key to continue ...";
	        cin.ignore(numeric_limits<streamsize>::max(), '\n');
	        cin.get();
		} 
	}

//------------------ HELPER FUNCTIONS ----------------------

	void printHeader(const string& menuTitle) const {
    cout << "================================================" << endl;
    cout << "           " << menuTitle << "                  " << endl;
    cout << "================================================" << endl;
	}	
};

class AccountManager {	//singleton implementation
private:
    static AccountManager* instance; // Static instance of the Singleton
    vector<User> users;              // Store registered users

    AccountManager() {} // Private constructor

public:
    // Delete copy constructor and assignment operator
    AccountManager(const AccountManager&) = delete;
    AccountManager& operator=(const AccountManager&) = delete;

    const vector<User>& getUsers() const {
        return users; // Return reference to the users vector
    }
	
    static AccountManager* getInstance() {
        if (!instance) {
            instance = new AccountManager();
        }
        return instance;
    }

	bool registerUser(const string& username, const string& password, double budget) {
	    for (const auto& user : users) {
	        if (user.getUsername() == username) {
	            return false; // Username already exists
	        }
	    }
	    users.emplace_back(username, password, budget);
	    return true; // Registration successful
	}


    User* login(const string& username, const string& password) {
        for (auto& user : users) {
            if (user.getUsername() == username) {
                if (user.verifyPassword(password)) {
                    cout << "Welcome, " << username << "!" << endl;
                    return &user;
                } else {
                    cout << "Invalid password!" << endl;
                    return nullptr;
                }
            }
        }
        cout << "User doesn't exist." << endl;
        return nullptr;
    }
};

// Define the static member
AccountManager* AccountManager::instance = nullptr;

class MainMenuScreen : public UserInterface {
private:
    User& currentUser;
    BudgetManager budgetManager;
    ExpenseManager expenseManager;

public:
    MainMenuScreen(User& user)
        : currentUser(user), budgetManager(user) {}

    void displayScreen() const override {
    	system("cls");
        cout << "================================================" << endl;
        cout << "     EXPENSE TRACKER MAIN MENU                 " << endl;
        cout << "================================================" << endl;
        cout << "1 - Add Expense" << endl;
        cout << "2 - View Expenses" << endl;
        cout << "3 - Modify Expenses" << endl;
        cout << "4 - Manage Budget" << endl;
        cout << "5 - Remove Expenses" << endl;
        cout << "6 - Generate Report" << endl;
        cout << "7 - Logout" << endl;
        cout << "8 - Exit" << endl;
        cout << "\nHello, '" << currentUser.getUsername() << "'!" << endl; 
        cout << "> Please input your choice: ";
    }

    void handleMainMenu() {
        int choice;
        while (true) {
            displayScreen();
            validateNumericInput(choice, 1, 8);

            switch (choice) {
                case 1:
                    expenseManager.addExpense(currentUser, budgetManager);
                    break;
                case 2:
                    expenseManager.viewExpenses(currentUser);
                    break;
                case 3:
                    expenseManager.modifyExpense(currentUser, budgetManager);
                    break;
                case 4:
                    budgetManager.manageBudgetPrompt();
                    break;
                case 5:
                    expenseManager.removeExpense(currentUser, budgetManager);
                    break;
                case 6:
                    expenseManager.generateReport(currentUser, budgetManager);
                    break;
                case 7:
                	cout <<"logging out, returning to the start screen ..." << endl;
                	system("pause");
                    return; 
                case 8:
                	cout << "Exiting the program..." << endl;
                	exit(0);
                default:
                    cout << "Invalid choice. Please try again." << endl;
            }
        }
    }
};

class StartScreen : public UserInterface {
public:

    StartScreen() {}

    void displayScreen() const override {
        system("cls");
        cout << "================================================" << endl;
        cout << "                  EXPENSE TRACKER              " << endl;
        cout << "================================================" << endl;
        cout << "1 - Register an account" << endl;
        cout << "2 - Login" << endl;
        cout << "3 - Exit" << endl;
        cout << "> Please enter your choice: ";
    }

    void handleStartMenu() {
        int choice;
        while (true) {
            displayScreen();
            validateNumericInput(choice, 1, 3);

            if (choice == 1) {
                handleRegistration();
            } else if (choice == 2) {
                handleLogin();
            } else if (choice == 3) {
                cout << "Thank you for using the Expense Tracker. Goodbye!" << endl;
                break;
            }
        }
    }

private:
	void handleRegistration() const {
    string username, password, choice;
    InputValidator inputValidator;
    double budget;

    system("cls");
    cout << "================================================" << endl;
    cout << "                  REGISTER USER                 " << endl;
    cout << "================================================" << endl;
    cout << "> Please enter the following credentials (Case Sensitive)." << endl;
    cout << "> Input 'x' to cancel anytime." << endl;

    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Get username
    while (true) {
        cout << "\nEnter username: ";
        std::getline(cin, username);

        if (username == "x") {
            cout << "\nCancelling registration. Returning to start menu..." << endl;
            system("pause");
            return;
        }

        try {
            inputValidator.addUsername(username);
            InputValidator::validateNotEmpty(username);
            InputValidator::validateUsername(username);

            // Check if username already exists
            for (const auto& user : AccountManager::getInstance()->getUsers()) {
                if (user.getUsername() == username) {
                    throw std::invalid_argument("Username already exists. Please try again.");
                }
            }
            break; // Unique username accepted
        } catch (const std::invalid_argument& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    // Get password
    while (true) {
        cout << "Enter password: ";
        std::getline(cin, password);

        if (password == "x") {
            cout << "\nCancelling registration. Returning to start menu..." << endl;
            system("pause");
            return;
        }

        try {
            InputValidator::validateNotEmpty(password);
            InputValidator::validatePassword(password);
            break;
        } catch (const std::invalid_argument& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    // Get budget
    while (true) {
        cout << "Enter initial budget: ";
        string budgetInput;
        std::getline(cin, budgetInput);

        if (budgetInput == "x") {
            cout << "\nCancelling registration. Returning to start menu..." << endl;
            system("pause");
            return;
        }

        try {
            InputValidator::validateNoSpaces(budgetInput);
            InputValidator::validateIsNumeric(budgetInput);
            budget = stod(budgetInput);
            if (budget <= 0) {
                throw std::invalid_argument("Budget must be a positive number.");
            }
            break;
        } catch (const std::exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    // Confirm details before registration
    cout << "\nPlease confirm the details below:\n";
    cout << "Username: " << username << endl;
    cout << "Password: " << password << endl;
    cout << "Initial Budget: " << budget << endl;

    cout << "\nDo you want to confirm the details? (Y/N): ";
    cin >> choice;

    if (choice == "Y" || choice == "y") {
        if (AccountManager::getInstance()->registerUser(username, password, budget)) {
            cout << "\nAccount successfully registered for: " << username << endl;

            // Automatically log in
            User* user = AccountManager::getInstance()->login(username, password);
            if (user) {
                cout << "\nRedirecting to main menu ..." << endl;
                system("pause");
                MainMenuScreen mainMenu(*user);
                mainMenu.handleMainMenu(); // Redirect to main menu
            }
        } else {
            cout << "\nRegistration failed. Username might already exist." << endl;
            system("pause");
        }
    } else if (choice == "N" || choice == "n") {
        cout << "\nRedirecting to start menu ..." << endl;
        system("pause");
        return;
    } else {
        cout << "\nInvalid choice. Returning to start menu ..." << endl;
        system("pause");
        return;
    }
}

    void handleLogin() const {
        string username, password;
        system("cls");
        cout << "================================================" << endl;
        cout << "                    LOG IN                      " << endl;
        cout << "================================================" << endl;
        cout << "> Please enter the following credentials (Case Sensitive)." << endl;
        cout << "> Input 'x' to cancel anytime." << endl;

        cout << "\nEnter username: ";
        cin >> username;
        if (username == "x") {
            cout << "\nCancelling login. Returning to start menu..." << endl;
            system("pause");
            return;
        }

        cout << "Enter password: ";
        cin >> password;
        if (password == "x") {
            cout << "\nCancelling login. Returning to start menu..." << endl;
            system("pause");
            return;
        }

        User* user = AccountManager::getInstance()->login(username, password);
        if (user) {
            MainMenuScreen mainMenu(*user);
            mainMenu.handleMainMenu(); // Redirect to main menu
        } else {
            system("pause"); // Allow user to read the message before returning to start menu
        }
    }
};

int main() {
    StartScreen startScreen;
    startScreen.handleStartMenu();
    return 0;
}