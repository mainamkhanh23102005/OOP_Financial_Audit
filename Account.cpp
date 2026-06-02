#include "Account.h"
#include <iostream>
#include <utility>

Account::Account(std::string id, std::string owner, double bal)
    : accountID(std::move(id)), balance(bal),
      ownerName(std::move(owner)), status(AccountStatus::ACTIVE) {}

std::string   Account::getAccountID() const { return accountID; }
double        Account::getBalance()   const { return balance; }
std::string   Account::getOwnerName() const { return ownerName; }
AccountStatus Account::getStatus()    const { return status; }

void Account::freeze()         { status = AccountStatus::FROZEN; }
void Account::unfreeze()       { if (status == AccountStatus::FROZEN) status = AccountStatus::ACTIVE; }
bool Account::isFrozen() const { return status == AccountStatus::FROZEN; }

void Account::deposit(double amt) {
    if (amt > 0 && status == AccountStatus::ACTIVE) balance += amt;
}
bool Account::withdraw(double amt) {
    if (status != AccountStatus::ACTIVE || amt <= 0 || amt > balance) return false;
    balance -= amt; return true;
}

CorporateAccount::CorporateAccount(std::string id, std::string owner, double bal,
                                   std::string tax, std::string type)
    : Account(std::move(id), std::move(owner), bal),
      taxCode(std::move(tax)), companyType(std::move(type)) {}
void CorporateAccount::printDetails() const {
    std::cout << "[CORP]   " << accountID << " | " << ownerName
              << " | Tax:" << taxCode << " | " << companyType
              << " | Bal:" << balance << " | " << acctStatusToStr(status) << "\n";
}
Account* CorporateAccount::clone() const { return new CorporateAccount(*this); }

RetailAccount::RetailAccount(std::string id, std::string owner, double bal, std::string idCard)
    : Account(std::move(id), std::move(owner), bal), IDCardNumber(std::move(idCard)) {}
void RetailAccount::printDetails() const {
    std::cout << "[RETAIL] " << accountID << " | " << ownerName
              << " | CMND:" << IDCardNumber
              << " | Bal:" << balance << " | " << acctStatusToStr(status) << "\n";
}
Account* RetailAccount::clone() const { return new RetailAccount(*this); }
