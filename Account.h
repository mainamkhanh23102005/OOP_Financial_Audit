#pragma once
#include <string>
#include "Common.h"

// ===== MODULE B: feature/core-entities =====
class Account {                       // <<abstract>>
protected:
    std::string   accountID;
    double        balance;
    std::string   ownerName;
    AccountStatus status;
public:
    Account(std::string id, std::string owner, double bal);
    virtual ~Account() = default;

    std::string   getAccountID() const;
    double        getBalance()   const;
    std::string   getOwnerName() const;
    AccountStatus getStatus()    const;

    void freeze();
    void unfreeze();
    bool isFrozen() const;
    void deposit(double amt);
    bool withdraw(double amt);

    virtual void     printDetails() const = 0;
    virtual Account* clone()        const = 0;
};

class CorporateAccount : public Account {
    std::string taxCode, companyType;
public:
    CorporateAccount(std::string id, std::string owner, double bal,
                     std::string tax, std::string type);
    // getters
    std::string getTaxCode()     const { return taxCode; }
    std::string getCompanyType() const { return companyType; }

    void     printDetails() const override;
    Account* clone()        const override;
};

class RetailAccount : public Account {
    std::string IDCardNumber;
public:
    RetailAccount(std::string id, std::string owner, double bal, std::string idCard);
    // getter
    std::string getIDCard() const { return IDCardNumber; }

    void     printDetails() const override;
    Account* clone()        const override;
};