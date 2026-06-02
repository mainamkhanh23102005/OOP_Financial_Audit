#pragma once
#include <string>
#include <vector>
#include "Common.h"

// ===== MODULE C: feature/audit-rules (Thanh vien 3) — Polymorphism =====
class Transaction;        // forward declaration
class DataManagement;

struct AuditInspectionResult {
    bool        isPassed = true;
    std::string transactionID;
    std::string ruleViolated;
    Severity    severity = Severity::NONE;
    std::string notes;
};

class AuditRule {                     // <<interface>>
public:
    virtual ~AuditRule() = default;
    virtual AuditInspectionResult evaluate(const Transaction* t,
                                           const DataManagement& db) const = 0;
    virtual std::string getRuleName() const = 0;
    virtual Severity    getSeverity() const = 0;
};

class LargeAmountRule : public AuditRule {
    double thresholdAlert;
public:
    explicit LargeAmountRule(double th);
    std::string getRuleName() const override;
    Severity    getSeverity() const override;
    AuditInspectionResult evaluate(const Transaction* t, const DataManagement& db) const override;
};

class SmurfingRule : public AuditRule {
    int    maxFrequencyPerDay;
    double smallTxThreshold;
public:
    SmurfingRule(int maxFreq, double smallTh);
    std::string getRuleName() const override;
    Severity    getSeverity() const override;
    AuditInspectionResult evaluate(const Transaction* t, const DataManagement& db) const override;
};

class SanctionsRule : public AuditRule {
    std::vector<std::string> blacklist;
public:
    explicit SanctionsRule(std::vector<std::string> bl);
    std::string getRuleName() const override;
    Severity    getSeverity() const override;
    AuditInspectionResult evaluate(const Transaction* t, const DataManagement& db) const override;
};

class GeoAnomalyRule : public AuditRule {
    std::vector<std::string> highRiskCountries;
public:
    explicit GeoAnomalyRule(std::vector<std::string> hr);
    std::string getRuleName() const override;
    Severity    getSeverity() const override;
    AuditInspectionResult evaluate(const Transaction* t, const DataManagement& db) const override;
};
