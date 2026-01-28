#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>

using namespace std;

/*
    Cafe Ordering System (OOP + File Handling)

    - Loads menu from a file (menu.csv). Assumes the file exists (user-provided).
    - Allows customer to:
        1) View menu
        2) Add items to order
        3) View bill / receipt
        4) Apply student discount
        5) Save receipt to file
        6) Exit

    Files:
      - menu.csv       : menu items (id,name,category,price)
      - orders.txt     : saved receipts history
*/

static const string MENU_FILE  = "menu.csv";
static const string ORDER_FILE = "orders.txt";

static string nowString() {
    time_t t = time(nullptr);
    tm *lt = localtime(&t);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", lt);
    return string(buf);
}


class MenuItem {
private:
    int id_;
    string name_;
    string category_;
    double price_;
public:
    MenuItem() : id_(0), price_(0.0) {}
    MenuItem(int id, const string& name, const string& category, double price)
        : id_(id), name_(name), category_(category), price_(price) {}

    int id() const { return id_; }
    const string& name() const { return name_; }
    const string& category() const { return category_; }
    double price() const { return price_; }

    // CSV: id,name,category,price
    static bool fromCSV(const string& line, MenuItem& out) {
        stringstream ss(line);
        string sId, name, cat, sPrice;
        if (!getline(ss, sId, ',')) return false;
        if (!getline(ss, name, ',')) return false;
        if (!getline(ss, cat, ',')) return false;
        if (!getline(ss, sPrice, ',')) return false;

        try {
            int id = stoi(sId);
            double price = stod(sPrice);
            out = MenuItem(id, name, cat, price);
            return true;
        } catch (...) {
            return false;
        }
    }

    string toCSV() const {
        ostringstream os;
        os << id_ << "," << name_ << "," << category_ << "," << fixed << setprecision(2) << price_;
        return os.str();
    }
};

class OrderLine {
private:
    MenuItem item_;
    int qty_;
public:
    OrderLine(const MenuItem& item, int qty) : item_(item), qty_(qty) {}
    const MenuItem& item() const { return item_; }
    int qty() const { return qty_; }
    void addQty(int q) { qty_ += q; }
    double lineTotal() const { return item_.price() * qty_; }
};

class Order {
private:
    vector<OrderLine> lines_;
    bool studentDiscountApplied_ = false;

public:
    void clear() {
        lines_.clear();
        studentDiscountApplied_ = false;
    }

    bool empty() const { return lines_.empty(); }

    void setStudentDiscount(bool enabled) { studentDiscountApplied_ = enabled; }
    bool studentDiscount() const { return studentDiscountApplied_; }

    void addItem(const MenuItem& item, int qty) {
        // If same item already exists in order, just increase quantity
        for (auto &ln : lines_) {
            if (ln.item().id() == item.id()) {
                ln.addQty(qty);
                return;
            }
        }
        lines_.push_back(OrderLine(item, qty));
    }

    double subtotal() const {
        double s = 0.0;
        for (const auto &ln : lines_) s += ln.lineTotal();
        return s;
    }

    double discountAmount() const {
        // Example discounts:
        // - Student discount: 10%
        // - "Spend > 30" promo: 5% (stacking rules can be changed)
        double s = subtotal();

        double disc = 0.0;
        if (studentDiscountApplied_) disc += 0.10 * s;
        if (s > 30.0) disc += 0.05 * s; // promo

        return disc;
    }

    double total() const {
        double s = subtotal();
        return s - discountAmount();
    }

    string receiptText() const {
        ostringstream out;
        out << "==============================\n";
        out << "       MY CAFE RECEIPT        \n";
        out << "==============================\n";
        out << "Time: " << nowString() << "\n\n";

        out << left << setw(4) << "ID"
            << left << setw(18) << "Item"
            << right << setw(6) << "Qty"
            << right << setw(10) << "Price"
            << right << setw(12) << "Total" << "\n";
        out << "------------------------------------------------\n";

        for (const auto &ln : lines_) {
            out << left << setw(4) << ln.item().id()
                << left << setw(18) << ln.item().name().substr(0, 17)
                << right << setw(6) << ln.qty()
                << right << setw(10) << fixed << setprecision(2) << ln.item().price()
                << right << setw(12) << fixed << setprecision(2) << ln.lineTotal()
                << "\n";
        }

        out << "------------------------------------------------\n";
        out << right << setw(28) << "Subtotal: " << setw(10) << fixed << setprecision(2) << subtotal() << "\n";
        out << right << setw(28) << "Discount: " << setw(10) << fixed << setprecision(2) << discountAmount() << "\n";
        out << right << setw(28) << "Total: "    << setw(10) << fixed << setprecision(2) << total() << "\n";

        if (studentDiscountApplied_) out << "Student discount applied (10%)\n";
        if (subtotal() > 30.0) out << "Promo applied: Spend > 30 (5%)\n";
        out << "==============================\n";

        return out.str();
    }
};

