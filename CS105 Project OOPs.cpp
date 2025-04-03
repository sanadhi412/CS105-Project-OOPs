// Eats & Treats Restaurant Management System
// A complete C++ implementation with inheritance, polymorphism, abstraction, and encapsulation
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <limits>
#include <memory>

#include "File_Z.h"

using namespace std;

using namespace TwoCli;

// Forward declarations
class Order;
class Table;
class MenuItem;

// Utility functions
std::string getCurrentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return std::string(buf);
}

// class for additems to live moitor 
//rik

class KitchenOrder : public TwoCli::stbase {
public:
    int id;
    int tableNumber;
    std::string status;
    std::string itemList;

    // Required by stbase
    std::string toString() const override {
        return std::to_string(id) + "|" + std::to_string(tableNumber) + "|" + status + "|" + itemList;
    }

    // Required by FileManager
    static KitchenOrder fromString(const std::string& line) {
        KitchenOrder order;
        std::stringstream ss(line);
        std::string part;

        getline(ss, part, '|');
        order.id = std::stoi(part);

        getline(ss, part, '|');
        order.tableNumber = std::stoi(part);

        getline(ss, part, '|');
        order.status = part;

        getline(ss, part, '|');
        order.itemList = part;

        return order;
    }
};


//

// Abstract base class for all users of the system
class User {
protected:
    std::string name;
    std::string id;
    std::string role;

public:
    User(const std::string& name, const std::string& id, const std::string& role)
        : name(name), id(id), role(role) {
    }

    virtual ~User() {}

    virtual void displayMenu() const = 0;

    std::string getName() const { return name; }
    std::string getId() const { return id; }
    std::string getRole() const { return role; }
};

// Derived class for hostess/host
class Host : public User {
public:
    Host(const std::string& name, const std::string& id)
        : User(name, id, "Host") {
    }

    void displayMenu() const override {
        std::cout << "\n===== HOST MENU =====\n";
        std::cout << "1. View Tables\n";
        std::cout << "2. Assign Table\n";
        std::cout << "3. View Reservations\n";
        std::cout << "4. Add Reservation\n";
        std::cout << "5. Cancel Reservation\n";
        std::cout << "6. Return to Main Menu\n";
    }
};

// Derived class for waitstaff
class Waiter : public User {
public:
    Waiter(const std::string& name, const std::string& id)
        : User(name, id, "Waiter") {
    }

    void displayMenu() const override {
        std::cout << "\n===== WAITER MENU =====\n";
        std::cout << "1. View Tables\n";
        std::cout << "2. Take Order\n";
        std::cout << "3. View Order Status\n";
        std::cout << "4. Serve Order\n";
        std::cout << "5. Generate Bill\n";
        std::cout << "6. Return to Main Menu\n";
    }
};

// Derived class for kitchen staff
class Chef : public User {
public:
    Chef(const std::string& name, const std::string& id)
        : User(name, id, "Chef") {
    }

    // old 
    //void displayMenu() const override {
    //    std::cout << "\n===== CHEF MENU =====\n";
    //    std::cout << "1. View Pending Orders\n";
    //    std::cout << "2. Update Order Status\n";
    //    std::cout << "3. Return to Main Menu\n";
    //}

    //new 
    void displayMenu() const override {
        std::cout << "\n===== CHEF MENU =====\n";
        std::cout << "1. View Pending Orders\n";
        std::cout << "2. Update Order Status\n";
        std::cout << "3. Live Order Monitor\n";  // New option
        std::cout << "4. Return to Main Menu\n";
    }



};

// Derived class for managers
class Manager : public User {
public:
    Manager(const std::string& name, const std::string& id)
        : User(name, id, "Manager") {
    }

    void displayMenu() const override {
        std::cout << "\n===== MANAGER MENU =====\n";
        std::cout << "1. View All Tables\n";
        std::cout << "2. View All Orders\n";
        std::cout << "3. Manage Menu Items\n";
        std::cout << "4. Generate Reports\n";
        std::cout << "5. Manage Staff\n";
        std::cout << "6. Return to Main Menu\n";
    }
};

// Class for menu items
class MenuItem {
private:
    std::string id;
    std::string name;
    std::string category;
    std::string description;
    double price;
    bool available;

public:
    MenuItem(const std::string& id, const std::string& name, const std::string& category,
        const std::string& description, double price, bool available = true)
        : id(id), name(name), category(category), description(description), price(price), available(available) {
    }

    std::string getId() const { return id; }
    std::string getName() const { return name; }
    std::string getCategory() const { return category; }
    std::string getDescription() const { return description; }
    double getPrice() const { return price; }
    bool isAvailable() const { return available; }

    void setPrice(double newPrice) { price = newPrice; }
    void setAvailable(bool status) { available = status; }
    void setDescription(const std::string& newDesc) { description = newDesc; }

    void display() const {
        std::cout << std::left << std::setw(5) << id
            << std::setw(25) << name
            << std::setw(15) << category
            << std::setw(40) << description
            << "$" << std::fixed << std::setprecision(2) << price
            << (available ? " (Available)" : " (Unavailable)") << std::endl;
    }
};

