#include "ConsoleUI.h"
#include "Account.h"
#include "Transaction.h"
#include "AuditRule.h"
#include <iostream>
#include <sstream>
#include <vector>

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
    std::cout << "\n========== HELP & INPUT GUIDE ==========\n\n";
    
    std::cout << "1. ADD ACCOUNT\n"
              << "   RETAIL: ID (ACC001), Owner Name, Balance (>= 0), ID Card\n"
              << "   CORP:   ID (ACC001), Owner Name, Balance (>= 0), Tax Code, Company Type\n\n";
    
    std::cout << "2. ADD TRANSACTION\n"
              << "   TRANSFER: ID (TX001), Amount (> 0), Sender (ACC001), Receiver (ACC002),\n"
              << "             Date (day 1-31, month 1-12, year >= 2000),\n"
              << "             IP, Country, City, Bank Network, Fee\n"
              << "   INTL:     Same as TRANSFER + Source Currency, Target Currency,\n"
              << "             Exchange Rate (> 0), SWIFT Code\n\n";
    
    std::cout << "3. VIEW ACCOUNTS\n"
              << "   Display all accounts (CORP & RETAIL) in system\n\n";
    
    std::cout << "4. VIEW TRANSACTIONS\n"
              << "   Display all transactions (TRANSFER & INTL) in system\n\n";
    
    std::cout << "5. SELECT & RUN AUDIT RULES\n"
              << "   1 = LargeAmountRule (> 1,000,000)\n"
              << "   2 = SmurfingRule (>= 3 small txns/day)\n"
              << "   3 = SanctionsRule (blacklist check)\n"
              << "   4 = GeoAnomalyRule (high-risk countries)\n"
              << "   Enter space-separated numbers: '1 2 3 4'\n\n";
    
    std::cout << "6. EXPORT REPORT\n"
              << "   Save audit results to 'audit_report.txt'\n\n";
    
    std::cout << "CONSTRAINTS:\n"
              << "- Account/Transaction IDs: no spaces, unique preferred\n"
              << "- Balance/Amount: numeric, Balance >= 0, Amount > 0\n"
              << "- Date: valid (1-31 day, 1-12 month, year >= 2000)\n"
              << "- Sender/Receiver: must exist as Account ID\n\n";
    
    std::cout << "EXAMPLE WORKFLOW:\n"
              << "1. Load demo (y) -> 2. View Accounts (3) -> 3. Select Rules (5)\n"
              << "4. View Results -> 5. Export (6) -> 6. Exit (0)\n"
              << "========================================\n\n";
}

void ConsoleUI::addAccountInteractive() {
    std::string type, id, owner, bal_str;
    std::cout << "\n=== Add Account ===\n";
    std::cout << "Type (CORP or RETAIL)? ";
    std::getline(std::cin, type);
    if (type != "CORP" && type != "RETAIL") {
        std::cout << "ERROR: Type must be CORP or RETAIL\n"; return;
    }
    
    std::cout << "Account ID (format: ACC + number, e.g., ACC001)? ";
    std::getline(std::cin, id);
    if (id.empty()) { std::cout << "ERROR: ID cannot be empty\n"; return; }
    
    std::cout << "Owner Name (e.g., Acme Corp)? ";
    std::getline(std::cin, owner);
    if (owner.empty()) { std::cout << "ERROR: Name cannot be empty\n"; return; }
    
    std::cout << "Balance (numeric, >= 0, e.g., 5000000)? ";
    std::getline(std::cin, bal_str);
    
    try {
        double balance = std::stod(bal_str);
        if (balance < 0) { std::cout << "ERROR: Balance must be >= 0\n"; return; }
        
        if (type == "CORP") {
            std::string tax, company;
            std::cout << "Tax Code (e.g., TAX12345)? ";
            std::getline(std::cin, tax);
            std::cout << "Company Type (e.g., LLC)? ";
            std::getline(std::cin, company);
            db.addAccount(new CorporateAccount(id, owner, balance, tax, company));
        } else {
            std::string idcard;
            std::cout << "ID Card Number (e.g., CMND987654)? ";
            std::getline(std::cin, idcard);
            db.addAccount(new RetailAccount(id, owner, balance, idcard));
        }
        std::cout << ">> Account added successfully!\n";
    } catch (...) {
        std::cout << "ERROR: Invalid balance value\n";
    }
}

