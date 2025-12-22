#include <iostream>
#include <string> //allow string data type in arrays
#include <iomanip> //for decimal places
using namespace std;

// MENU DATA (Arrays)

const int MAX_ITEMS = 11;
const int MAX_ORDERS = 10;

string itemNms[MAX_ITEMS] = {
    "Water", "Latte", "Mocha",
    "Burger", "Pasta", "Salad", "Pizza",
    "Cake", "Cookie", "Brownie", "Muffin"
};

double prcs[MAX_ITEMS] = {
    1.00, 4.50, 5.00,
    8.00, 7.50, 5.50, 12.00,
    4.50, 2.00, 3.00, 2.50
};

// FUNCTION DECLARATIONS

void showMenu();
void getOrder(int items[], int qty[], int &cnt);
double calcSub(int items[], int qty[], int cnt);
double discount(double st, bool student);
void printBill(int items[], int qty[], int cnt,
                   double st, double disc, double total);

int main() {
    cout << "===============================" << endl;
    cout << "    MY CAFE ORDERING SYSTEM    " << endl;
    cout << "===============================" << endl;

    int items[MAX_ORDERS];
    int qty[MAX_ORDERS];
    int cnt = 0;

    bool student = false;
    bool ordered = false;

    int choice;
    while (true) {
        cout << "\n========== MAIN MENU ==========" << endl;
        cout << "1. View Menu" << endl;
        cout << "2. Order Items" << endl;
        cout << "3. View Bill" << endl;
        cout << "4. Apply Student Discount" << endl;
        cout << "5. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                showMenu();
                break;
            case 2:
                getOrder(items, qty, cnt);
                ordered = true;
                break;
            case 3:
                if (ordered && cnt > 0) {
                    double st = calcSub(items, qty, cnt);
                    double disc = discount(st, student);
                    double total = st - disc;
                    printBill(items, qty, cnt, st, disc, total);
                } else {
                    cout << "\nNo orders found!" << endl;
                }
                break;
            case 4:
                student = true;
                cout << "\nStudent discount applied!" << endl;
                break;
            case 5:
                cout << "\nThank you for coming!" << endl;
                return 0;
            default:
                cout << "\nInvalid choice!" << endl;
        }
    }
    return 0;
}

void showMenu() {
    cout << fixed << setprecision(2);   //for decimal places
    cout << "\nNo.\tItem\t\tPrice" << endl; // \t to indent
    cout << "--------------------------------" << endl;
    for (int i = 0; i < MAX_ITEMS; i++) {
        cout << (i + 1) << "\t" << itemNms[i] << "\t\t" << prcs[i] << "$" << endl; // \t to indent
        if (i == 2 || i == 7) cout << "--------------------------------" << endl; // \t to indent
    }
}

void getOrder(int items[], int qty[], int &cnt) {
    char cont = 'Y';
    while (cont == 'Y' || cont == 'y') {
        if (cnt >= MAX_ORDERS) {
            cout << "\nOrder limit reached!" << endl;
            break;
        }

        int itemNum, amt;
        cout << "\nEnter item number: ";
        cin >> itemNum;

        if (itemNum < 1 || itemNum > MAX_ITEMS) {
            cout << "Invalid item!" << endl;
            continue;
        }

        // Logic check: itemNum-1 is used to match the 0-based array index
        cout << "How many " << itemNms[itemNum - 1] << " do you want: ";
        cin >> amt;

        items[cnt] = itemNum - 1;
        qty[cnt] = amt;
        cnt++;

        cout << "\nAdded to cart." << endl;
        cout << "Order more? (Y/N): ";
        cin >> cont;
    }
}

double calcSub(int items[], int qty[], int cnt) {
    double st = 0.0;
    for (int i = 0; i < cnt; i++) {
        st += prcs[items[i]] * qty[i];
    }
    return st;
}

double discount(double st, bool student) {
    double disc = 0.0;
    if (student)
        {
            disc += st * 0.10;
        }
    if (st > 30.00)
        {
            disc += st * 0.05;
        }
    return disc;
}

void printBill(int items[], int qty[], int cnt,
                   double st, double disc, double total) {
    cout << "\n--- FINAL RECEIPT ---" << endl;
    if (st > 30.00){
        cout << "Congrats you had 5% off!" << endl;
    }
    cout << "Item\t\tQty\tTotal" << endl;
    for (int i = 0; i < cnt; i++) {
        int idx = items[i];
        cout << itemNms[idx] << "\t" << qty[i] << "\t" << (prcs[idx] * qty[i]) << "$" << endl;
    }
    cout << "--------------------------------" << endl;
    cout << "Subtotal: " << st << "$" << endl;
    cout << "Discount: -" << disc << "$" << endl;
    cout << "Total: " << total << "$" << endl;
}
