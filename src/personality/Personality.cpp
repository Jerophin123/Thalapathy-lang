#include "thalapathy/personality/Personality.hpp"
#include <unordered_set>
#include <random>
#include <iostream>

namespace thalapathy {

static PersonalityMode currentMode = PersonalityMode::Professional;
static std::unordered_set<std::string> usedMessageIds;
static int deterministicIndex = -1;

void PersonalityEngine::setMode(PersonalityMode mode) {
    currentMode = mode;
}

PersonalityMode PersonalityEngine::getMode() {
    return currentMode;
}

void PersonalityEngine::resetCache() {
    usedMessageIds.clear();
}

void PersonalityEngine::setDeterministicIndex(int index) {
    deterministicIndex = index;
}

void PersonalityEngine::clearDeterministicIndex() {
    deterministicIndex = -1;
}

static const PersonalityMessage& selectMessage(const std::vector<PersonalityMessage>& pack) {
    if (pack.empty()) {
        static PersonalityMessage fallback = {"FALLBACK", "Nanba, standard fallback active."};
        return fallback;
    }
    if (deterministicIndex != -1) {
        return pack[static_cast<size_t>(deterministicIndex) % pack.size()];
    }

    // Try to find an unused message
    std::vector<size_t> unusedIndices;
    for (size_t i = 0; i < pack.size(); ++i) {
        if (usedMessageIds.find(pack[i].id) == usedMessageIds.end()) {
            unusedIndices.push_back(i);
        }
    }

    if (unusedIndices.empty()) {
        // All messages have been used. Clear cache for this pack's items.
        for (const auto& msg : pack) {
            usedMessageIds.erase(msg.id);
        }
        return pack[0];
    }

    // Select first unused for deterministic progress, or random.
    // To ensure testing doesn't depend on rand(), we can pick the first unused, or use a simple pseudo-random.
    size_t chosenIndex = unusedIndices[0];
    const auto& selected = pack[chosenIndex];
    usedMessageIds.insert(selected.id);
    return selected;
}

std::string PersonalityEngine::decorate(const Diagnostic& diag, const std::string& renderedDiag) {
    if (currentMode == PersonalityMode::Professional) {
        return renderedDiag;
    }

    std::string decoration;

    if (currentMode == PersonalityMode::Mass) {
        // Only Vijay/Thalapathy Cinema messages
        decoration = selectMessage(getVijayPack()).text;
    } else if (currentMode == PersonalityMode::PoliticalParody) {
        // Semantic selection based on diagnostic code
        if (diag.code == "THALA-MUT-001") { // Immutable reassignment
            // Vijay mudivu joke
            decoration = getVijayPack()[1].text; // "Compiler oru thadava immutable-nu mudivu..."
        } else if (diag.code == "THALA-MUT-002") { // Constant reassignment
            decoration = getVijayPack()[1].text;
        } else if (diag.code == "THALA-TYPE-001") { // Type mismatch
            // Mersal/type joke or politics exit poll
            if (deterministicIndex != -1) {
                decoration = getTamilPoliticsPack()[0].text; // ExitPollTypeError
            } else {
                decoration = selectMessage(getTamilPoliticsPack()).text;
            }
        } else if (diag.code == "THALA-CLASS-001") { // Inheritance cycle
            decoration = getDmkPack()[3].text; // DMKInheritanceTree family tree
        } else if (diag.code == "THALA-CLASS-002") { // Super without inheritance or direct entry
            decoration = getVijayPack()[8].text; // Leo visibility protected or super
        } else if (diag.code == "THALA-LOOP-001") { // Break/continue outside loop or infinite loop
            // Infinite loop
            decoration = getTheeyaSakthiPack()[4].text; // TheeyaSakthiLoopError
        } else if (diag.code == "THALA-RUNTIME-001") { // Stack overflow or PartyFund warning
            decoration = getPartyFundPack()[0].text; // PartyFundBigIntWarning
        } else {
            // General political fallback
            // Rotate through political packs
            static int packRotator = 0;
            int chosenPack = (deterministicIndex != -1) ? deterministicIndex : packRotator++;
            switch (chosenPack % 6) {
                case 0: decoration = selectMessage(getTvkPack()).text; break;
                case 1: decoration = selectMessage(getDmkPack()).text; break;
                case 2: decoration = selectMessage(getPartyFundPack()).text; break;
                case 3: decoration = selectMessage(getTheeyaSakthiPack()).text; break;
                case 4: decoration = selectMessage(getTvkVsLegacyPack()).text; break;
                default: decoration = selectMessage(getTamilPoliticsPack()).text; break;
            }
        }
    }

    if (decoration.empty()) {
        return renderedDiag;
    }

    return renderedDiag + "\n" + decoration + "\n";
}

std::string PersonalityEngine::getBuildSuccessMessage() {
    if (currentMode == PersonalityMode::Professional) {
        return "Build successful. Zero errors. Zero warnings.";
    }
    return selectMessage(getBuildPack()).text;
}

std::string PersonalityEngine::getStartupBanner() {
    if (currentMode == PersonalityMode::Professional) {
        return "THALAPATHY REPL v0.1.0\nNanba, code sollu. \U0001F525";
    } else if (currentMode == PersonalityMode::Mass) {
        return getStartupBannerPack()[0];
    } else {
        // PoliticalParody
        if (deterministicIndex != -1) {
            return getStartupBannerPack()[static_cast<size_t>(deterministicIndex) % getStartupBannerPack().size()];
        }
        // Return political parity banner
        return getStartupBannerPack()[1];
    }
}

} // namespace thalapathy