// Class for managing the menu
class Menu {
private:
    std::vector<MenuItem> items;

public:
    Menu() {
        // Initialize with some default menu items
        addItem(MenuItem("A001", "Crispy Spring Rolls", "Appetizer", "Vegetable spring rolls with sweet chili sauce", 8.99));
        addItem(MenuItem("A002", "Dumplings", "Appetizer", "Steamed pork and vegetable dumplings", 10.99));
        addItem(MenuItem("M001", "Pad Thai", "Main Course", "Stir-fried rice noodles with tamarind sauce", 16.99));
        addItem(MenuItem("M002", "Green Curry", "Main Course", "Coconut curry with vegetables and chicken", 18.99));
        addItem(MenuItem("D001", "Mango Sticky Rice", "Dessert", "Sweet sticky rice with fresh mango", 7.99));
        addItem(MenuItem("D002", "Green Tea Ice Cream", "Dessert", "Matcha flavored ice cream", 5.99));
        addItem(MenuItem("B001", "Thai Iced Tea", "Beverage", "Sweet milk tea with aromatic spices", 4.99));
    }

    void addItem(const MenuItem& item) {
        items.push_back(item);
    }

    void removeItem(const std::string& itemId) {
        auto it = std::find_if(items.begin(), items.end(),
            [&itemId](const MenuItem& item) { return item.getId() == itemId; });
        if (it != items.end()) {
            items.erase(it);
            std::cout << "Item removed successfully.\n";
        }
        else {
            std::cout << "Item not found.\n";
        }
    }

    void updateItemAvailability(const std::string& itemId, bool available) {
        auto it = std::find_if(items.begin(), items.end(),
            [&itemId](const MenuItem& item) { return item.getId() == itemId; });
        if (it != items.end()) {
            it->setAvailable(available);
            std::cout << "Item availability updated successfully.\n";
        }
        else {
            std::cout << "Item not found.\n";
        }
    }

    void updateItemPrice(const std::string& itemId, double newPrice) {
        auto it = std::find_if(items.begin(), items.end(),
            [&itemId](const MenuItem& item) { return item.getId() == itemId; });
        if (it != items.end()) {
            it->setPrice(newPrice);
            std::cout << "Item price updated successfully.\n";
        }
        else {
            std::cout << "Item not found.\n";
        }
    }

    // Non-const version for modifying menu items
    MenuItem* findItem(const std::string& itemId) {
        auto it = std::find_if(items.begin(), items.end(),
            [&itemId](const MenuItem& item) { return item.getId() == itemId; });
        if (it != items.end()) {
            return &(*it);
        }
        return nullptr;
    }

    // Const version for const contexts (like reporting)
    const MenuItem* findItem(const std::string& itemId) const {
        auto it = std::find_if(items.begin(), items.end(),
            [&itemId](const MenuItem& item) { return item.getId() == itemId; });
        if (it != items.end()) {
            return &(*it);
        }
        return nullptr;
    }

    void displayMenu() const {
        std::cout << "\n===== EATS & TREATS MENU =====\n";
        std::cout << std::left << std::setw(5) << "ID"
            << std::setw(25) << "Name"
            << std::setw(15) << "Category"
            << std::setw(40) << "Description"
            << "Price" << std::endl;
        std::cout << std::string(100, '-') << std::endl;

        for (const auto& item : items) {
            if (item.isAvailable()) {
                item.display();
            }
        }
    }

    void displayAllItems() const {
        std::cout << "\n===== ALL MENU ITEMS =====\n";
        std::cout << std::left << std::setw(5) << "ID"
            << std::setw(25) << "Name"
            << std::setw(15) << "Category"
            << std::setw(40) << "Description"
            << "Price" << std::endl;
        std::cout << std::string(100, '-') << std::endl;

        for (const auto& item : items) {
            item.display();
        }
    }

    std::vector<MenuItem> getItems() const {
        return items;
    }
};

// Class for order items
class OrderItem {
private:
    MenuItem* menuItem;
    int quantity;
    std::string specialInstructions;

public:
    OrderItem(MenuItem* menuItem, int quantity, const std::string& specialInstructions = "")
        : menuItem(menuItem), quantity(quantity), specialInstructions(specialInstructions) {
    }

    double getSubtotal() const { return menuItem->getPrice() * quantity; }
    MenuItem* getMenuItem() const { return menuItem; }
    int getQuantity() const { return quantity; }
    std::string getSpecialInstructions() const { return specialInstructions; }

    void display() const {
        std::cout << std::left << std::setw(5) << menuItem->getId()
            << std::setw(25) << menuItem->getName()
            << std::setw(10) << quantity
            << std::setw(30) << specialInstructions
            << "$" << std::fixed << std::setprecision(2) << getSubtotal() << std::endl;
    }
};

// Enum for order status
enum class OrderStatus { Pending, InProgress, Ready, Served, Completed, Cancelled };

// Utility function to convert order status to string
std::string orderStatusToString(OrderStatus status) {
    switch (status) {
    case OrderStatus::Pending: return "Pending";
    case OrderStatus::InProgress: return "In Progress";
    case OrderStatus::Ready: return "Ready";
    case OrderStatus::Served: return "Served";
    case OrderStatus::Completed: return "Completed";
    case OrderStatus::Cancelled: return "Cancelled";
    default: return "Unknown";
    }
}

// Class for orders
class Order {
private:
    std::string orderId;
    std::vector<OrderItem> items;
    OrderStatus status;
    std::string timestamp;
    int tableNumber;
    std::string waiterId;

public:
    Order(const std::string& orderId, int tableNumber, const std::string& waiterId)
        : orderId(orderId), status(OrderStatus::Pending), tableNumber(tableNumber), waiterId(waiterId) {
        timestamp = getCurrentDateTime();
    }

