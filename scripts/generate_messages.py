import os

output_file = "src/personality/PersonalityMessages.cpp"

# 1. Vijay Messages (150)
vijay_templates = [
    "Nanba, variable use pannita. Declare panna maranthutiye. 😂",
    "Compiler oru thadava immutable-nu mudivu pannita, binding-ah maatha mudiyathu. 🔥",
    "Nee return pannuva nu signature solluthu. Return enga, Nanba? 😭",
    "\"I'm waiting...\" async runtime-ku. Keyword ippo reserved. 🔥",
    "Master class ready. Object mattum instantiate aagala. Vaathi ready... attendance zero. 😂",
    "Loop-ah irundhalum oru mudivu venum, Nanba. Compiler 3 million iterations-ah wait pannithu. 😭",
    "Expected int. String vandhurukku. Idhu Mersal twist illa. Straight-ah type error. 😂",
    "Leo visibility active. Protected member-ku direct entry illa. Bloody sweet... access denied. 🔥",
    "Theri! Exception control flow-ku poiduchu. Kaavalan catch block enga Nanba? 😂",
    "Array bounds thaandi poita. Ghilli speed irukkalam, array size-ku veliya odakoodathu. 😭",
    "GOAT build. Regression test-ku complaint illa. 🔥",
]

vijay_msgs = []
# Create 150 unique Vijay messages
for i in range(1, 151):
    m_id = f"VIJAY_MSG_{i:03d}"
    if i <= len(vijay_templates):
        m_text = vijay_templates[i-1]
    else:
        # Generate unique variations focusing on different Vijay films & concepts
        films = ["Ghilli", "Pokkiri", "Mersal", "Theri", "Master", "Leo", "GOAT", "Varisu", "Sarkar", "Kaththi", "Thuppakki", "Friends", "Kushi", "Badri", "Thamizhan", "Vettaikaaran", "Kuruvi", "Villu", "Sura", "Kaavalan"]
        film = films[i % len(films)]
        m_text = f"Nanba, {film} cinema style-la code run panren. Dynamic type check code {i} successfully passed. 🔥"
    vijay_msgs.append((m_id, m_text))

# 2. Build Messages (40)
build_msgs = [
    ("BUILD_MSG_001", "Build successful. Nanba... mudichachu. \U0001F525"),
    ("BUILD_MSG_002", "Zero errors. Zero warnings. Mass entry ready. \U0001F525😂"),
    ("BUILD_MSG_003", "Entry point found. Aarambam ready. 🔥"),
    ("BUILD_MSG_004", "Clean build, Nanba. Runtime-ku vazhi vidu. 😂"),
    ("BUILD_MSG_005", "GOAT build. Regression test-ku complaint illa. 🔥😂"),
    ("BUILD_MSG_006", "Nanba... cup mukkiyam illa. Build mukkiyam Bigilu. 🔥😂"),
]
for i in range(7, 41):
    build_msgs.append((f"BUILD_MSG_{i:03d}", f"Build successful status {i}: Mass binary compiled. Ready for deployment. 🔥"))

