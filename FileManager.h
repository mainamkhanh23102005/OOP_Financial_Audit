#pragma once
#include <string>
#include <vector>
#include "AuditRule.h"

class Account;
class Transaction;

class FileManager {
    static std::vector<std::string> split(const std::string& line, char d = ',');
public:
    // Load
    std::vector<Account*>     loadAccounts(const std::string& path) const;
    std::vector<Transaction*> loadTransactions(const std::string& path) const;

    // Save (auto-called after add)
    bool saveAccounts(const std::vector<Account*>& accounts,
                      const std::string& path) const;
    bool saveTransactions(const std::vector<Transaction*>& transactions,
                          const std::string& path) const;

    // Report
    bool exportReport(const std::vector<AuditInspectionResult>& results,
                      const std::string& path) const;
};