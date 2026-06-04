#include "ConsoleUI.h"
#include "Account.h"
#include "Transaction.h"
#include "AuditRule.h"
#include <iostream>
#include <sstream>
#include <vector>

// ===== Helpers =====
static std::string readNonEmpty(const std::string& prompt) {
    std::string val;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, val);
        if (!val.empty()) return val;
        std::cout << "  >> Cannot be empty. Try again.\n";
    }
}

static double readPositiveDouble(const std::string& prompt, bool allowZero = false) {
    while (true) {
        std::cout << prompt;
        std::string s; std::getline(std::cin, s);
        try {
            double v = std::stod(s);
            if (allowZero ? v >= 0 : v > 0) return v;
            std::cout << "  >> Must be " << (allowZero ? ">= 0" : "> 0") << ". Try again.\n";
        } catch (...) {
            std::cout << "  >> Invalid number. Try again.\n";
        }
    }
}

static int readIntInRange(const std::string& prompt, int lo, int hi) {
    while (true) {
        std::cout << prompt;
        std::string s; std::getline(std::cin, s);
        try {
            int v = std::stoi(s);
            if (v >= lo && v <= hi) return v;
            std::cout << "  >> Must be between " << lo << " and " << hi << ". Try again.\n";
        } catch (...) {
            std::cout << "  >> Invalid number. Try again.\n";
        }
    }
}

void ConsoleUI::displayMenu() const {
    std::cout << "\n===== FINANCIAL AUDIT SYSTEM (Interactive) =====\n"
              << "1. Add Account\n"
              << "2. Add Transaction\n"
              << "3. View Accounts\n"
              << "4. View Transactions\n"
              << "5. Select & Run Audit Rules\n"
              << "6. Export Report\n"
              << "7. Help & Guide\n"
              << "0. Exit\n"
              << "Choice (0-7): ";
}

void ConsoleUI::showHelp() const {
    std::cout << "\n========== HELP & INPUT GUIDE ==========\n\n"
              << "1. ADD ACCOUNT\n"
              << "   Type    : CORP or RETAIL (exact, case-sensitive)\n"
              << "   ID      : unique, no spaces (e.g. ACC001)\n"
              << "   Balance : number >= 0\n"
              << "   CORP    : Tax Code + Company Type required\n"
              << "   RETAIL  : ID Card Number required\n\n"
              << "2. ADD TRANSACTION\n"
              << "   Type    : TRANSFER or INTL (exact, case-sensitive)\n"
              << "   ID      : unique, no spaces (e.g. TX001)\n"
              << "   Amount  : number > 0\n"
              << "   Sender / Receiver: must exist in system, cannot be the same\n"
              << "   Date    : Day 1-31, Month 1-12, Year >= 2000\n"
              << "   INTL    : Source != Target currency, Rate > 0\n\n"
              << "5. AUDIT RULES\n"
              << "   1 = LargeAmountRule  (> 1,000,000,000 VND)\n"
              << "   2 = SmurfingRule     (>= 3 txns < 5,000,000 VND same day)\n"
              << "   3 = SanctionsRule    (blacklist: ACC_BAD, ACC_OFAC)\n"
              << "   4 = GeoAnomalyRule   (high-risk: NorthKorea, Iran)\n"
              << "   Enter: '1 2 3 4' to run all\n"
              << "========================================\n\n";
}

void ConsoleUI::addAccountInteractive() {
    std::cout << "\n=== Add Account ===\n";

    std::string type;
    while (true) {
        std::cout << "Type (CORP or RETAIL)? ";
        std::getline(std::cin, type);
        if (type == "CORP" || type == "RETAIL") break;
        std::cout << "  >> Must be exactly CORP or RETAIL. Try again.\n";
    }

    std::string id;
    while (true) {
        std::cout << "Account ID (e.g. ACC001)? ";
        std::getline(std::cin, id);
        if (id.empty())                          { std::cout << "  >> Cannot be empty.\n"; continue; }
        if (id.find(' ') != std::string::npos)   { std::cout << "  >> No spaces allowed.\n"; continue; }
        if (db.getAccountByID(id))               { std::cout << "  >> ID already exists.\n"; continue; }
        break;
    }

    std::string owner   = readNonEmpty("Owner Name? ");
    double      balance = readPositiveDouble("Balance (>= 0)? ", true);

    if (type == "CORP") {
        std::string tax     = readNonEmpty("Tax Code (e.g. TAX12345)? ");
        std::string company = readNonEmpty("Company Type (e.g. LLC)? ");
        db.addAccount(new CorporateAccount(id, owner, balance, tax, company));
    } else {
        std::string idcard = readNonEmpty("ID Card Number (e.g. CMND987654)? ");
        db.addAccount(new RetailAccount(id, owner, balance, idcard));
    }

    // Auto-save vao CSV
    if (fileMgr.saveAccounts(db.getAccounts(), "accounts.csv"))
        std::cout << ">> Account '" << id << "' added and saved to accounts.csv!\n";
    else
        std::cout << ">> Account added (warning: could not save to file).\n";
}