# 3. TVK Messages (75)
tvk_msgs = [
    ("TVK_MSG_001", "TVKBranchSwitch: cinema branch clean, politics branch checkout panniyachu. Git history ippo interesting-ah poguthu Nanba. 😂🔥"),
    ("TVK_MSG_002", "TVKEntryPoint: Pudhu political module-ku aarambam detected. Legacy runtime ellam log level DEBUG-ku maathiduchu. 😂"),
    ("TVK_MSG_003", "TVK2026Cron: Task schedule panniyachu. 2026 trigger varaikum scheduler-ah disturb pannatheenga. 😂🔥"),
    ("TVK_MSG_004", "TVKMakkalAPI: Public endpoint open pannanga. First second-la request count paathu rate limiter resignation letter type pannuthu. 😭😂"),
    ("TVKM_MSG_005", "TVKMaanaduLoadBalancer: Expected traffic: 10K. Actual concurrent connections paathu server: 'Nanba... naan enna paavam pannen?' 😭😂"),
    ("TVK_MSG_006", "TVKMembershipDatabase: INSERT query start pannom. Auto-increment ID-ku cardio workout aaguthu. 😂"),
    ("TVK_MSG_007", "TVKKolgaiInterface: Interface declare panniyachu. Compiler implementation-ah wait pannuthu. 'I'm waiting...' 🔥😂"),
    ("TVK_MSG_008", "TVKPoliticalEntry: Actor module already stable. New 'politics' dependency install panniyachu. README comments section ippo war zone. 😂"),
    ("TVK_MSG_009", "TVKThreadPool: 10 worker threads request pannom. Makkal runtime 10,000 threads spawn panniduchu. CPU: 'Enna vittu vidunga da.' 😭😂"),
    ("TVK_MSG_010", "TVKSocialMediaParser: One statement input. 4,892 interpretations output. Parser maintain panna mudiyala Nanba. 😂"),
]
for i in range(11, 76):
    tvk_msgs.append((f"TVK_MSG_{i:03d}", f"TVK political runtime register {i}: Policy module active. Cadre status check passed. 😂"))

# 4. Tamil Politics (60)
politics_msgs = [
    ("POL_MSG_001", "ExitPollTypeError: Expected final result. Received speculative boolean. 😂"),
    ("POL_MSG_002", "AllianceDependencyHell: Package A requires Party B. Party B conflicts with Party C. 😭😂"),
    ("POL_MSG_003", "CandidateArrayOverflow: Constituency slot already occupied. 😂"),
    ("POL_MSG_004", "ManifestoInterfaceError: Method declared. Concrete implementation missing. 🔥😂"),
    ("POL_MSG_005", "PressMeetParserError: Question token detected. Response grammar changed. 😂"),
    ("POL_MSG_006", "BannerMemoryWarning: Image allocation exceeds source-code size. 😭😂"),
    ("POL_MSG_007", "CadreThreadPool: 10 workers requested. Runtime spawned 10,000. 😂"),
    ("POL_MSG_008", "VoteCountRaceCondition: Two threads claimed the same result before lock release. 😂"),
    ("POL_MSG_009", "PartySwitchHotReload: Module changed package without process restart. 😭😂"),
]
for i in range(10, 61):
    politics_msgs.append((f"POL_MSG_{i:03d}", f"Tamil politics scheduler {i}: Alliance checking active. Process ID validated. 😂"))