void ConsoleUI::addTransactionInteractive() {
    std::string type, id, amt_str, from, to, d_str, m_str, y_str, ip, country, city;
    std::cout << "\n=== Add Transaction ===\n";
    std::cout << "Type (TRANSFER or INTL)? ";
    std::getline(std::cin, type);
    if (type != "TRANSFER" && type != "INTL") {
        std::cout << "ERROR: Type must be TRANSFER or INTL\n"; return;
    }
    
    std::cout << "Transaction ID (format: TX + number, e.g., TX001)? ";
    std::getline(std::cin, id);
    if (id.empty()) { std::cout << "ERROR: ID cannot be empty\n"; return; }
    
    std::cout << "Amount (numeric, > 0, e.g., 2000000)? ";
    std::getline(std::cin, amt_str);
    
    std::cout << "Sender Account ID (must exist, e.g., ACC001)? ";
    std::getline(std::cin, from);
    if (from.empty()) { std::cout << "ERROR: Sender ID cannot be empty\n"; return; }
    if (!db.getAccountByID(from)) { std::cout << "ERROR: Sender account not found\n"; return; }
    
    std::cout << "Receiver Account ID (must exist, e.g., ACC002)? ";
    std::getline(std::cin, to);
    if (to.empty()) { std::cout << "ERROR: Receiver ID cannot be empty\n"; return; }
    if (!db.getAccountByID(to)) { std::cout << "ERROR: Receiver account not found\n"; return; }
    
    std::cout << "Date - Day (1-31, e.g., 15)? ";
    std::getline(std::cin, d_str);
    std::cout << "Date - Month (1-12, e.g., 3)? ";
    std::getline(std::cin, m_str);
    std::cout << "Date - Year (>= 2000, e.g., 2026)? ";
    std::getline(std::cin, y_str);
    
    std::cout << "Location - IP Address (e.g., 192.168.1.10)? ";
    std::getline(std::cin, ip);
    std::cout << "Location - Country (e.g., Vietnam)? ";
    std::getline(std::cin, country);
    std::cout << "Location - City (e.g., Hanoi)? ";
    std::getline(std::cin, city);

    try {
        double amount = std::stod(amt_str);
        if (amount <= 0) { std::cout << "ERROR: Amount must be > 0\n"; return; }
        
        int day = std::stoi(d_str);
        int month = std::stoi(m_str);
        int year = std::stoi(y_str);
        if (day < 1 || day > 31 || month < 1 || month > 12 || year < 2000) {
            std::cout << "ERROR: Invalid date (day 1-31, month 1-12, year >= 2000)\n"; return;
        }
        
        Date dt{day, month, year};
        Location loc{ip, country, city};
        
        if (type == "INTL") {
            std::string src, tgt, rate_str, swift;
            std::cout << "Source Currency (e.g., USD)? ";
            std::getline(std::cin, src);
            std::cout << "Target Currency (e.g., VND)? ";
            std::getline(std::cin, tgt);
            std::cout << "Exchange Rate (numeric, > 0, e.g., 24000)? ";
            std::getline(std::cin, rate_str);
            std::cout << "SWIFT Code (e.g., SWFT123)? ";
            std::getline(std::cin, swift);
            
            double rate = std::stod(rate_str);
            if (rate <= 0) { std::cout << "ERROR: Exchange rate must be > 0\n"; return; }
            db.addTransaction(new InternationalTransfer(id, amount, from, to, dt, loc, src, tgt, rate, swift));
        } else {
            std::string bank;
            std::string fee_str;
            std::cout << "Bank Network (e.g., NAPAS)? ";
            std::getline(std::cin, bank);
            std::cout << "Fee (numeric, e.g., 5000)? ";
            std::getline(std::cin, fee_str);
            double fee = std::stod(fee_str);
            db.addTransaction(new TransferTransaction(id, amount, from, to, dt, loc, bank, fee));
        }
        std::cout << ">> Transaction added successfully!\n";
    } catch (...) {
        std::cout << "ERROR: Invalid input values\n";
    }
}

