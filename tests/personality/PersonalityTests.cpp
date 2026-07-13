#include <catch2/catch_test_macros.hpp>
#include "thalapathy/personality/Personality.hpp"
#include <unordered_set>
#include <algorithm>

using namespace thalapathy;

TEST_CASE("Personality pack counts and IDs validation", "[personality]") {
    const auto& vijay = PersonalityEngine::getVijayPack();
    const auto& build = PersonalityEngine::getBuildPack();
    const auto& tvk = PersonalityEngine::getTvkPack();
    const auto& pol = PersonalityEngine::getTamilPoliticsPack();
    const auto& dmk = PersonalityEngine::getDmkPack();
    const auto& pf = PersonalityEngine::getPartyFundPack();
    const auto& ts = PersonalityEngine::getTheeyaSakthiPack();
    const auto& vs = PersonalityEngine::getTvkVsLegacyPack();
    const auto& banners = PersonalityEngine::getStartupBannerPack();
    const auto& tvkoop = PersonalityEngine::getTVKOOPPack();
    const auto& massoop = PersonalityEngine::getThalapathyOOPMassPack();
    const auto& dmkoop = PersonalityEngine::getDMKOOPRoastPack();

    // 1. Pack counts meet minimum requirements
    SECTION("Minimum counts verification") {
        CHECK(vijay.size() >= 150);
        CHECK(build.size() >= 40);
        CHECK(tvk.size() >= 75);
        CHECK(pol.size() >= 60);
        CHECK(dmk.size() >= 300);
        CHECK(pf.size() >= 100);
        CHECK(ts.size() >= 75);
        CHECK(vs.size() >= 75);
        CHECK(banners.size() >= 20);
        CHECK(tvkoop.size() >= 200);
        CHECK(massoop.size() >= 200);
        CHECK(dmkoop.size() >= 200);
    }

    // 2. Unique message IDs verification
    SECTION("ID uniqueness verification") {
        std::unordered_set<std::string> ids;
        auto addIds = [&](const std::vector<PersonalityMessage>& pack) {
            for (const auto& msg : pack) {
                CHECK(!msg.id.empty());
                CHECK(!msg.text.empty());
                auto inserted = ids.insert(msg.id);
                CHECK(inserted.second); // must be unique
            }
        };

        addIds(vijay);
        addIds(build);
        addIds(tvk);
        addIds(pol);
        addIds(dmk);
        addIds(pf);
        addIds(ts);
        addIds(vs);
        addIds(tvkoop);
        addIds(massoop);
        addIds(dmkoop);
    }
}

TEST_CASE("Personality mode sanitation rules", "[personality]") {
    Diagnostic dummyDiag;
    dummyDiag.code = "THALA-GEN-001";
    dummyDiag.severity = DiagnosticSeverity::ERROR;
    dummyDiag.title = "General Error";

    SECTION("Professional mode has no decorated text") {
        PersonalityEngine::setMode(PersonalityMode::Professional);
        std::string raw = "error: test";
        std::string decorated = PersonalityEngine::decorate(dummyDiag, raw);
        CHECK(decorated == raw);
    }

    SECTION("Mass mode verification") {
        PersonalityEngine::setMode(PersonalityMode::Mass);
        
        // Ensure decoration does not contain DMK, PartyFund or TheeyaSakthi substrings
        PersonalityEngine::resetCache();
        for (int i = 0; i < 50; ++i) {
            PersonalityEngine::setDeterministicIndex(i);
            std::string decorated = PersonalityEngine::decorate(dummyDiag, "error");
            CHECK(decorated.find("DMK") == std::string::npos);
            CHECK(decorated.find("PartyFund") == std::string::npos);
            CHECK(decorated.find("theeya sakthi") == std::string::npos);
        }
        PersonalityEngine::clearDeterministicIndex();
    }

    SECTION("Political Parody can render political contents") {
        PersonalityEngine::setMode(PersonalityMode::PoliticalParody);
        PersonalityEngine::setDeterministicIndex(0);
        // DMK message verification
        Diagnostic dmkDiag;
        dmkDiag.code = "THALA-CLASS-001"; // maps to DMK inheritance warning
        std::string dmkDec = PersonalityEngine::decorate(dmkDiag, "error");
        CHECK(dmkDec.find("DMKInheritanceTree") != std::string::npos);

        // PartyFund verification
        Diagnostic pfDiag;
        pfDiag.code = "THALA-RUNTIME-001"; // maps to PartyFund
        std::string pfDec = PersonalityEngine::decorate(pfDiag, "error");
        CHECK(pfDec.find("PartyFundBigInt") != std::string::npos);

        // Theeya Sakthi verification
        Diagnostic tsDiag;
        tsDiag.code = "THALA-LOOP-001"; // maps to loop error
        std::string tsDec = PersonalityEngine::decorate(tsDiag, "error");
        CHECK(tsDec.find("theeya sakthi") != std::string::npos);

        PersonalityEngine::clearDeterministicIndex();
    }
}
