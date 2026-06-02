#pragma once
#include <string>
#include <sstream>

// ===== MODULE A: DATA STRUCTS (POD) + ENUMS — dung chung toan he thong =====
struct Date {
    int day = 1, month = 1, year = 2000;
    std::string toString() const {
        std::ostringstream os;
        os << (day < 10 ? "0" : "") << day << "/"
           << (month < 10 ? "0" : "") << month << "/" << year;
        return os.str();
    }
    bool sameDay(const Date& o) const {
        return day == o.day && month == o.month && year == o.year;
    }
};

struct Location {
    std::string ipAddress, country, city;
};

enum class Severity      { NONE, LOW, MEDIUM, HIGH, CRITICAL };
enum class AccountStatus { ACTIVE, FROZEN, CLOSED, UNDER_REVIEW };
enum class TxStatus      { PENDING, COMPLETED, FLAGGED, REVERSED, BLOCKED };

inline std::string severityToStr(Severity s) {
    switch (s) {
        case Severity::LOW:      return "LOW";
        case Severity::MEDIUM:   return "MEDIUM";
        case Severity::HIGH:     return "HIGH";
        case Severity::CRITICAL: return "CRITICAL";
        default:                 return "NONE";
    }
}
inline std::string acctStatusToStr(AccountStatus s) {
    switch (s) {
        case AccountStatus::FROZEN:       return "FROZEN";
        case AccountStatus::CLOSED:       return "CLOSED";
        case AccountStatus::UNDER_REVIEW: return "UNDER_REVIEW";
        default:                          return "ACTIVE";
    }
}