void ConsoleUI::addTransactionInteractive() {
    std::cout << "\n=== Add Transaction ===\n";

    std::string type;
    while (true) {
        std::cout << "Type (TRANSFER or INTL)? ";
        std::getline(std::cin, type);
        if (type == "TRANSFER" || type == "INTL") break;
        std::cout << "  >> Must be exactly TRANSFER or INTL. Try again.\n";
    }

    std::string id;
    while (true) {
        std::cout << "Transaction ID (e.g. TX001)? ";
        std::getline(std::cin, id);
        if (id.empty())                        { std::cout << "  >> Cannot be empty.\n"; continue; }
        if (id.find(' ') != std::string::npos) { std::cout << "  >> No spaces allowed.\n"; continue; }
        break;
    }

    double amount = readPositiveDouble("Amount (> 0)? ");

    std::string from;
    while (true) {
        std::cout << "Sender Account ID? ";
        std::getline(std::cin, from);
        if (from.empty())              { std::cout << "  >> Cannot be empty.\n"; continue; }
        if (!db.getAccountByID(from))  { std::cout << "  >> Account '" << from << "' not found.\n"; continue; }
        break;
    }

    std::string to;
    while (true) {
        std::cout << "Receiver Account ID? ";
        std::getline(std::cin, to);
        if (to.empty())              { std::cout << "  >> Cannot be empty.\n"; continue; }
        if (!db.getAccountByID(to))  { std::cout << "  >> Account '" << to << "' not found.\n"; continue; }
        if (to == from)              { std::cout << "  >> Sender and receiver cannot be the same.\n"; continue; }
        break;
    }

    int day   = readIntInRange("Day   (1-31)?   ", 1, 31);
    int month = readIntInRange("Month (1-12)?   ", 1, 12);
    int year  = readIntInRange("Year  (>= 2000)? ", 2000, 9999);

    std::string ip      = readNonEmpty("IP Address (e.g. 192.168.1.1)? ");
    std::string country = readNonEmpty("Country    (e.g. Vietnam)?     ");
    std::string city    = readNonEmpty("City       (e.g. Hanoi)?       ");

    Date     dt{day, month, year};
    Location loc{ip, country, city};

    if (type == "INTL") {
        std::string src = readNonEmpty("Source Currency (e.g. USD)? ");
        std::string tgt;
        while (true) {
            tgt = readNonEmpty("Target Currency (e.g. VND)? ");
            if (tgt == src) { std::cout << "  >> Source and target currency cannot be the same.\n"; continue; }
            break;
        }
        std::cout << "  Tip: " << src << " -> " << tgt << "\n"
                  << "       Major currency -> VND: enter rate ~24000-26000\n"
                  << "       VND -> Major currency: enter rate ~0.000041\n";
        double      rate  = readPositiveDouble("Exchange Rate (> 0)? ");
        std::string swift = readNonEmpty("SWIFT Code (e.g. BFTVVNVX)? ");
        db.addTransaction(new InternationalTransfer(id, amount, from, to, dt, loc,
                                                    src, tgt, rate, swift));
    } else {
        std::string bank = readNonEmpty("Bank Network (e.g. NAPAS)? ");
        double      fee  = readPositiveDouble("Fee (>= 0)? ", true);
        db.addTransaction(new TransferTransaction(id, amount, from, to, dt, loc, bank, fee));
    }

    // Auto-save vao CSV
    if (fileMgr.saveTransactions(db.getTransactions(), "transactions.csv"))
        std::cout << ">> Transaction '" << id << "' added and saved to transactions.csv!\n";
    else
        std::cout << ">> Transaction added (warning: could not save to file).\n";
}

void ConsoleUI::viewAccountsInteractive() const {
    std::cout << "\n=== Accounts (" << db.getAccounts().size() << " total) ===\n";
    if (db.getAccounts().empty()) { std::cout << "(No accounts yet)\n"; return; }
    for (auto* a : db.getAccounts()) a->printDetails();
}

