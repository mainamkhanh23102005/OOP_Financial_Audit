#include "AuditEngine.h"
#include "DataManagement.h"
#include "Transaction.h"

void AuditEngine::addRule(AuditRule* r) { if (r) rules.push_back(r); }

std::vector<AuditInspectionResult> AuditEngine::runAudit(const DataManagement& db) const {
    std::vector<AuditInspectionResult> out;
    for (auto* t : db.getTransactions())
        for (auto* rule : rules) {
            AuditInspectionResult res = rule->evaluate(t, db);   // DYNAMIC DISPATCH (vtable)
            if (!res.isPassed) out.push_back(res);
        }
    return out;
}
