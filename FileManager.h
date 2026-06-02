#pragma once
#include <string>
#include <vector>
#include "AuditRule.h"   // can AuditInspectionResult cho exportReport

class Account;
class Transaction;

class FileManager {
    static std::vector<std::string> split(const std::string& line, char d = ',');
public:
    std::vector<Account*>     loadAccounts(const std::string& path) const;
    std::vector<Transaction*> loadTransactions(const std::string& path) const;
    bool exportReport(const std::vector<AuditInspectionResult>& results,
                      const std::string& path) const;
};