    void addItem(const OrderItem& item) {
        items.push_back(item);
    }

    void updateStatus(OrderStatus newStatus) {
        status = newStatus;
    }

    double getTotal() const {
        double total = 0.0;
        for (const auto& item : items) {
            total += item.getSubtotal();
        }
        return total;
    }

    // Getters
    std::string getOrderId() const { return orderId; }
    OrderStatus getStatus() const { return status; }
    std::string getTimestamp() const { return timestamp; }
    int getTableNumber() const { return tableNumber; }
    std::string getWaiterId() const { return waiterId; }
    const std::vector<OrderItem>& getItems() const { return items; }

    void display() const {
        std::cout << "Order ID: " << orderId << " | Table: " << tableNumber
            << " | Status: " << orderStatusToString(status) << " | Time: " << timestamp << std::endl;
        std::cout << std::string(80, '-') << std::endl;
        std::cout << std::left << std::setw(5) << "ID"
            << std::setw(25) << "Item"
            << std::setw(10) << "Quantity"
            << std::setw(30) << "Special Instructions"
            << "Subtotal" << std::endl;
        std::cout << std::string(80, '-') << std::endl;

        for (const auto& item : items) {
            item.display();
        }

        std::cout << std::string(80, '-') << std::endl;
        std::cout << "Total: $" << std::fixed << std::setprecision(2) << getTotal() << std::endl;
    }

    void generateBill() const {
        std::cout << "\n===== EATS & TREATS =====\n";
        std::cout << "Asian Fusion Cuisine\n";
        std::cout << "123 Downtown St, Auckland City\n";
        std::cout << "Phone: (09) 123-4567\n";
        std::cout << std::string(30, '-') << std::endl;
        std::cout << "Order: " << orderId << std::endl;
        std::cout << "Table: " << tableNumber << std::endl;
        std::cout << "Date: " << timestamp << std::endl;
        std::cout << std::string(30, '-') << std::endl;

        for (const auto& item : items) {
            std::cout << std::left << std::setw(20) << item.getMenuItem()->getName()
                << std::setw(5) << item.getQuantity() << "x"
                << "$" << std::fixed << std::setprecision(2) << item.getMenuItem()->getPrice()
                << " = $" << item.getSubtotal() << std::endl;
        }

        std::cout << std::string(30, '-') << std::endl;
        double subtotal = getTotal();
        double gst = subtotal * 0.15; // 15% GST
        double total = subtotal + gst;

        std::cout << std::left << std::setw(20) << "Subtotal:" << "$" << std::fixed << std::setprecision(2) << subtotal << std::endl;
        std::cout << std::left << std::setw(20) << "GST (15%):" << "$" << std::fixed << std::setprecision(2) << gst << std::endl;
        std::cout << std::string(30, '-') << std::endl;
        std::cout << std::left << std::setw(20) << "Total:" << "$" << std::fixed << std::setprecision(2) << total << std::endl;
        std::cout << std::string(30, '-') << std::endl;
        std::cout << "Thank you for dining at Eats & Treats!\n";
        std::cout << "We hope to see you again soon.\n";
    }
};

// Class for table management
class Table {
private:
    int tableNumber;
    int capacity;
    bool occupied;
    std::string reservation;

public:
    Table(int tableNumber, int capacity)
        : tableNumber(tableNumber), capacity(capacity), occupied(false), reservation("") {
    }

    // Getters and setters
    int getTableNumber() const { return tableNumber; }
    int getCapacity() const { return capacity; }
    bool isOccupied() const { return occupied; }
    std::string getReservation() const { return reservation; }

    void setOccupied(bool status) { occupied = status; }
    void setReservation(const std::string& reservationInfo) { reservation = reservationInfo; }

    void display() const {
        std::cout << "Table " << tableNumber << " | Capacity: " << capacity
            << " | Status: " << (occupied ? "Occupied" : "Available");

        if (!reservation.empty()) {
            std::cout << " | Reserved: " << reservation;
        }

        std::cout << std::endl;
    }
};

// Class for reservation
class Reservation {
private:
    std::string reservationId;
    std::string customerName;
    std::string contactNumber;
    std::string dateTime;
    int partySize;
    int tableNumber;
    bool confirmed;

public:
    Reservation(const std::string& reservationId, const std::string& customerName,
        const std::string& contactNumber, const std::string& dateTime,
        int partySize, int tableNumber)
        : reservationId(reservationId), customerName(customerName), contactNumber(contactNumber),
        dateTime(dateTime), partySize(partySize), tableNumber(tableNumber), confirmed(true) {
    }

    // Getters
    std::string getReservationId() const { return reservationId; }
    std::string getCustomerName() const { return customerName; }
    std::string getContactNumber() const { return contactNumber; }
    std::string getDateTime() const { return dateTime; }
    int getPartySize() const { return partySize; }
    int getTableNumber() const { return tableNumber; }
    bool isConfirmed() const { return confirmed; }

    void setConfirmed(bool status) { confirmed = status; }

    void display() const {
        std::cout << "Reservation ID: " << reservationId
            << " | Customer: " << customerName
            << " | Contact: " << contactNumber
            << " | Date/Time: " << dateTime
            << " | Party Size: " << partySize
            << " | Table: " << tableNumber
            << " | Status: " << (confirmed ? "Confirmed" : "Cancelled")
            << std::endl;
    }

