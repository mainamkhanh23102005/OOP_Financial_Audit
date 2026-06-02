#pragma once
#include <vector>
#include "AuditRule.h"   // can AuditInspectionResult (kieu tra ve)

class DataManagement;

class AuditEngine {
    std::vector<AuditRule*> rules;   // AGGREGATION: chi tham chieu, KHONG so huu
public:
    void addRule(AuditRule* r);
    std::vector<AuditInspectionResult> runAudit(const DataManagement& db) const;
};
