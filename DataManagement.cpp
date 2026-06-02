#include "DataManagement.h"
#include "Account.h"
#include "Transaction.h"
#include <utility>

void DataManagement::freeAll() {
    for (auto* a : accounts)     delete a;
    for (auto* t : transactions) delete t;
    accounts.clear(); transactions.clear();
}
void DataManagement::deepCopyFrom(const DataManagement& o) {
    for (auto* a : o.accounts)     accounts.push_back(a->clone());      // deep copy da hinh
    for (auto* t : o.transactions) transactions.push_back(t->clone());
}

DataManagement::~DataManagement() { freeAll(); }
DataManagement::DataManagement(const DataManagement& o) { deepCopyFrom(o); }
DataManagement& DataManagement::operator=(const DataManagement& o) {
    if (this != &o) { freeAll(); deepCopyFrom(o); }
    return *this;
}
DataManagement::DataManagement(DataManagement&& o) noexcept
    : accounts(std::move(o.accounts)), transactions(std::move(o.transactions)) {
    o.accounts.clear(); o.transactions.clear();
}
DataManagement& DataManagement::operator=(DataManagement&& o) noexcept {
    if (this != &o) {
        freeAll();
        accounts     = std::move(o.accounts);
        transactions = std::move(o.transactions);
        o.accounts.clear(); o.transactions.clear();
    }
    return *this;
}

void DataManagement::addAccount(Account* a)         { if (a) accounts.push_back(a); }
void DataManagement::addTransaction(Transaction* t) { if (t) transactions.push_back(t); }
Account* DataManagement::getAccountByID(const std::string& id) const {
    for (auto* a : accounts) if (a->getAccountID() == id) return a;
    return nullptr;
}
const std::vector<Transaction*>& DataManagement::getTransactions() const { return transactions; }
const std::vector<Account*>&     DataManagement::getAccounts()     const { return accounts; }