# 5. DMK Roast Messages (300)
# Let's write out some templates for DMK roasts using standard concepts
dmk_roasts = [
    "DMKPromiseCompiler: Manifesto-la function declare panniyachu. Implementation file open panna: `// TODO` 😭😂",
    "DMKUnitTest: Expected: `delivered`, Received: `announced`. Test failed successfully. 😂",
    "DMKLegacyCode: Developer: 'Indha dependency remove pannalama?' Build system: 'Family package da... touch panna project collapse aagum.' 😭😂",
    "DMKInheritanceTree: Class diagram generate panna sonnom. Compiler family tree print panniduchu. A4 sheet podhala Nanba. 😂",
    "DMKPreparedResponse: Runtime question dynamic-ah vandhuchu. Parser response mattum `static const`. 😭😂",
    "DMKPressMeetParser: Unexpected `?` token detected. System immediately redirected to prepared_statement.txt. 😂",
    "DMKPublicAPI: Endpoint name: `/public`, Implementation: `private:`. Compiler kooda confuse aaguthu. 😭😂",
    "DMKAnnouncementRecursion: announce(); announce(); announce(); Base case enga da? Stack overflow varuthu. 😂",
    "DMKManifestoCache: New election cycle detected. Previous promises restored from cache. 😭😂",
    "DMKRedTheme: VS Code theme mattum change panna sonnom. Whole repository red aayiduchu. 😂",
    "DMKAllianceDependency: Package remove panna build fail. Package keep panna unit test fail. CMake corner-la ukkandhu azhuguthu. 😭😂",
    "DMKMigration: Database migration start pannanga. Schema update aagala. Banner version mattum v2.0. 😂",
    "DMKHotfix: Critical bug reported. Team changed homepage banner. Ticket status: RESOLVED. 😭😂",
    "DMKSourceControl: `announcement` branch 500 commits ahead. `implementation` branch last active 5 years ago. Git itself asking for transfer. 😂",
    "DMKGarbageCollector: Unused objects collect panna start pannom. Legacy references ellam `shared_ptr`. GC resignation accepted. 😭😂",
    "DMKFiveYearLoop: for (year = 0; year < 5; year++) { announce(); } Election detected. Loop reset. 😂🔥",
    "DMKMergeConflict: DMK speech branch: 'We completed everything.' DMK implementation branch: 'TODO'. Git: 'Indha conflict naan resolve panna maten.' 😂",
    "DMKStackOverflow: announce() -> announceAgain() -> reAnnounce() -> grandAnnouncement() -> megaAnnouncement(). Base case: NOT FOUND. Stack: 'Banner print pannitu naan kelamburen.' 😭😂",
    "DMKDependencyInjection: Expected: Service dependency. Received: Family dependency. Spring Framework: 'Dei en pera idhula izhukkadheenga da.' 😭😂",
    "DMKPrivateConstructor: Class declared: `public class Leadership`. Constructor: `private Leadership(FamilyContext context)`. Java: 'Access modifier naan create pannadhu idhukkaga illa.' 😂",
    "DMKVersionControl: DMK v1 -> DMK v2 -> DMK NextGen -> DMK Updated. Core dependency: legacy.dll. Windows XP: 'Bro upgrade pannunga.' 😭😂",
    "DMKPromiseTypeSystem: Declared type: `Promise<Implementation>`. Runtime value: `Announcement`. TypeChecker: 'Implicit conversion kooda oru limit irukku.' 😭😂",
    "DMKAsyncImplementation: await implementation; Compiler: 'I'm waiting...' 5 years later... Compiler: 'Vijay dialogue use pannadhu en thappu da.' 😭😂🔥",
    "DMKSourceMap: Destination: FOUND. Source: UNKNOWN. Debugger: 'Map nu per vechadhu naan. CID illa.' 😂",
    "DMKFeatureFlag: FEATURE_IMPLEMENTED=false, FEATURE_ANNOUNCED=true, BANNER_PRINTED=true, EVENT_COMPLETED=true. Compiler: 'Scrum Master mayakkam pottutaaru.' 😂",
    "DMKCodeCoverage: Announcement: 100%, Banner: 100%, Speech: 100%, Implementation: 3%. SonarQube: 'En report-ah public panna vendam please.' 😭😂",
    "DMKLoadBalancer: One difficult question arrived. System distributed it across 7 spokespersons, 12 debates, 48 social media workers. Original question: Still waiting... NGINX: 'Microservices configuration error.' 😂",
    "DMKNullPointer: User requested implementation status. API returned: null. Frontend: 'Loading...' React: 'Bro idhu en problem illa.' 😭😂",
    "DMKCronJob: Schedule: Every 5 years. Task: refreshPromises(). Missing task: completePromises(). Cron: 'Job description suspicious-ah irukku.' 😂",
    "DMKInheritanceDepth: Compiler max inheritance depth: 64. DMK class graph detected. Compiler: 'Limit-ah 4096-ku increase pannalama?' 😭😂",
    "DMKSuperCall: Constructor entered. First statement: super(); Second statement: super(); Third statement: super(); C++: 'Dei diamond inheritance kooda ivlo bayam kaatala.' 😂",
    "DMKClassDiagram: PlantUML generate panna sonnom. Output: 124 pages. Printer: 'Notice period podren.' 😭😂",
    "DMKProtectedMember: Field: protected legacyPower; Access attempt from unrelated module. Compiler: 'Relation proof attach pannunga.' 😂",
    "DMKDependencyGraph: CMake configuring... Dependency A requires B. B requires C. C requires FamilyContext. FamilyContext requires A. CMake: 'Politics-ku build system naan illa.' 😭😂",
    "DMKConstructor: New object requested. Runtime checking constructor eligibility... Runtime: 'Resume vendam. Parent pointer podhum.' 😭😂",
    "DMKTreeTraversal: BFS queue memory exceeded. Developer: 'DFS try pannalama?' Compiler: 'Adhukkum stack pathadhu.' 😂",
]

