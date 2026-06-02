#pragma once
#include <string>
#include "Common.h"

// ===== MODULE B: feature/transactions (Thanh vien 2) — Da ke thua + Composition =====
class Transaction {                   // <<abstract>>
protected:
    std::string transactionID;
    double      amount;
    std::string senderID, receiverID;
    TxStatus    status;
    Date        date;                 // Composition
    Location    location;             // Composition
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
    void        reverse();            // chargeback / hoan tien

    virtual std::string  getDetails() const = 0;   // pure virtual
    virtual Transaction* clone()      const = 0;
};

class TransferTransaction : public Transaction {
    std::string bankNetwork;
    double      fee;
public:
    TransferTransaction(std::string id, double amt, std::string from, std::string to,
                        Date d, Location loc, std::string net, double f);
    std::string  getDetails() const override;
    Transaction* clone()      const override;
};

// Base 2 cho da ke thua (mixin xu ly ty gia)
class ForeignExchange {
protected:
    std::string sourceCurrency, targetCurrency;
    double      exchangeRate;
public:
    ForeignExchange(std::string src, std::string tgt, double rate);
    virtual ~ForeignExchange() = default;
    double      convertedAmount(double a) const;
    std::string getExchangeInfo() const;
};

// DA KE THUA: Transaction + ForeignExchange (khong diamond -> khong can virtual inheritance)
class InternationalTransfer : public Transaction, public ForeignExchange {
    std::string swiftCode;
public:
    InternationalTransfer(std::string id, double amt, std::string from, std::string to,
                          Date d, Location loc, std::string src, std::string tgt,
                          double rate, std::string swift);
    std::string  getDetails() const override;
    Transaction* clone()      const override;
};
