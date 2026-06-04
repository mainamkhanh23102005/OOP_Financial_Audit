#include "FileManager.h"
#include "Account.h"
#include "Transaction.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<std::string> FileManager::split(const std::string& line, char d) {
    std::vector<std::string> v; std::stringstream ss(line); std::string item;
    while (std::getline(ss, item, d)) v.push_back(item);
    return v;
}

// ===== LOAD =====
std::vector<Account*> FileManager::loadAccounts(const std::string& path) const {
    std::vector<Account*> v; std::ifstream f(path); std::string line;
    if (!f) { std::cerr << "Cannot open " << path << "\n"; return v; }
    std::getline(f, line);   // skip header
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        auto c = split(line);
        if (c[0] == "CORP")
            v.push_back(new CorporateAccount(c[1], c[2], std::stod(c[3]),
                                             c[4], c.size() > 5 ? c[5] : ""));
        else
            v.push_back(new RetailAccount(c[1], c[2], std::stod(c[3]),
                                          c.size() > 4 ? c[4] : ""));
    }
    return v;
}

std::vector<Transaction*> FileManager::loadTransactions(const std::string& path) const {
    std::vector<Transaction*> v; std::ifstream f(path); std::string line;
    if (!f) { std::cerr << "Cannot open " << path << "\n"; return v; }
    std::getline(f, line);
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        auto c = split(line);
        Date     dt{std::stoi(c[5]), std::stoi(c[6]), std::stoi(c[7])};
        Location loc{c[8], c[9], c[10]};
        if (c[0] == "INTL")
            v.push_back(new InternationalTransfer(
                c[1], std::stod(c[2]), c[3], c[4], dt, loc,
                c[11], c[12], std::stod(c[13]), c.size() > 14 ? c[14] : ""));
        else
            v.push_back(new TransferTransaction(
                c[1], std::stod(c[2]), c[3], c[4], dt, loc,
                c.size() > 11 ? c[11] : "NAPAS",
                c.size() > 12 ? std::stod(c[12]) : 0.0));
    }
    return v;
}

// ===== SAVE (tu dong ghi vao CSV sau khi add) =====
bool FileManager::saveAccounts(const std::vector<Account*>& accounts,
                                const std::string& path) const {
    std::ofstream f(path);
    if (!f) return false;
    f << "type,accountID,ownerName,balance,extra1,extra2\n";
    for (auto* a : accounts) {
        if (auto* corp = dynamic_cast<CorporateAccount*>(a))
            f << "CORP,"   << corp->getAccountID() << ","
                           << corp->getOwnerName()  << ","
                           << corp->getBalance()    << ","
                           << corp->getTaxCode()    << ","
                           << corp->getCompanyType()<< "\n";
        else if (auto* ret = dynamic_cast<RetailAccount*>(a))
            f << "RETAIL," << ret->getAccountID()  << ","
                           << ret->getOwnerName()   << ","
                           << ret->getBalance()     << ","
                           << ret->getIDCard()      << "\n";
    }
    return true;
}

bool FileManager::saveTransactions(const std::vector<Transaction*>& transactions,
                                    const std::string& path) const {
    std::ofstream f(path);
    if (!f) return false;
    f << "type,txnID,amount,senderID,receiverID,day,month,year,"
         "ip,country,city,extra1,extra2,extra3,extra4\n";
    for (auto* t : transactions) {
        Date     d = t->getDate();
        Location l = t->getLocation();
        if (auto* intl = dynamic_cast<InternationalTransfer*>(t))
            f << "INTL,"     << intl->getID()             << ","
                             << intl->getAmount()          << ","
                             << intl->getSenderID()        << ","
                             << intl->getReceiverID()      << ","
                             << d.day   << "," << d.month  << "," << d.year << ","
                             << l.ipAddress << "," << l.country << "," << l.city << ","
                             << intl->getSourceCurrency()  << ","
                             << intl->getTargetCurrency()  << ","
                             << intl->getExchangeRate()    << ","
                             << intl->getSwiftCode()       << "\n";
        else if (auto* tr = dynamic_cast<TransferTransaction*>(t))
            f << "TRANSFER," << tr->getID()               << ","
                             << tr->getAmount()            << ","
                             << tr->getSenderID()          << ","
                             << tr->getReceiverID()        << ","
                             << d.day   << "," << d.month  << "," << d.year << ","
                             << l.ipAddress << "," << l.country << "," << l.city << ","
                             << tr->getBankNetwork()       << ","
                             << tr->getFee()               << "\n";
    }
    return true;
}

// ===== EXPORT REPORT =====
bool FileManager::exportReport(const std::vector<AuditInspectionResult>& res,
                               const std::string& path) const {
    std::ofstream f(path); if (!f) return false;
    f << "==== AUDIT REPORT ====\nTong canh bao: " << res.size() << "\n\n";
    for (const auto& r : res)
        f << "[" << severityToStr(r.severity) << "] Tx " << r.transactionID
          << " vi pham " << r.ruleViolated << " -> " << r.notes << "\n";
    return true;
}