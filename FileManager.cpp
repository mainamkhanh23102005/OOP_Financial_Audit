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

std::vector<Account*> FileManager::loadAccounts(const std::string& path) const {
    std::vector<Account*> v; std::ifstream f(path); std::string line;
    if (!f) { std::cerr << "Khong mo duoc " << path << "\n"; return v; }
    std::getline(f, line);                               // header
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        auto c = split(line);                            // type,id,owner,balance,extra...
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
    if (!f) { std::cerr << "Khong mo duoc " << path << "\n"; return v; }
    std::getline(f, line);
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        auto c = split(line);   // type,id,amount,from,to,d,m,y,ip,country,city,[extra...]
        Date     dt{std::stoi(c[5]), std::stoi(c[6]), std::stoi(c[7])};
        Location loc{c[8], c[9], c[10]};
        if (c[0] == "INTL")
            v.push_back(new InternationalTransfer(c[1], std::stod(c[2]), c[3], c[4], dt, loc,
                        c[11], c[12], std::stod(c[13]), c.size() > 14 ? c[14] : ""));
        else
            v.push_back(new TransferTransaction(c[1], std::stod(c[2]), c[3], c[4], dt, loc,
                        c.size() > 11 ? c[11] : "NAPAS",
                        c.size() > 12 ? std::stod(c[12]) : 0.0));
    }
    return v;
}

bool FileManager::exportReport(const std::vector<AuditInspectionResult>& res,
                               const std::string& path) const {
    std::ofstream f(path); if (!f) return false;
    f << "==== AUDIT REPORT ====\nTong canh bao: " << res.size() << "\n\n";
    for (const auto& r : res)
        f << "[" << severityToStr(r.severity) << "] Tx " << r.transactionID
          << " vi pham " << r.ruleViolated << " -> " << r.notes << "\n";
    return true;
}
