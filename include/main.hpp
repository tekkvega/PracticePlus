#pragma once

#include "modloader/shared/modloader.hpp"

#include <unordered_set>

#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"

#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "HMUI/NoTransitionsButton.hpp"
#include "Polyglot/LocalizedTextMeshProUGUI.hpp"
#include "UnityEngine/UI/Button_ButtonClickedEvent.hpp"
#include "UnityEngine/Events/UnityAction.hpp"
#include "TMPro/TextOverflowModes.hpp"

#include "GlobalNamespace/PracticeViewController.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/PauseMenuManager.hpp"



// Define these functions here so that we can easily read configuration and log information from other files
Configuration& getConfig();
Logger& getLogger();

template<class T>
T FindObjectsOfTypeAllFirstOrDefault();

typedef struct practice_state {
    float customNjs;
    float customOffset;
    float defaultNjs;
    float defaultOffset;
    int difficulty;
    bool inPracticeMode;
    std::string levelID;
} practice_state_t;
extern practice_state_t STATE;

class CustomButton {
    public:
        HMUI::CurvedTextMeshPro* TMP = nullptr;
        UnityEngine::GameObject* button = nullptr;
        UnityEngine::Transform* buttonTransform = nullptr;
        UnityEngine::GameObject* parent = nullptr;
        UnityEngine::Transform* parentTransform = nullptr;
        Polyglot::LocalizedTextMeshProUGUI* TMPLocalizer = nullptr;
        UnityEngine::Vector3 relativePos = {0, 0, 0};
        UnityEngine::Vector3 scale = {1.0f, 1.0f, 1.0f};
        UnityEngine::Vector3 rotation = {0, 0, 0};
        float fontSize = 4.0f;
        std::string text = "Custom Button UI";
        function_ptr_t<void> onPress;

        bool isCreated = false;

        // void setParentAndTransform(UnityEngine::GameObject* Obj, int parentedAmount) {
        //     parent = Obj;

        //     if(parentedAmount < 1) {
        //         parentedAmount = 1;
        //     }
        //     Il2CppObject* transform = *RunMethod(Obj, "get_transform");
        //     std::vector<Il2CppObject*> parents;
        //     Il2CppObject* firstParent = *RunMethod(transform, "GetParent");
        //     parents.push_back(firstParent);
        //     for(int i = 1; i < parentedAmount; i++) {
        //         Il2CppObject* otherParent = *RunMethod(parents[i-1], "GetParent");
        //         parents.push_back(otherParent);
        //     }
        //     parentTransform = parents[parents.size()-1];
        // }

        // void setParentTransform(Il2CppObject* Obj, int parentedAmount) {
        //     if(parentedAmount < 1) {
        //         parentedAmount = 1;
        //     }
        //     Il2CppObject* transform = *RunMethod(Obj, "get_transform");
        //     std::vector<Il2CppObject*> parents;
        //     Il2CppObject* firstParent = *RunMethod(transform, "GetParent");
        //     parents.push_back(firstParent);
        //     for(int i = 1; i < parentedAmount; i++) {
        //         Il2CppObject* otherParent = *RunMethod(parents[i-1], "GetParent");
        //         parents.push_back(otherParent);
        //     }
        //     parentTransform = parents[parents.size()-1];
        // }

        void create() {
            if(!isCreated) {
                button = UnityEngine::UI::Button::Instantiate(parent);
                buttonTransform = button->get_transform();
                buttonTransform->set_parent(parentTransform);
                buttonTransform->set_localScale(scale);
                buttonTransform->set_localPosition(relativePos);

                TMP = button->GetComponentInChildren<HMUI::CurvedTextMeshPro*>();
                TMPLocalizer = button->GetComponentInChildren<Polyglot::LocalizedTextMeshProUGUI*>();
                UnityEngine::Object::Destroy(TMPLocalizer);

                TMP->set_text(il2cpp_utils::createcsstr(text));
                TMP->set_overflowMode(TMPro::TextOverflowModes::Overflow);
                TMP->set_fontSize(fontSize);

                // Undo the text scaling that was done when the overall button was scaled
                TMP->get_transform()->set_localScale(UnityEngine::Vector3(1.0/scale.x, 1.0/scale.y, 1.0));

                HMUI::NoTransitionsButton *ui_button = button->GetComponentInChildren<HMUI::NoTransitionsButton*>();
                auto actionToRun = il2cpp_utils::MakeDelegate(il2cpp_functions::class_get_type(il2cpp_utils::GetClassFromName("UnityEngine.Events", "UnityAction")), (Il2CppObject*)nullptr, onPress);
                ui_button->get_onClick()->AddListener(reinterpret_cast<UnityEngine::Events::UnityAction*>(actionToRun));
                isCreated = true;
            } else {
                getLogger().info("Already created.");
            }
        }

        void setText(std::string newText) {
            TMP->set_text(il2cpp_utils::createcsstr(newText));
        }

        void setActive(bool isActive) {
            button->SetActive(isActive);
        }

        void setPos(UnityEngine::Vector3 pos) { // Doesnt work yet, work on it more later
            buttonTransform->set_localPosition(pos);
        }

        void destroy() {
            if(button != nullptr) {
                UnityEngine::Object::Destroy(button);
                button = nullptr;
                TMP = nullptr;
                parentTransform = nullptr;
                parent = nullptr;
                TMPLocalizer = nullptr;
                isCreated = false;
            } else {
                getLogger().info("Already destroyed.");
            }
        }
};
