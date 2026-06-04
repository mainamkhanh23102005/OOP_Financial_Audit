#include "Transaction.h"
#include <sstream>
#include <iomanip>
#include <utility>

Transaction::Transaction(std::string id, double amt, std::string from, std::string to,
                         Date d, Location loc)
    : transactionID(std::move(id)), amount(amt),
      senderID(std::move(from)), receiverID(std::move(to)),
      status(TxStatus::PENDING), date(d), location(std::move(loc)) {}

double      Transaction::getAmount()     const { return amount; }
std::string Transaction::getID()         const { return transactionID; }
std::string Transaction::getSenderID()   const { return senderID; }
std::string Transaction::getReceiverID() const { return receiverID; }
Date        Transaction::getDate()       const { return date; }
Location    Transaction::getLocation()   const { return location; }
TxStatus    Transaction::getStatus()     const { return status; }
void        Transaction::setStatus(TxStatus s) { status = s; }
void        Transaction::reverse()             { status = TxStatus::REVERSED; }

// ===== TransferTransaction =====
TransferTransaction::TransferTransaction(std::string id, double amt, std::string from,
                                         std::string to, Date d, Location loc,
                                         std::string net, double f)
    : Transaction(std::move(id), amt, std::move(from), std::move(to), d, std::move(loc)),
      bankNetwork(std::move(net)), fee(f) {}

std::string TransferTransaction::getDetails() const {
    std::ostringstream os;
    os << std::fixed << std::setprecision(0);
    os << "TRANSFER " << transactionID << " " << senderID << "->" << receiverID
       << " amt=" << amount << " fee=" << fee << " via " << bankNetwork
       << " @" << date.toString();
    return os.str();
}
Transaction* TransferTransaction::clone() const { return new TransferTransaction(*this); }

// ===== ForeignExchange =====
ForeignExchange::ForeignExchange(std::string src, std::string tgt, double rate)
    : sourceCurrency(std::move(src)), targetCurrency(std::move(tgt)), exchangeRate(rate) {}

double ForeignExchange::convertedAmount(double a) const { return a * exchangeRate; }

std::string ForeignExchange::getExchangeInfo() const {
    std::ostringstream os;
    os << std::fixed << std::setprecision(2);
    os << sourceCurrency << "->" << targetCurrency << " @" << exchangeRate;
    return os.str();
}

// ===== InternationalTransfer =====
InternationalTransfer::InternationalTransfer(std::string id, double amt, std::string from,
                                             std::string to, Date d, Location loc,
                                             std::string src, std::string tgt,
                                             double rate, std::string swift)
    : Transaction(std::move(id), amt, std::move(from), std::move(to), d, std::move(loc)),
      ForeignExchange(std::move(src), std::move(tgt), rate),
      swiftCode(std::move(swift)) {}

std::string InternationalTransfer::getDetails() const {
    std::ostringstream os;
    os << std::fixed << std::setprecision(0);
    os << "INTL     " << transactionID << " " << senderID << "->" << receiverID
       << " amt=" << amount << " (" << getExchangeInfo()
       << " => " << convertedAmount(amount) << " " << targetCurrency << ")"
       << " SWIFT:" << swiftCode << " @" << date.toString();
    return os.str();
}
Transaction* InternationalTransfer::clone() const { return new InternationalTransfer(*this); }