class CafeSystem {
private:
    vector<MenuItem> menu_;
    Order currentOrder_;

    const MenuItem* findMenuById(int id) const {
        for (const auto &it : menu_) {
            if (it.id() == id) return &it;
        }
        return nullptr;
    }

    bool loadMenu() {
        ifstream fin(MENU_FILE);

        menu_.clear();

        string line;
        bool firstLine = true;
        while (getline(fin, line)) {
            if (line.empty()) continue;

            // skip header if present
            if (firstLine) {
                firstLine = false;
                string lower = line;
                for (auto &c : lower) c = (char)tolower(c);
                if (lower.find("id,") == 0) continue;
            }

            MenuItem m;
            if (MenuItem::fromCSV(line, m)) menu_.push_back(m);
        }
        return !menu_.empty();
    }

    void showMenu() const {
        cout << "\n========== MENU ==========\n";
        cout << left << setw(4) << "ID"
             << left << setw(18) << "Item"
             << left << setw(10) << "Category"
             << right << setw(8) << "Price\n";
        cout << "----------------------------------------\n";
        for (const auto &m : menu_) {
            cout << left << setw(4) << m.id()
                 << left << setw(18) << m.name().substr(0, 17)
                 << left << setw(10) << m.category().substr(0, 9)
                 << right << setw(8) << fixed << setprecision(2) << m.price()
                 << "\n";
        }
        cout << "----------------------------------------\n";
        cout << "Tip: Menu is loaded from '" << MENU_FILE << "'. You can edit it.\n";
    }

    void addOrderFlow() {
        while (true) {
            showMenu();
            cout << "\nEnter item ID to add (0 to stop): ";
            int id;
            cin >> id;
            if (id == 0) break;

            const MenuItem* mi = findMenuById(id); // assumed found
            cout << "Enter quantity: ";
            int q;
            cin >> q; // assumed >= 1

            currentOrder_.addItem(*mi, q);
            cout << "Added: " << mi->name() << " x" << q << "\n";
        }
    }

void viewBill() const {
        if (currentOrder_.empty()) {
            cout << "\nNo orders found.\n";
            return;
        }
        cout << "\n" << currentOrder_.receiptText();
    }

    void applyStudentDiscount() {
        if (currentOrder_.empty()) {
            cout << "\nNo order yet. Please order items first.\n";
            return;
        }

        if (currentOrder_.studentDiscount()) {
            cout << "Student discount already applied.\n";
            return;
        }

        cout << "Apply student discount (10%)? (Y/N): ";
        char c;
        cin >> c;
        c = (char)toupper(c);
        if (c == 'Y') {
            currentOrder_.setStudentDiscount(true);
            cout << "Student discount applied.\n";
        } else {
            cout << "Student discount not applied.\n";
        }
    }

    void saveReceiptToFile() {
        if (currentOrder_.empty()) {
            cout << "\nNo order to save.\n";
            return;
        }

        ofstream fout(ORDER_FILE, ios::app);
        if (!fout.is_open()) {
            cout << "Failed to open '" << ORDER_FILE << "' for writing.\n";
            return;
        }
        fout << currentOrder_.receiptText() << "\n";
        cout << "Receipt saved to '" << ORDER_FILE << "'.\n";
    }

    void viewSavedReceipts() const {
        ifstream fin(ORDER_FILE);
        if (!fin.is_open()) {
            cout << "No saved receipts found yet.\n";
            return;
        }
        cout << "\n========== SAVED RECEIPTS (History) ==========\n";
        cout << fin.rdbuf();
        cout << "=============================================\n";
    }

public:
    bool init() {
        bool ok = loadMenu();
        if (!ok) {
            cout << "ERROR: Could not load menu from '" << MENU_FILE << "'.\n";
        }
        return ok;
    }

    void run() {
        cout << "===============================\n";
        cout << "    MY CAFE ORDERING SYSTEM    \n";
        cout << "===============================\n" << flush;

        int choice;
        while (true) {
            cout << "\n========== MAIN MENU ==========\n";
            cout << "1. View Menu\n";
            cout << "2. Order Items\n";
            cout << "3. View Bill\n";
            cout << "4. Apply Student Discount\n";
            cout << "5. Save Receipt (File)\n";
            cout << "6. View Saved Receipts\n";
            cout << "7. Clear Current Order\n";
            cout << "8. Exit\n";
            cout << "Enter choice: " << flush;
            cin >> choice;

            switch (choice) {
                case 1: showMenu(); break;
                case 2: addOrderFlow(); break;
                case 3: viewBill(); break;
                case 4: applyStudentDiscount(); break;
                case 5: saveReceiptToFile(); break;
                case 6: viewSavedReceipts(); break;
                case 7: currentOrder_.clear(); cout << "Current order cleared.\n"; break;
                case 8: cout << "Thank you. Bye!\n"; return;
            }
        }
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(&cout);

    CafeSystem app;
    app.init();
    app.run();
    return 0;
}