void ConsoleUI::viewTransactionsInteractive() const {
    std::cout << "\n=== Transactions (" << db.getTransactions().size() << " total) ===\n";
    if (db.getTransactions().empty()) { std::cout << "(No transactions yet)\n"; return; }
    for (auto* t : db.getTransactions()) std::cout << t->getDetails() << "\n";
}

void ConsoleUI::selectRulesInteractive() {
    engine = AuditEngine();
    std::cout << "\n=== Select Audit Rules ===\n"
              << "  1. LargeAmountRule  (> 1,000,000,000 VND)\n"
              << "  2. SmurfingRule     (>= 3 txns < 5,000,000 VND/day)\n"
              << "  3. SanctionsRule    (blacklist check)\n"
              << "  4. GeoAnomalyRule   (high-risk countries)\n"
              << "Enter numbers separated by space (e.g. '1 2 3 4'): ";

    std::string input; std::getline(std::cin, input);
    if (input.empty()) { std::cout << "ERROR: Must select at least 1 rule.\n"; return; }

    std::istringstream iss(input);
    int choice, count = 0;
    while (iss >> choice) {
        if      (choice == 1) { engine.addRule(new LargeAmountRule(1000000000)); count++; }
        else if (choice == 2) { engine.addRule(new SmurfingRule(3, 5000000));    count++; }
        else if (choice == 3) { engine.addRule(new SanctionsRule({"ACC_BAD", "ACC_OFAC"})); count++; }
        else if (choice == 4) { engine.addRule(new GeoAnomalyRule({"NorthKorea", "Iran"})); count++; }
        else    std::cout << "  >> Unknown rule '" << choice << "' skipped.\n";
    }
    if (count == 0) { std::cout << "ERROR: No valid rules selected.\n"; return; }
    std::cout << ">> " << count << " rule(s) selected! Running audit...\n";

    auto results = engine.runAudit(db);
    std::cout << "\n=== Audit Results (" << results.size() << " warnings) ===\n";
    if (results.empty()) {
        std::cout << "No violations found!\n";
    } else {
        for (const auto& r : results)
            std::cout << "[" << severityToStr(r.severity) << "] Tx " << r.transactionID
                      << " | " << r.ruleViolated << " | " << r.notes << "\n";
    }
}

void ConsoleUI::exportReportInteractive() {
    auto results = engine.runAudit(db);
    if (results.empty()) {
        std::cout << ">> No audit results. Run audit first (option 5).\n"; return;
    }
    if (fileMgr.exportReport(results, "audit_report.txt")) {
        std::cout << ">> Report exported to 'audit_report.txt'\n"
                  << "   Total warnings: " << results.size() << "\n";
    } else {
        std::cout << "ERROR: Could not write report file.\n";
    }
}

void ConsoleUI::run() {
    std::cout << "\n========================================\n"
              << "  FINANCIAL AUDIT SYSTEM (Interactive)\n"
              << "  Type '7' anytime for Help & Guide\n"
              << "========================================\n";
    std::cout << "Load demo data from CSV? (y/n): ";
    std::string ans; std::getline(std::cin, ans);
    if (ans == "y" || ans == "Y") {
        for (auto* a : fileMgr.loadAccounts("accounts.csv"))        db.addAccount(a);
        for (auto* t : fileMgr.loadTransactions("transactions.csv")) db.addTransaction(t);
        std::cout << ">> Loaded " << db.getAccounts().size()
                  << " accounts, " << db.getTransactions().size() << " transactions.\n";
    }

    while (running) {
        displayMenu();
        std::string choice_str;
        if (!std::getline(std::cin, choice_str) || choice_str.empty()) continue;
        try {
            int choice = std::stoi(choice_str);
            switch (choice) {
                case 1: addAccountInteractive();       break;
                case 2: addTransactionInteractive();   break;
                case 3: viewAccountsInteractive();     break;
                case 4: viewTransactionsInteractive(); break;
                case 5: selectRulesInteractive();      break;
                case 6: exportReportInteractive();     break;
                case 7: showHelp();                    break;
                case 0: running = false; std::cout << "\n>> Goodbye!\n"; break;
                default: std::cout << "ERROR: Invalid choice. Enter 0-7.\n";
            }
        } catch (...) {
            std::cout << "ERROR: Please enter a number (0-7).\n";
        }
    }
}