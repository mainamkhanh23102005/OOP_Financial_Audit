#pragma once
#include <string>
#include <vector>

// ===== MODULE D: feature/engine-io (Team Lead) — Composition + Rule of Five =====
class Account;        // forward declaration
class Transaction;

class DataManagement {
    std::vector<Account*>     accounts;       // OWNING (composition)
    std::vector<Transaction*> transactions;   // OWNING (composition)
    void freeAll();
    void deepCopyFrom(const DataManagement& o);
public:
    DataManagement() = default;
    // ----- RULE OF FIVE -----
    ~DataManagement();                                      // 1
    DataManagement(const DataManagement& o);               // 2 copy ctor (deep)
    DataManagement& operator=(const DataManagement& o);    // 3 copy assign
    DataManagement(DataManagement&& o) noexcept;           // 4 move ctor
    DataManagement& operator=(DataManagement&& o) noexcept;// 5 move assign

    void addAccount(Account* a);
    void addTransaction(Transaction* t);
    Account* getAccountByID(const std::string& id) const;
    const std::vector<Transaction*>& getTransactions() const;
    const std::vector<Account*>&     getAccounts()     const;
};