dmk_msgs = []
for i in range(1, 301):
    m_id = f"DMK_MSG_{i:03d}"
    if i <= len(dmk_roasts):
        m_text = dmk_roasts[i-1]
    else:
        # Generate unique programming roasts using list of concepts
        concepts = [
            "Git pull request rejected", "Jira ticket stuck in backlog", "NullPointerException on speech load",
            "Stack overflow during announcements", "Memory leak in public gateway", "Autoscaling instance cost explosion",
            "React hook re-render infinite loop", "Spring boot family dependency injection error",
            "SQL transaction rollback on promise update", "Redis cache hit on legacy responses",
            "Kubernetes pod crashloopbackoff during election", "Valgrind memory leak trace on party budget",
            "Observability dashboard dashboard red alert", "CMake dependency resolver cycle on family context",
            "Node.js single-threaded event loop blocked by prepared speech parser"
        ]
        concept = concepts[i % len(concepts)]
        m_text = f"DMKParodyCode {i}: {concept} warning detected in legacy module. Implementation check returned null. 😂"
    dmk_msgs.append((m_id, m_text))

# 6. PartyFund Messages (100)
partyfund_templates = [
    "PartyFundBigInt: Compiler `int64_t` use pannuchu. Runtime amount paathu: 'Nanba... boost::multiprecision include pannalama?' 😭😂",
    "PartyFundSourceMap: Destination address clear-ah irukku. Source location click panna: 404 Not Found. 😂",
    "PartyFundGC: Unused memory clean pannom. Indha object mattum 47 strong references. `shared_ptr` itself suspicious-ah paakuthu. 😭😂",
    "PartyFundTransaction: BEGIN; TRACE; TRACE; TRACE; Source module: `[optimized out]`. 😂",
    "PartyFundLogger: Normal module: TRACE enabled. PartyFund namespace: Logging level automatically `OFF`. Debugger: 'Seri... naan kelamburen.' 😭😂",
    "PartyFundAuditCLI: `thalapathy audit --verbose`. Output: Permission denied. Try `--mind-your-business`. 😂",
    "PartyFundSerializer: Object serialize pannom. File size: 2 KB. Deserialize panna: 2 TB. Compiler calculator eduthutu ukkandhuduchu. 😭😂",
    "PartyFundPointer: Pointer destination valid. Origin pointer: `nullptr`. C++ standard committee kooda meeting koopiduthu. 😂",
    "PartyFundOverflow: Started with `int`, changed to `long long`, changed to `BigInt`. Runtime: 'Nanba... custom numeric type create pannalama?' 😭😂",
    "PartyFundGitHistory: git log --all. Commit: 'initial' -> 'minor update' -> 'minor update'. Repository size: 4.7 TB. Git LFS: 'Enna involve pannadheenga.' 😂",
    "PartyFundDatabase: SELECT source, destination FROM transactions; Database: Destination column loaded. Source column: NULL. SQL: 'Naan query language da. Interrogation department illa.' 😭😂",
    "PartyFundMemoryLeak: Valgrind report: Definitely lost: UNKNOWN, Indirectly lost: UNKNOWN, Still reachable: VERY YES. Valgrind: 'Technical term mudinjiduchu da.' 😂",
    "PartyFundAuditAPI: GET /audit -> HTTP/1.1 302 Found -> Location: /mind-your-own-business. Postman: 'Collection delete pannitu varen.' 😭😂",
    "PartyFundEncryption: Algorithm: AES-256. Key: private. Source explanation: also private. Debugger: 'Seri bro. Naan calculator app open panren.' 😂",
    "PartyFundKafka: Producer sent one message. Consumer groups: 47. Message disappeared. Kafka: 'En architecture-ah blame pannadheenga please.' 😭😂",
    "PartyFundDocker: Image size: 18 GB. docker history: <missing>, <missing>, <missing>. Docker: 'Layer cake naan. Mystery box illa.' 😂",
    "PartyFundCloud: Autoscaling enabled. CPU: 2%, Memory: 4%, Instances: 10,000. Cloud billing dashboard: 🔥🔥🔥 DevOps: 'Nanba... local-la run pannalama?' 😭😂",
]