    std::string getReservationInfo() const {
        return customerName + " (" + dateTime + ", " + std::to_string(partySize) + " people)";
    }
};

// Main restaurant management system class
class RestaurantSystem {
private:
    std::string restaurantName;
    Menu menu;
    std::vector<Table> tables;
    std::vector<Reservation> reservations;
    std::vector<Order> orders;
    std::vector<std::shared_ptr<User>> users;
    std::shared_ptr<User> currentUser;
    int nextOrderId;
    int nextReservationId;




private:
    // need to add a place to save the orderrs bro 

    void saveOrdersToFile() {
        // Save orders to a file for live monitoring
        std::vector<KitchenOrder> kitchenOrders;

        for (const auto& order : orders) {
            if (order.getStatus() == OrderStatus::Pending ||
                order.getStatus() == OrderStatus::InProgress) {

                KitchenOrder ko;
                ko.id = std::stoi(order.getOrderId().substr(1));
                ko.tableNumber = order.getTableNumber();
                ko.status = orderStatusToString(order.getStatus());

                // Create a summary of items
                std::string items;
                for (const auto& item : order.getItems()) {
                    items += item.getMenuItem()->getName() + " x" +
                        std::to_string(item.getQuantity()) + ", ";
                }
                if (!items.empty()) {
                    items.pop_back();
                    items.pop_back();
                }
                ko.itemList = items;

                kitchenOrders.push_back(ko);
            }
        }

        // Use FileManager to save the kitchen orders
        TwoCli::FileManager<KitchenOrder> orderManager("kitchen_orders.txt");
        orderManager.writeRecords(kitchenOrders);
    }

public:
    RestaurantSystem(const std::string& name)
        : restaurantName(name), nextOrderId(1), nextReservationId(1) {
        // Initialize tables
        for (int i = 1; i <= 10; ++i) {
            if (i <= 4) {
                tables.push_back(Table(i, 2)); // 2-seater tables
            }
            else if (i <= 8) {
                tables.push_back(Table(i, 4)); // 4-seater tables
            }
            else {
                tables.push_back(Table(i, 6)); // 6-seater tables
            }
        }

        // Initialize users
        users.push_back(std::make_shared<Host>("John", "H001"));
        users.push_back(std::make_shared<Waiter>("Emily", "W001"));
        users.push_back(std::make_shared<Chef>("Michael", "C001"));
        users.push_back(std::make_shared<Manager>("Lisa", "M001"));
    }

    void displayWelcome() const {
        std::cout << "\n=======================================\n";
        std::cout << "  Welcome to " << restaurantName << " Management System\n";
        std::cout << "=======================================\n";
    }

    bool login() {
        std::string userId;
        std::cout << "\nPlease enter your user ID (or 'exit' to quit): ";
        std::cin >> userId;

        if (userId == "exit") {
            return false;
        }

        auto it = std::find_if(users.begin(), users.end(),
            [&userId](const std::shared_ptr<User>& user) { return user->getId() == userId; });

        if (it != users.end()) {
            currentUser = *it;
            std::cout << "\nWelcome, " << currentUser->getName() << " (" << currentUser->getRole() << ")!\n";
            return true;
        }
        else {
            std::cout << "Invalid user ID. Please try again.\n";
            return login();
        }
    }

    void run() {
        displayWelcome();

        while (login()) {
            bool logout = false;

            while (!logout) {
                currentUser->displayMenu();
                int choice;
                std::cout << "\nEnter your choice: ";
                std::cin >> choice;

                // Clear input buffer
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (currentUser->getRole() == "Host") {
                    logout = handleHostMenu(choice);
                }
                else if (currentUser->getRole() == "Waiter") {
                    logout = handleWaiterMenu(choice);
                }
                else if (currentUser->getRole() == "Chef") {
                    logout = handleChefMenu(choice);
                }
                else if (currentUser->getRole() == "Manager") {
                    logout = handleManagerMenu(choice);
                }
            }
        }

        std::cout << "\nThank you for using " << restaurantName << " Management System!\n";
    }

    bool handleHostMenu(int choice) {
        switch (choice) {
        case 1:
            displayTables();
            break;
        case 2:
            assignTable();
            break;
        case 3:
            displayReservations();
            break;
        case 4:
            addReservation();
            break;
        case 5:
            cancelReservation();
            break;
        case 6:
            return true; // Return to login
        default:
            std::cout << "Invalid choice. Please try again.\n";
        }
        return false;
    }

    bool handleWaiterMenu(int choice) {
        switch (choice) {
        case 1:
            displayTables();
            break;
        case 2:
            takeOrder();
            break;
        case 3:
            viewOrderStatus();
            break;
        case 4:
            serveOrder();
            break;
        case 5:
            generateBill();
            break;
        case 6:
            return true; // Return to login
        default:
            std::cout << "Invalid choice. Please try again.\n";
        }
        return false;
    }

    //bool handleChefMenu(int choice) {
    //    switch (choice) {
    //    case 1:
    //        viewPendingOrders();
    //        break;
    //    case 2:
    //        updateOrderStatus();
    //        break;
    //    case 3:
    //        return true; // Return to login
    //    default:
    //        std::cout << "Invalid choice. Please try again.\n";
    //    }
    //    return false;
    //}

    //new 

    bool handleChefMenu(int choice) {
        switch (choice) {
        case 1:
            viewPendingOrders();
            break;
        case 2:
            updateOrderStatus();
            break;
        case 3:
            viewLiveOrders();  // Call the new live monitor function
            break;
        case 4:
            return true; // Return to login
        default:
            cout << "Invalid choice. Please try again.\n";
        }
        return false;
    }