void ConsoleUI::viewAccountsInteractive() const {
    std::cout << "\n=== Accounts ===\n";
    if (db.getAccounts().empty()) {
        std::cout << "(No accounts yet)\n"; return;
    }
    for (auto* a : db.getAccounts()) a->printDetails();
}

void ConsoleUI::viewTransactionsInteractive() const {
    std::cout << "\n=== Transactions ===\n";
    if (db.getTransactions().empty()) {
        std::cout << "(No transactions yet)\n"; return;
    }
    for (auto* t : db.getTransactions())
        std::cout << t->getDetails() << "\n";
}

void ConsoleUI::selectRulesInteractive() {
    engine = AuditEngine();  // reset
    std::cout << "\n=== Select Audit Rules ===\n"
              << "Available rules:\n"
              << "  1. LargeAmountRule (flags > 1,000,000)\n"
              << "  2. SmurfingRule (multiple small txns/day)\n"
              << "  3. SanctionsRule (blacklist check)\n"
              << "  4. GeoAnomalyRule (high-risk countries)\n"
              << "Enter numbers separated by space (e.g., '1 2 4'): ";
    
    std::string input; std::getline(std::cin, input);
    if (input.empty()) { std::cout << "ERROR: Must select at least 1 rule\n"; return; }
    
    std::istringstream iss(input);
    int choice;
    int count = 0;
    while (iss >> choice) {
        if (choice == 1) { engine.addRule(new LargeAmountRule(1000000)); count++; }
        else if (choice == 2) { engine.addRule(new SmurfingRule(3, 5000)); count++; }
        else if (choice == 3) { engine.addRule(new SanctionsRule({"ACC_BAD", "ACC_OFAC"})); count++; }
        else if (choice == 4) { engine.addRule(new GeoAnomalyRule({"NorthKorea", "Iran"})); count++; }
    }
    
    if (count == 0) {
        std::cout << "ERROR: Invalid rule numbers\n"; return;
    }
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
    if (fileMgr.exportReport(results, "audit_report.txt")) {
        std::cout << ">> Report exported to 'audit_report.txt'\n";
        std::cout << "   Total warnings: " << results.size() << "\n";
    } else {
        std::cout << "ERROR: Could not write report file\n";
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
        for (auto* a : fileMgr.loadAccounts("accounts.csv")) db.addAccount(a);
        for (auto* t : fileMgr.loadTransactions("transactions.csv")) db.addTransaction(t);
        std::cout << ">> Demo data loaded (4 accounts, 8 transactions)!\n";
    }

    while (running) {
        displayMenu();
        std::string choice_str; 
        if (!std::getline(std::cin, choice_str) || choice_str.empty()) continue;
        
        try {
            int choice = std::stoi(choice_str);
            switch (choice) {
                case 1: addAccountInteractive(); break;
                case 2: addTransactionInteractive(); break;
                case 3: viewAccountsInteractive(); break;
                case 4: viewTransactionsInteractive(); break;
                case 5: selectRulesInteractive(); break;
                case 6: exportReportInteractive(); break;
                case 7: showHelp(); break;
                case 0: running = false; std::cout << "\n>> Goodbye!\n"; break;
                default: std::cout << "ERROR: Invalid choice (0-7)\n";
            }
        } catch (...) {
            std::cout << "ERROR: Invalid input\n";
        }
    }
}