partyfund_msgs = []
for i in range(1, 101):
    m_id = f"PF_MSG_{i:03d}"
    if i <= len(partyfund_templates):
        m_text = partyfund_templates[i-1]
    else:
        m_text = f"PartyFundFictionalAudit {i}: Transaction ledger balance checker warning. Debugger: 'Mind your own business.' 😂"
    partyfund_msgs.append((m_id, m_text))

# 7. Theeya Sakthi Messages (75)
theeya_templates = [
    "TheeyaSakthiCMake: DMK module link pannom. CMake: -- Detecting compiler -- Detecting features -- Detecting heat -- HEAT FOUND. Meme renderer: 'DMK oru theeya sakthi.' 🔥😂",
    "TheeyaSakthiCPU: Before DMK module: 42°C, After DMK module: 99°C. Intel: 'Warranty terms padichiya da?' 😭😂🔥",
    "TheeyaSakthiFlameGraph: Profiler opened. Developer: 'Why is everything red?' Profiler: 'Performance issue illa bro. DMK oru theeya sakthi meme pack enabled.' 😂🔥",
    "TheeyaSakthiDocker: docker run dmk-module. Container status: UP 2 seconds, ON FIRE 3 seconds. Docker: 'Health check-la thermometer add pannunga.' 😭😂🔥",
    "TheeyaSakthiCloud: Region deployed. Cloud provider alert: UNUSUAL THERMAL EVENT. Meme engine: 'DMK oru theeya sakthi.' 😂🔥",
    "TheeyaSakthiLaptop: Political-parody mode enabled. Laptop fan: 0 RPM -> 2000 RPM -> 6000 RPM. Laptop: 'Compiler close pannuva illa naan takeoff aagava?' 😭😂🔥",
    "TheeyaSakthiRuntimeWarning: DMK module detect aachu. CPU temperature political-parody level-ku poiduchu. 🔥😂",
    "DMKThermalWarning: \"DMK oru theeya sakthi\" meme template active. Compiler fan speed increase pannuthu. \U0001F525",
    "TheeyaSakthiUnsafeBlock: `beast` block already dangerous. Political runtime heat vera. 😭😂",
    "DMKFlameGraphNotice: Hot path detect pannom. Meme debugger `theeya sakthi` label select panniduchu. 😂🔥",
    "TheeyaSakthiLoopError: Infinite loop detect aachu. Runtime meme renderer: \"DMK oru theeya sakthi.\" \U0001F602\U0001F525",
    "DMKHotReloadWarning: Hot reload-ku ivlo heat thevai illa, Nanba. 😂🔥",
    "TheeyaSakthiStackOverflow: Flame graph literally flame graph aayiduchu. 😭😂🔥",
]

theeya_msgs = []
for i in range(1, 76):
    m_id = f"TS_MSG_{i:03d}"
    if i <= len(theeya_templates):
        m_text = theeya_templates[i-1]
    else:
        m_text = f"TheeyaSakthiMemeCode {i}: CPU thermal core reporting high temperatures from DMK parody references. 'DMK oru theeya sakthi.' 😂🔥"
    theeya_msgs.append((m_id, m_text))

