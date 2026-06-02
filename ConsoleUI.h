#pragma once
#include <string>
#include "AuditEngine.h"
#include "DataManagement.h"
#include "FileManager.h"

class ConsoleUI {
    AuditEngine    engine;
    DataManagement db;
    FileManager    fileMgr;
    bool           running = true;

    void displayMenu() const;
    void displayAccountGuide() const;
    void displayTransactionGuide() const;
    void displayRulesGuide() const;
    void showHelpMenu();
    void addAccountInteractive();
    void addTransactionInteractive();
    void selectRulesInteractive();
    void runAuditInteractive();
    void viewAccountsInteractive() const;
    void viewTransactionsInteractive() const;
    void exportReportInteractive();
    void showHelp() const;
public:
    void run();  // main loop
};