    bool handleManagerMenu(int choice) {
        switch (choice) {
        case 1:
            displayTables();
            break;
        case 2:
            displayAllOrders();
            break;
        case 3:
            manageMenu();
            break;
        case 4:
            generateReports();
            break;
        case 5:
            manageStaff();
            break;
        case 6:
            return true; // Return to login
        default:
            std::cout << "Invalid choice. Please try again.\n";
        }
        return false;
    }

    void displayTables() const {
        std::cout << "\n===== TABLE STATUS =====\n";
        for (const auto& table : tables) {
            table.display();
        }
    }

    void assignTable() {
        displayTables();

        int tableNumber;
        std::cout << "\nEnter table number to assign: ";
        std::cin >> tableNumber;

        auto it = std::find_if(tables.begin(), tables.end(),
            [tableNumber](const Table& table) { return table.getTableNumber() == tableNumber; });

        if (it != tables.end()) {
            if (it->isOccupied()) {
                std::cout << "Table " << tableNumber << " is already occupied.\n";
            }
            else {
                std::string customerName;
                int partySize;

                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Enter customer name: ";
                std::getline(std::cin, customerName);

                std::cout << "Enter party size: ";
                std::cin >> partySize;

                if (partySize > it->getCapacity()) {
                    std::cout << "Party size exceeds table capacity. Please choose a larger table.\n";
                }
                else {
                    it->setOccupied(true);
                    std::cout << "Table " << tableNumber << " assigned to " << customerName << ".\n";
                }
            }
        }
        else {
            std::cout << "Invalid table number.\n";
        }
    }

    void displayReservations() const {
        std::cout << "\n===== RESERVATIONS =====\n";

        if (reservations.empty()) {
            std::cout << "No reservations found.\n";
            return;
        }

        for (const auto& reservation : reservations) {
            if (reservation.isConfirmed()) {
                reservation.display();
            }
        }
    }

    void addReservation() {
        std::string customerName, contactNumber, dateTime;
        int partySize;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Enter customer name: ";
        std::getline(std::cin, customerName);

        std::cout << "Enter contact number: ";
        std::getline(std::cin, contactNumber);

        std::cout << "Enter date and time (YYYY-MM-DD HH:MM): ";
        std::getline(std::cin, dateTime);

        std::cout << "Enter party size: ";
        std::cin >> partySize;

        // Find available table
        Table* suitableTable = nullptr;
        for (auto& table : tables) {
            if (table.getCapacity() >= partySize && !table.isOccupied() && table.getReservation().empty()) {
                suitableTable = &table;
                break;
            }
        }

        if (suitableTable == nullptr) {
            std::cout << "No suitable table available for this party size.\n";
            return;
        }

        std::string reservationId = "R" + std::to_string(nextReservationId++);
        reservations.push_back(Reservation(reservationId, customerName, contactNumber, dateTime, partySize, suitableTable->getTableNumber()));

        suitableTable->setReservation(customerName + " (" + dateTime + ")");

        std::cout << "Reservation created successfully. Reservation ID: " << reservationId << "\n";
    }

    void cancelReservation() {
        std::string reservationId;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Enter reservation ID to cancel: ";
        std::getline(std::cin, reservationId);

        auto it = std::find_if(reservations.begin(), reservations.end(),
            [&reservationId](const Reservation& reservation) {
                return reservation.getReservationId() == reservationId && reservation.isConfirmed();
            });

        if (it != reservations.end()) {
            it->setConfirmed(false);

            // Clear reservation from table
            for (auto& table : tables) {
                if (table.getTableNumber() == it->getTableNumber()) {
                    table.setReservation("");
                    break;
                }
            }

            std::cout << "Reservation cancelled successfully.\n";
        }
        else {
            std::cout << "Reservation not found or already cancelled.\n";
        }
    }

    void takeOrder() {
        displayTables();

        int tableNumber;
        std::cout << "\nEnter table number to take order: ";
        std::cin >> tableNumber;

        auto tableIt = std::find_if(tables.begin(), tables.end(),
            [tableNumber](const Table& table) { return table.getTableNumber() == tableNumber; });

        if (tableIt == tables.end()) {
            std::cout << "Invalid table number.\n";
            return;
        }

        if (!tableIt->isOccupied()) {
            std::cout << "Table " << tableNumber << " is not occupied.\n";
            return;
        }

        std::string orderId = "O" + std::to_string(nextOrderId++);
        Order order(orderId, tableNumber, currentUser->getId());

        menu.displayMenu();

        bool addingItems = true;
        while (addingItems) {
            std::string itemId;
            int quantity;
            std::string specialInstructions;

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "\nEnter item ID (or 'done' to finish): ";
            std::getline(std::cin, itemId);
            if (itemId == "done") {
                addingItems = false;
                continue;
            }

            MenuItem* menuItem = menu.findItem(itemId);

            if (menuItem == nullptr) {
                std::cout << "Item not found. Please try again.\n";
                continue;
            }

            if (!menuItem->isAvailable()) {
                std::cout << "This item is currently unavailable. Please choose another item.\n";
                continue;
            }

            std::cout << "Enter quantity: ";
            std::cin >> quantity;

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Enter special instructions (or press Enter for none): ";
            std::getline(std::cin, specialInstructions);

            OrderItem orderItem(menuItem, quantity, specialInstructions);
            order.addItem(orderItem);

            std::cout << "Item added to order.\n";
        }

        if (order.getItems().empty()) {
            std::cout << "Order is empty. No order created.\n";
        }
        else {
            orders.push_back(order);
            std::cout << "Order created successfully. Order ID: " << orderId << "\n";
            order.display();
        }

        saveOrdersToFile();
    }