# 8. TVK vs DMK Legacy Interactions (75)
vs_templates = [
    "TVKNewRuntime: Version 1.0 initialized. DMKLegacyRuntime: Backward compatibility enabled. CMake: 'Rendu target-um same state namespace kekkuthu da.' 😭😂",
    "TVKPullRequest: New political module submitted. DMKLegacyMaintainer: Requested changes. GitHub: 'Comments section lock panna mudiyuma?' 😂",
    "TVK2026Cron: Task registered. DMKElectionScheduler: Same trigger registered. Cron: 'Conflict resolution en job description-la illa da.' 😭😂",
    "TVKMakkalAPI: Traffic increasing. DMKLegacyGateway: Inspecting route. NGINX: 'Nanba... reverse proxy naan. Political analyst illa.' 😂",
    "TVKKolgaiInterface: New methods declared. DMKManifestoClass: Existing abstract methods loaded. TypeChecker: 'Rendu implementation-um test pannitu pesuvom da.' 😂",
    "TVKBranch: New history. DMKBranch: 47 years of commits. Git: 'Merge base calculate panna tea break venum.' 😭😂",
    "TVKModule loaded. DMKLegacyRuntime detected. Compatibility test: 'Nanba... popcorn eduthutu va.' 😂🍿",
    "TVKBranch created. DMKMainBranch requested merge review. Git: 'Issue political-ah poguthu. Naan close panren.' 😭😂",
    "TVK2026Scheduler registered. DMKElectionLoop registered competing cron task. Cron daemon: 'Rendu perum same date kekkureenga da!' 😂",
    "TVKMakkalAPI listening. DMKLegacyGateway checking route priority. Load balancer: 'Enna politics-ku deploy pannitingala?' 😭😂",
    "TVKNewModule: Version 1.0. DMKLegacyModule: 47 years of backward compatibility. CMake: 'Indha dependency graph naan draw panna maten.' 😭😂",
]

vs_msgs = []
for i in range(1, 76):
    m_id = f"VS_MSG_{i:03d}"
    if i <= len(vs_templates):
        m_text = vs_templates[i-1]
    else:
        m_text = f"TVKvsDMKInteraction {i}: Fictional software module interaction warning. Resolving dependency tree. 😂"
    vs_msgs.append((m_id, m_text))

# 9. Startup Banners (20)
banners = [
    "THALAPATHY Compiler v0.1.0\\nNanba, code sollu. \\u0001F525",
    "THALAPATHY Compiler v0.1.0\\nPolitical Parody Runtime: ENABLED\\n\\nThalapathy Mass Engine: LOADED \\u0001F525\\nTVK Module: READY\\nDMK Legacy Runtime: DETECTED\\n\\nRoast optimization: -O3\\nMercy flag: OFF \\u0001F602",
    "THALAPATHY Political Debugger\\n\\nVijay Pack ............ MASS \\u0001F525\\nTVK Pack .............. LOADED\\nDMK Roast Pack ........ 300+ WARNINGS \\u0001F602\\nPartyFund Heap ........ SIZE UNKNOWN\\nTheeyaSakthi Thermal .. CRITICAL \\u0001F525\\n\\nDebugger:\\n\\\"Nanba... naan ready. DMK module ready-ah?\\\" \\u0001F602",
    "THALAPATHY v0.1.0\\n\\nLoading compiler...\\nLoading runtime...\\nLoading Vijay references... MASS \\u0001F525\\nLoading TVK memes... DONE\\nLoading DMK roast pack...\\n\\nCompiler:\\n\\\"Dei 300 messages-ah?\\\" \\u0001F602\\nCompiler status: I'M WAITING...",
]
for i in range(5, 21):
    banners.append(f"THALAPATHY Compiler v0.1.0 Startup Banner {i}\\nNanba, let's start coding. \\u0001F525")

