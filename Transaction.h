#pragma once
#include <string>
#include "Common.h"

// ===== MODULE B: feature/transactions =====
class Transaction {                   // <<abstract>>
protected:
    std::string transactionID;
    double      amount;
    std::string senderID, receiverID;
    TxStatus    status;
    Date        date;
    Location    location;
public:
    Transaction(std::string id, double amt, std::string from, std::string to,
                Date d, Location loc);
    virtual ~Transaction() = default;

    double      getAmount()     const;
    std::string getID()         const;
    std::string getSenderID()   const;
    std::string getReceiverID() const;
    Date        getDate()       const;
    Location    getLocation()   const;
    TxStatus    getStatus()     const;
    void        setStatus(TxStatus s);
    void        reverse();

    virtual std::string  getDetails() const = 0;
    virtual Transaction* clone()      const = 0;
};

class TransferTransaction : public Transaction {
    std::string bankNetwork;
    double      fee;
public:
    TransferTransaction(std::string id, double amt, std::string from, std::string to,
                        Date d, Location loc, std::string net, double f);
    // getters
    std::string getBankNetwork() const { return bankNetwork; }
    double      getFee()         const { return fee; }

    std::string  getDetails() const override;
    Transaction* clone()      const override;
};

// Base 2 cho da ke thua
class ForeignExchange {
protected:
    std::string sourceCurrency, targetCurrency;
    double      exchangeRate;
public:
    ForeignExchange(std::string src, std::string tgt, double rate);
    virtual ~ForeignExchange() = default;
    // getters
    std::string getSourceCurrency() const { return sourceCurrency; }
    std::string getTargetCurrency() const { return targetCurrency; }
    double      getExchangeRate()   const { return exchangeRate; }

    double      convertedAmount(double a) const;
    std::string getExchangeInfo() const;
};

// Da ke thua: Transaction + ForeignExchange (khong diamond -> khong can virtual inheritance)
class InternationalTransfer : public Transaction, public ForeignExchange {
    std::string swiftCode;
public:
    InternationalTransfer(std::string id, double amt, std::string from, std::string to,
                          Date d, Location loc, std::string src, std::string tgt,
                          double rate, std::string swift);
    // getter
    std::string getSwiftCode() const { return swiftCode; }

    std::string  getDetails() const override;
    Transaction* clone()      const override;
};