    void viewOrderStatus() {
        std::string orderId;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Enter order ID to view status: ";
        std::getline(std::cin, orderId);

        auto it = std::find_if(orders.begin(), orders.end(),
            [&orderId](const Order& order) { return order.getOrderId() == orderId; });

        if (it != orders.end()) {
            std::cout << "Order Status: " << orderStatusToString(it->getStatus()) << "\n";
            it->display();
        }
        else {
            std::cout << "Order not found.\n";
        }
    }

    void serveOrder() {
        std::cout << "\n===== READY ORDERS =====\n";
        bool foundReadyOrders = false;

        for (const auto& order : orders) {
            if (order.getStatus() == OrderStatus::Ready) {
                std::cout << "Order ID: " << order.getOrderId() << " | Table: " << order.getTableNumber() << "\n";
                foundReadyOrders = true;
            }
        }

        if (!foundReadyOrders) {
            std::cout << "No orders ready to serve.\n";
            return;
        }

        std::string orderId;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\nEnter order ID to serve: ";
        std::getline(std::cin, orderId);

        auto it = std::find_if(orders.begin(), orders.end(),
            [&orderId](Order& order) {
                return order.getOrderId() == orderId && order.getStatus() == OrderStatus::Ready;
            });

        if (it != orders.end()) {
            it->updateStatus(OrderStatus::Served);
            std::cout << "Order " << orderId << " has been served to table " << it->getTableNumber() << ".\n";
        }
        else {
            std::cout << "Order not found or not ready to serve.\n";
        }
    }

    void generateBill() {
        int tableNumber;
        std::cout << "Enter table number to generate bill: ";
        std::cin >> tableNumber;

        auto tableIt = std::find_if(tables.begin(), tables.end(),
            [tableNumber](const Table& table) { return table.getTableNumber() == tableNumber; });

        if (tableIt == tables.end() || !tableIt->isOccupied()) {
            std::cout << "Invalid table number or table not occupied.\n";
            return;
        }

        std::vector<Order*> tableOrders;
        for (auto& order : orders) {
            if (order.getTableNumber() == tableNumber &&
                (order.getStatus() == OrderStatus::Served || order.getStatus() == OrderStatus::Ready)) {
                tableOrders.push_back(&order);
            }
        }

        if (tableOrders.empty()) {
            std::cout << "No active orders found for this table.\n";
            return;
        }

        std::cout << "\n===== BILL FOR TABLE " << tableNumber << " =====\n";
        double totalAmount = 0.0;

        for (auto& order : tableOrders) {
            order->generateBill();
            totalAmount += order->getTotal();
            order->updateStatus(OrderStatus::Completed);
        }

        // Free up the table
        tableIt->setOccupied(false);

        std::cout << "\nTotal bill amount: $" << std::fixed << std::setprecision(2) << totalAmount << "\n";
        std::cout << "Thank you for dining at " << restaurantName << "!\n";
    }

    void viewPendingOrders() {
        std::cout << "\n===== PENDING ORDERS =====\n";
        bool foundPendingOrders = false;

        for (const auto& order : orders) {
            if (order.getStatus() == OrderStatus::Pending) {
                order.display();
                std::cout << std::endl;
                foundPendingOrders = true;
            }
        }

        if (!foundPendingOrders) {
            std::cout << "No pending orders.\n";
        }
    }

    void updateOrderStatus() {
        std::cout << "\n===== UPDATE ORDER STATUS =====\n";
        std::cout << "Current orders:\n";

        for (const auto& order : orders) {
            if (order.getStatus() == OrderStatus::Pending || order.getStatus() == OrderStatus::InProgress) {
                std::cout << "Order ID: " << order.getOrderId()
                    << " | Table: " << order.getTableNumber()
                    << " | Status: " << orderStatusToString(order.getStatus())
                    << " | Items: " << order.getItems().size() << std::endl;
            }
        }

        std::string orderId;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\nEnter order ID to update: ";
        std::getline(std::cin, orderId);

        auto it = std::find_if(orders.begin(), orders.end(),
            [&orderId](Order& order) {
                return order.getOrderId() == orderId &&
                    (order.getStatus() == OrderStatus::Pending ||
                        order.getStatus() == OrderStatus::InProgress);
            });

        if (it != orders.end()) {
            std::cout << "Current status: " << orderStatusToString(it->getStatus()) << "\n";
            std::cout << "Select new status:\n";
            std::cout << "1. In Progress\n";
            std::cout << "2. Ready\n";

            int choice;
            std::cout << "Enter choice: ";
            std::cin >> choice;

            if (choice == 1) {
                it->updateStatus(OrderStatus::InProgress);
                std::cout << "Order status updated to In Progress.\n";
            }
            else if (choice == 2) {
                it->updateStatus(OrderStatus::Ready);
                std::cout << "Order status updated to Ready.\n";
            }
            else {
                std::cout << "Invalid choice.\n";
            }
        }
        else {
            std::cout << "Order not found or not updateable.\n";
        }
        saveOrdersToFile();

    }

