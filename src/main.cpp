#include "main.hpp"
#include "practiceUI.hpp"
#include "extern/codegen/include/GlobalNamespace/IBeatmapLevel.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/utils.h"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Vector3.hpp"



// pink cute

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

Logger& getLogger() {

    static Logger* logger = new Logger(modInfo);
    return *logger;
}

practice_state_t STATE = {0};
PracticeMenuUI practiceUI;

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

// Returns a logger, useful for printing debug messages


template<class T>
T FindObjectsOfTypeAllFirstOrDefault() {
    Array<T> *objects = UnityEngine::Resources::FindObjectsOfTypeAll<T>();
    if (objects)
        return objects->values[0];
    return nullptr;
}

MAKE_HOOK_OFFSETLESS(BOSMD_Init, void, Il2CppObject* self, int noteLinesCount, float startNoteJumpMovementSpeed, float startBPM, float noteJumpStartBeatOffset, float jumpOffsetY, UnityEngine::Vector3 rightVec, UnityEngine::Vector3 forwardVec) {
    getLogger().info("BOSMD_Init");
    BOSMD_Init(self,
               noteLinesCount,
               STATE.inPracticeMode ? STATE.customNjs : startNoteJumpMovementSpeed,
               startBPM,
               STATE.inPracticeMode ? STATE.customOffset : noteJumpStartBeatOffset,
               jumpOffsetY,
               rightVec,
               forwardVec);
}

MAKE_HOOK_OFFSETLESS(LevelSelectionFlowCoordinator_StartLevel, void,  Il2CppObject* self, Il2CppObject* beforeSceneSwitchCallback, bool practice) {
    getLogger().info("LSFC_StartLevel");
    STATE.inPracticeMode = practice;
    LevelSelectionFlowCoordinator_StartLevel(self, beforeSceneSwitchCallback, practice);
}

MAKE_HOOK_OFFSETLESS(PracticeViewController_DidActivate, void, GlobalNamespace::PracticeViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    getLogger().info("PVC_DidActivate");

    GlobalNamespace::StandardLevelDetailView *sldv = FindObjectsOfTypeAllFirstOrDefault<GlobalNamespace::StandardLevelDetailView*>();
    STATE.defaultOffset = sldv->get_selectedDifficultyBeatmap()->get_noteJumpStartBeatOffset();
    STATE.difficulty = sldv->get_selectedDifficultyBeatmap()->get_difficultyRank();
    STATE.defaultNjs = sldv->get_selectedDifficultyBeatmap()->get_noteJumpMovementSpeed();
    STATE.levelID = to_utf8(csstrtostr(reinterpret_cast<GlobalNamespace::IPreviewBeatmapLevel*>(self->level)->get_levelID()));

    PracticeViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    practiceUI.createUI();
}

extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;

    getConfig().Load();
    getLogger().info("Completed setup!");
}

extern "C" void load() {
    il2cpp_functions::Init();
    INSTALL_HOOK_OFFSETLESS(getLogger(), PracticeViewController_DidActivate, il2cpp_utils::FindMethodUnsafe("", "PracticeViewController", "DidActivate", 3));
    INSTALL_HOOK_OFFSETLESS(getLogger(), LevelSelectionFlowCoordinator_StartLevel, il2cpp_utils::FindMethodUnsafe("", "SinglePlayerLevelSelectionFlowCoordinator", "StartLevel", 2));
    INSTALL_HOOK_OFFSETLESS(getLogger(), BOSMD_Init, il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectSpawnMovementData", "Init", 7));
}
