#pragma once
#include <string>
#include <vector>
#include "thalapathy/diagnostics/Diagnostics.hpp"

namespace thalapathy {

enum class PersonalityMode {
    Professional,
    Mass,
    PoliticalParody
};

struct PersonalityMessage {
    std::string id;
    std::string text;
};

class PersonalityEngine {
public:
    static void setMode(PersonalityMode mode);
    static PersonalityMode getMode();

    static std::string decorate(const Diagnostic& diag, const std::string& renderedDiag);
    static std::string getBuildSuccessMessage();
    static std::string getStartupBanner();
    static void resetCache();

    // For deterministic testing or deterministic mode (e.g. index selection)
    static void setDeterministicIndex(int index);
    static void clearDeterministicIndex();

    // Packs
    static const std::vector<PersonalityMessage>& getVijayPack();
    static const std::vector<PersonalityMessage>& getBuildPack();
    static const std::vector<PersonalityMessage>& getTvkPack();
    static const std::vector<PersonalityMessage>& getTamilPoliticsPack();
    static const std::vector<PersonalityMessage>& getDmkPack();
    static const std::vector<PersonalityMessage>& getPartyFundPack();
    static const std::vector<PersonalityMessage>& getTheeyaSakthiPack();
    static const std::vector<PersonalityMessage>& getTvkVsLegacyPack();
    static const std::vector<std::string>& getStartupBannerPack();
    static const std::vector<PersonalityMessage>& getTVKOOPPack();
    static const std::vector<PersonalityMessage>& getThalapathyOOPMassPack();
    static const std::vector<PersonalityMessage>& getDMKOOPRoastPack();
};

} // namespace thalapathy