    void displayAllOrders() const {
        std::cout << "\n===== ALL ORDERS =====\n";

        if (orders.empty()) {
            std::cout << "No orders found.\n";
            return;
        }

        for (const auto& order : orders) {
            order.display();
            std::cout << std::endl;
        }
    }
    // new 


    void viewLiveOrders() {
        std::cout << "Starting live order monitoring from file..." << std::endl;

        // Create a file manager for KitchenOrder
        TwoCli::FileManager<KitchenOrder> orderManager("kitchen_orders.txt");

        // Start the watch function
        orderManager.watch(5); // Refresh every 5 seconds
    }
    //


    void manageMenu() {
        std::cout << "\n===== MENU MANAGEMENT =====\n";
        std::cout << "1. View All Menu Items\n";
        std::cout << "2. Add New Menu Item\n";
        std::cout << "3. Update Item Availability\n";
        std::cout << "4. Update Item Price\n";
        std::cout << "5. Remove Menu Item\n";
        std::cout << "6. Return to Previous Menu\n";

        int choice;
        std::cout << "\nEnter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1:
            menu.displayAllItems();
            break;
        case 2:
            addMenuItem();
            break;
        case 3:
            updateItemAvailability();
            break;
        case 4:
            updateItemPrice();
            break;
        case 5:
            removeMenuItem();
            break;
        case 6:
            return;
        default:
            std::cout << "Invalid choice.\n";
        }
    }

    void addMenuItem() {
        std::string id, name, category, description;
        double price;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Enter item ID (e.g., A003, M003, D003): ";
        std::getline(std::cin, id);

        if (menu.findItem(id) != nullptr) {
            std::cout << "Item with this ID already exists.\n";
            return;
        }

        std::cout << "Enter item name: ";
        std::getline(std::cin, name);

        std::cout << "Enter category (Appetizer, Main Course, Dessert, Beverage): ";
        std::getline(std::cin, category);

        std::cout << "Enter description: ";
        std::getline(std::cin, description);

        std::cout << "Enter price: $";
        std::cin >> price;

        MenuItem newItem(id, name, category, description, price);
        menu.addItem(newItem);

        std::cout << "Menu item added successfully.\n";
    }

    void updateItemAvailability() {
        menu.displayAllItems();

        std::string itemId;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\nEnter item ID to update availability: ";
        std::getline(std::cin, itemId);

        MenuItem* item = menu.findItem(itemId);
        if (item == nullptr) {
            std::cout << "Item not found.\n";
            return;
        }

        std::cout << "Current availability: " << (item->isAvailable() ? "Available" : "Unavailable") << "\n";
        std::cout << "Set as available (1) or unavailable (0): ";

        int choice;
        std::cin >> choice;

        menu.updateItemAvailability(itemId, choice == 1);
    }

    void updateItemPrice() {
        menu.displayAllItems();

        std::string itemId;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\nEnter item ID to update price: ";
        std::getline(std::cin, itemId);

        MenuItem* item = menu.findItem(itemId);
        if (item == nullptr) {
            std::cout << "Item not found.\n";
            return;
        }

        std::cout << "Current price: $" << std::fixed << std::setprecision(2) << item->getPrice() << "\n";
        std::cout << "Enter new price: $";

        double newPrice;
        std::cin >> newPrice;

        menu.updateItemPrice(itemId, newPrice);
    }

    void removeMenuItem() {
        menu.displayAllItems();

        std::string itemId;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\nEnter item ID to remove: ";
        std::getline(std::cin, itemId);

        menu.removeItem(itemId);
    }

    void generateReports() {
        std::cout << "\n===== REPORTS =====\n";
        std::cout << "1. Daily Sales Report\n";
        std::cout << "2. Popular Items Report\n";
        std::cout << "3. Table Occupancy Report\n";
        std::cout << "4. Return to Previous Menu\n";

        int choice;
        std::cout << "\nEnter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1:
            generateDailySalesReport();
            break;
        case 2:
            generatePopularItemsReport();
            break;
        case 3:
            generateTableOccupancyReport();
            break;
        case 4:
            return;
        default:
            std::cout << "Invalid choice.\n";
        }
    }

    void generateDailySalesReport() const {
        // Get today's date
        time_t now = time(0);
        struct tm tstruct;
        char dateBuffer[11];
        tstruct = *localtime(&now);
        strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d", &tstruct);
        std::string today(dateBuffer);

        std::cout << "\n===== DAILY SALES REPORT: " << today << " =====\n";

        double totalSales = 0.0;
        int totalOrders = 0;

        for (const auto& order : orders) {
            if (order.getStatus() == OrderStatus::Completed && order.getTimestamp().substr(0, 10) == today) {
                totalSales += order.getTotal();
                totalOrders++;
            }
        }

        std::cout << "Total Orders: " << totalOrders << std::endl;
        std::cout << "Total Sales: $" << std::fixed << std::setprecision(2) << totalSales << std::endl;
        std::cout << "Average Order Value: $" << (totalOrders > 0 ? totalSales / totalOrders : 0) << std::endl;

        // Option to save report to file
        std::cout << "\nWould you like to save this report to a file? (y/n): ";
        char saveChoice;
        std::cin >> saveChoice;

        if (saveChoice == 'y' || saveChoice == 'Y') {
            std::string filename = "sales_report_" + today + ".txt";
            std::ofstream reportFile(filename);

            if (reportFile.is_open()) {
                reportFile << "===== EATS & TREATS DAILY SALES REPORT =====" << std::endl;
                reportFile << "Date: " << today << std::endl;
                reportFile << "Total Orders: " << totalOrders << std::endl;
                reportFile << "Total Sales: $" << std::fixed << std::setprecision(2) << totalSales << std::endl;
                reportFile << "Average Order Value: $" << (totalOrders > 0 ? totalSales / totalOrders : 0) << std::endl;

                reportFile.close();
                std::cout << "Report saved to " << filename << std::endl;
            }
            else {
                std::cout << "Error: Could not open file for writing." << std::endl;
            }
        }
    }

    void generatePopularItemsReport() const {
        std::cout << "\n===== POPULAR ITEMS REPORT =====\n";

        // Map to track item popularity
        std::map<std::string, int> itemCounts;

        // Count items across all completed orders
        for (const auto& order : orders) {
            if (order.getStatus() == OrderStatus::Completed) {
                for (const auto& item : order.getItems()) {
                    itemCounts[item.getMenuItem()->getId()] += item.getQuantity();
                }
            }
        }

        // Convert to vector for sorting
        std::vector<std::pair<std::string, int>> sortedItems(itemCounts.begin(), itemCounts.end());
        std::sort(sortedItems.begin(), sortedItems.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });

        // Display top items
        std::cout << "Top ordered items:" << std::endl;
        std::cout << std::left << std::setw(10) << "Rank"
            << std::setw(10) << "ID"
            << std::setw(30) << "Name"
            << "Quantity Ordered" << std::endl;
        std::cout << std::string(60, '-') << std::endl;

        int rank = 1;
        for (const auto& item : sortedItems) {
            if (rank > 10) break; // Show top 10

            const MenuItem* menuItem = menu.findItem(item.first);
            if (menuItem) {
                std::cout << std::left << std::setw(10) << rank
                    << std::setw(10) << menuItem->getId()
                    << std::setw(30) << menuItem->getName()
                    << item.second << std::endl;
            }
            rank++;
        }
    }

    void generateTableOccupancyReport() const {
        std::cout << "\n===== TABLE OCCUPANCY REPORT =====\n";

        int totalTables = static_cast<int>(tables.size()); // Fix for size_t to int conversion warning
        int occupiedTables = 0;
        int reservedTables = 0;

        for (const auto& table : tables) {
            if (table.isOccupied()) {
                occupiedTables++;
            }
            if (!table.getReservation().empty()) {
                reservedTables++;
            }
        }

        std::cout << "Total Tables: " << totalTables << std::endl;
        std::cout << "Currently Occupied: " << occupiedTables << " ("
            << (totalTables > 0 ? (occupiedTables * 100.0 / totalTables) : 0) << "%)" << std::endl;
        std::cout << "Reserved (Not Occupied): " << (reservedTables - occupiedTables) << std::endl;
        std::cout << "Available: " << (totalTables - occupiedTables) << " ("
            << (totalTables > 0 ? ((totalTables - occupiedTables) * 100.0 / totalTables) : 0) << "%)" << std::endl;

        std::cout << "\nDetailed Table Status:" << std::endl;
        for (const auto& table : tables) {
            table.display();
        }
    }

    void manageStaff() {
        std::cout << "\n===== STAFF MANAGEMENT =====\n";
        std::cout << "1. View All Staff\n";
        std::cout << "2. Add New Staff\n";
        std::cout << "3. Return to Previous Menu\n";

        int choice;
        std::cout << "\nEnter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1:
            displayAllStaff();
            break;
        case 2:
            addNewStaff();
            break;
        case 3:
            return;
        default:
            std::cout << "Invalid choice.\n";
        }
    }

    void displayAllStaff() const {
        std::cout << "\n===== STAFF LIST =====\n";
        std::cout << std::left << std::setw(10) << "ID"
            << std::setw(20) << "Name"
            << "Role" << std::endl;
        std::cout << std::string(40, '-') << std::endl;

        for (const auto& user : users) {
            std::cout << std::left << std::setw(10) << user->getId()
                << std::setw(20) << user->getName()
                << user->getRole() << std::endl;
        }
    }

    void addNewStaff() {
        std::string name, id, role;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Enter staff name: ";
        std::getline(std::cin, name);

        std::cout << "Enter staff ID: ";
        std::getline(std::cin, id);

        // Check if ID already exists
        auto it = std::find_if(users.begin(), users.end(),
            [&id](const std::shared_ptr<User>& user) { return user->getId() == id; });

        if (it != users.end()) {
            std::cout << "Error: Staff ID already exists.\n";
            return;
        }

        std::cout << "Enter role (Host, Waiter, Chef, Manager): ";
        std::getline(std::cin, role);

        if (role == "Host") {
            users.push_back(std::make_shared<Host>(name, id));
        }
        else if (role == "Waiter") {
            users.push_back(std::make_shared<Waiter>(name, id));
        }
        else if (role == "Chef") {
            users.push_back(std::make_shared<Chef>(name, id));
        }
        else if (role == "Manager") {
            users.push_back(std::make_shared<Manager>(name, id));
        }
        else {
            std::cout << "Invalid role. Please enter Host, Waiter, Chef, or Manager.\n";
            return;
        }

        std::cout << "Staff added successfully.\n";
    }
};

// Main function
int main() {
    RestaurantSystem restaurantSystem("Eats & Treats");
    restaurantSystem.run();
    return 0;
}