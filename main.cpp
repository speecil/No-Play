#include "main.hpp"
#include "MainConfig.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "config-utils/shared/config-utils.hpp"
#include "questui/shared/QuestUI.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace QuestUI;
using namespace custom_types;

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup
DEFINE_CONFIG(MainConfig);
// Loads the config from disk using our modInfo, then returns it for use
// other config tools such as config-utils don't use this config, so it can be removed if those are in use
// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}


void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling){
    if(firstActivation) 
    {
        UnityEngine::GameObject* container = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(self->get_transform());
        QuestUI::BeatSaberUI::CreateText(container->get_transform(), "Enable the No Play Mod here");
        UnityEngine::UI::Toggle* isEnabled = AddConfigValueToggle(container->get_transform(), getMainConfig().isEnabled);
        QuestUI::BeatSaberUI::AddHoverHint(isEnabled->get_gameObject(), "Enable the No Play Mod");
    }
}   
MAKE_HOOK_MATCH(LevelUIHook, &GlobalNamespace::StandardLevelDetailView::RefreshContent, void, GlobalNamespace::StandardLevelDetailView
*self) {
    
    LevelUIHook(self); 
    
    UnityEngine::UI::Button *playMenuButton = self->dyn__actionButton();
    UnityEngine::GameObject *play = playMenuButton->get_gameObject();
    if(getMainConfig().isEnabled.GetValue() == true){
        play->SetActive(false);
        getLogger().info("turned off the mod");
        getLogger().info("turned on the Play Button");
    }
    else if (getMainConfig().isEnabled.GetValue() == false)
    {
        play->SetActive(true);
        getLogger().info("turned on the mod");
        getLogger().info("turned off the Play Button");
    }
    
    
}
// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;
    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();
    getMainConfig().Init(modInfo);
    QuestUI::Init();
    QuestUI::Register::RegisterModSettingsViewController(modInfo, DidActivate);
    QuestUI::Register::RegisterMainMenuModSettingsViewController(modInfo, DidActivate);
    getLogger().info("Installing hooks...");
    INSTALL_HOOK(getLogger(), LevelUIHook);
    getLogger().info("Installed all hooks!");
}