#include "AuditRule.h"
#include "Transaction.h"
#include "DataManagement.h"
#include <algorithm>
#include <utility>

// ---- LargeAmountRule ----
LargeAmountRule::LargeAmountRule(double th) : thresholdAlert(th) {}
std::string LargeAmountRule::getRuleName() const { return "LargeAmountRule"; }
Severity    LargeAmountRule::getSeverity() const { return Severity::HIGH; }
AuditInspectionResult LargeAmountRule::evaluate(const Transaction* t, const DataManagement&) const {
    AuditInspectionResult r; r.transactionID = t->getID();
    if (t->getAmount() > thresholdAlert) {
        r.isPassed = false; r.ruleViolated = getRuleName(); r.severity = getSeverity();
        r.notes = "Giao dich vuot han muc " + std::to_string((long long)thresholdAlert);
    }
    return r;
}

// ---- SmurfingRule (can lich su -> dung db) ----
SmurfingRule::SmurfingRule(int maxFreq, double smallTh)
    : maxFrequencyPerDay(maxFreq), smallTxThreshold(smallTh) {}
std::string SmurfingRule::getRuleName() const { return "SmurfingRule"; }
Severity    SmurfingRule::getSeverity() const { return Severity::CRITICAL; }
AuditInspectionResult SmurfingRule::evaluate(const Transaction* t, const DataManagement& db) const {
    AuditInspectionResult r; r.transactionID = t->getID();
    if (t->getAmount() >= smallTxThreshold) return r;
    int count = 0;
    for (auto* o : db.getTransactions())
        if (o->getSenderID() == t->getSenderID()
            && o->getDate().sameDay(t->getDate())
            && o->getAmount() < smallTxThreshold) ++count;
    if (count >= maxFrequencyPerDay) {
        r.isPassed = false; r.ruleViolated = getRuleName(); r.severity = getSeverity();
        r.notes = "Phat hien " + std::to_string(count) + " giao dich nho/ngay tu " + t->getSenderID();
    }
    return r;
}

// ---- SanctionsRule ----
SanctionsRule::SanctionsRule(std::vector<std::string> bl) : blacklist(std::move(bl)) {}
std::string SanctionsRule::getRuleName() const { return "SanctionsRule"; }
Severity    SanctionsRule::getSeverity() const { return Severity::CRITICAL; }
AuditInspectionResult SanctionsRule::evaluate(const Transaction* t, const DataManagement&) const {
    AuditInspectionResult r; r.transactionID = t->getID();
    auto hit = [&](const std::string& id) {
        return std::find(blacklist.begin(), blacklist.end(), id) != blacklist.end(); };
    if (hit(t->getSenderID()) || hit(t->getReceiverID())) {
        r.isPassed = false; r.ruleViolated = getRuleName(); r.severity = getSeverity();
        r.notes = "Lien quan tai khoan trong danh sach cam van";
    }
    return r;
}

// ---- GeoAnomalyRule (tan dung struct Location) ----
GeoAnomalyRule::GeoAnomalyRule(std::vector<std::string> hr) : highRiskCountries(std::move(hr)) {}
std::string GeoAnomalyRule::getRuleName() const { return "GeoAnomalyRule"; }
Severity    GeoAnomalyRule::getSeverity() const { return Severity::MEDIUM; }
AuditInspectionResult GeoAnomalyRule::evaluate(const Transaction* t, const DataManagement&) const {
    AuditInspectionResult r; r.transactionID = t->getID();
    const std::string country = t->getLocation().country;
    if (std::find(highRiskCountries.begin(), highRiskCountries.end(), country)
        != highRiskCountries.end()) {
        r.isPassed = false; r.ruleViolated = getRuleName(); r.severity = getSeverity();
        r.notes = "Giao dich tu quoc gia rui ro cao: " + country;
    }
    return r;
}