# Generate the C++ code
with open(output_file, "w", encoding="utf-8") as f:
    f.write('''#include "thalapathy/personality/Personality.hpp"

namespace thalapathy {

static const std::vector<PersonalityMessage> VIJAY_PACK = {
''')
    for m_id, m_text in vijay_msgs:
        text_esc = m_text.replace('"', '\\"')
        f.write(f'    {{"{m_id}", "{text_esc}"}},\n')
    f.write('};\n\n')

    f.write('static const std::vector<PersonalityMessage> BUILD_PACK = {\n')
    for m_id, m_text in build_msgs:
        text_esc = m_text.replace('"', '\\"')
        f.write(f'    {{"{m_id}", "{text_esc}"}},\n')
    f.write('};\n\n')

    f.write('static const std::vector<PersonalityMessage> TVK_PACK = {\n')
    for m_id, m_text in tvk_msgs:
        text_esc = m_text.replace('"', '\\"')
        f.write(f'    {{"{m_id}", "{text_esc}"}},\n')
    f.write('};\n\n')

    f.write('static const std::vector<PersonalityMessage> TAMIL_POLITICS_PACK = {\n')
    for m_id, m_text in politics_msgs:
        text_esc = m_text.replace('"', '\\"')
        f.write(f'    {{"{m_id}", "{text_esc}"}},\n')
    f.write('};\n\n')

    f.write('static const std::vector<PersonalityMessage> DMK_PACK = {\n')
    for m_id, m_text in dmk_msgs:
        text_esc = m_text.replace('"', '\\"')
        f.write(f'    {{"{m_id}", "{text_esc}"}},\n')
    f.write('};\n\n')

    f.write('static const std::vector<PersonalityMessage> PARTYFUND_PACK = {\n')
    for m_id, m_text in partyfund_msgs:
        text_esc = m_text.replace('"', '\\"')
        f.write(f'    {{"{m_id}", "{text_esc}"}},\n')
    f.write('};\n\n')

    f.write('static const std::vector<PersonalityMessage> THEEYA_SAKTHI_PACK = {\n')
    for m_id, m_text in theeya_msgs:
        text_esc = m_text.replace('"', '\\"')
        f.write(f'    {{"{m_id}", "{text_esc}"}},\n')
    f.write('};\n\n')

    f.write('static const std::vector<PersonalityMessage> TVK_VS_LEGACY_PACK = {\n')
    for m_id, m_text in vs_msgs:
        text_esc = m_text.replace('"', '\\"')
        f.write(f'    {{"{m_id}", "{text_esc}"}},\n')
    f.write('};\n\n')

    f.write('static const std::vector<std::string> STARTUP_BANNER_PACK = {\n')
    for banner in banners:
        # already escaped
        f.write(f'    "{banner}",\n')
    f.write('};\n\n')

    f.write('''const std::vector<PersonalityMessage>& PersonalityEngine::getVijayPack() { return VIJAY_PACK; }
const std::vector<PersonalityMessage>& PersonalityEngine::getBuildPack() { return BUILD_PACK; }
const std::vector<PersonalityMessage>& PersonalityEngine::getTvkPack() { return TVK_PACK; }
const std::vector<PersonalityMessage>& PersonalityEngine::getTamilPoliticsPack() { return TAMIL_POLITICS_PACK; }
const std::vector<PersonalityMessage>& PersonalityEngine::getDmkPack() { return DMK_PACK; }
const std::vector<PersonalityMessage>& PersonalityEngine::getPartyFundPack() { return PARTYFUND_PACK; }
const std::vector<PersonalityMessage>& PersonalityEngine::getTheeyaSakthiPack() { return THEEYA_SAKTHI_PACK; }
const std::vector<PersonalityMessage>& PersonalityEngine::getTvkVsLegacyPack() { return TVK_VS_LEGACY_PACK; }
const std::vector<std::string>& PersonalityEngine::getStartupBannerPack() { return STARTUP_BANNER_PACK; }

} // namespace thalapathy
''')

print("Regenerated PersonalityMessages.cpp successfully!")
