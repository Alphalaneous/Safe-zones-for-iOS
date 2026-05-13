#include <Geode/Geode.hpp>
#include <Geode/binding/AppDelegate.hpp>
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>
#include <alphalaneous.alphas_geode_utils/include/Utils.hpp>

using namespace geode::prelude;

/*bool g_doSafeArea = false;

std::vector<std::string> g_exclusions = {

};

std::vector<std::string> g_exclusionsClass = {
    "LevelSelectLayer",
    "GauntletSelectLayer",
    "GJPromoPopup",
    "EventsPush",
    "RecordLayer",
    "LoadMacroLayer",
    "ModPopup",
    "ModSettingsPopup",
    "RewardsPage",
    "MenuLayer",
    "LevelEditorLayer",
    "DailyLevelPage",
    "SecretRewardsLayer",
    "FLAlertLayer",
    "geode::MDPopup",
    "ConfirmUninstallPopup",
    "FiltersPopup",
    "SortPopup",
    "ModListPopup",
    "QuickPopup",
    "DailyPopup",
    "GlobedUserListPopup",
    "GlobedNotificationPanel",
  	"GlobedLevelFilterPopup",
    "GlobedUserActionsPopup",
    "PrivacySettingsPopup",
    "CreateRoomPopup",
    "AdminUserPopup",
    "AdminSendNoticePopup",
    "AdminPopup",
    "AdminPunishUserPopup",
    "AdminLoginPopup",
    "SuperWorldSelectLayer",
    "GJShopLayer",
    "LevelLeaderboard",
    "ModsLayer",
    "AndroidBall",
    "AndroidUI"
};

void manualOffset(CCNode* node, float offset) {
    if (!node || !node->getParent() || !g_doSafeArea) return;
    auto worldPosition = node->convertToWorldSpaceAR({0, 0});
    worldPosition.x += offset;
    auto newNodePos = node->getParent()->convertToNodeSpace(worldPosition);
    newNodePos.y = node->getPositionY();

    node->setPosition(newNodePos);
}

void manualOffsetIfExists(CCNode* node, std::string_view id, float offset) {
    if (auto child = node->getChildByID(id)) {
        manualOffset(child, offset);
    }
}

void checkPosition(CCNode* node) {
    if (!node || !node->getParent() || utils::string::contains(node->getParent()->getID(), "-navigation-menu") || !g_doSafeArea) return;

    auto worldPosition = node->convertToWorldSpaceAR({0, 0});
    auto winSize = CCDirector::get()->getWinSize();

    float scaledWidth = node->getContentSize().width * node->getScaleX();
    auto anchor = node->getAnchorPoint();

    float leftEdge = worldPosition.x - (scaledWidth * anchor.x);
    float rightEdge = worldPosition.x + (scaledWidth * (1.0f - anchor.x));

    float distanceFromLeft = leftEdge;  
    float distanceFromRight = winSize.width - rightEdge;  
    bool isCloserToLeft = (distanceFromLeft < distanceFromRight);

    auto adjustedWorldPos = worldPosition;

    if (isCloserToLeft) {
        if (leftEdge < 40) {
            adjustedWorldPos.x = 40 + (scaledWidth * anchor.x);
        }
    } else {
        if (rightEdge > winSize.width - 40) {
            adjustedWorldPos.x = (winSize.width - 40) - (scaledWidth * (1.0f - anchor.x));
        }
    }

    auto newNodePos = node->getParent()->convertToNodeSpace(adjustedWorldPos);
    newNodePos.y = node->getPositionY();

    node->setPosition(newNodePos);
}

void checkPositionIfExists(CCNode* node, std::string_view id) {
    if (auto child = node->getChildByID(id)) {
        checkPosition(child);
    }
}

void modifyButtons(CCNode* node) {
    for (auto child : node->getChildrenExt()) {
        if (!child->getUserFlag("checked"_spr)) {
            child->setUserFlag("checked"_spr);
            
            if (std::find(g_exclusions.begin(), g_exclusions.end(), child->getID()) != g_exclusions.end()) continue;
            if (std::find(g_exclusionsClass.begin(), g_exclusionsClass.end(), geode::cocos::getObjectName(child)) != g_exclusionsClass.end()) continue;

            if (typeinfo_cast<geode::MDPopup*>(child)) {
                continue;
            }
            
            if (child->getID() == "level-count-label") {
                checkPosition(child);
                continue;
            }
            if (auto menu = typeinfo_cast<CCMenu*>(child)) {
                if (auto layout = menu->getLayout()) {
                    if (!typeinfo_cast<AnchorLayout*>(layout)) {
                        checkPosition(child);
                        continue;
                    }
                }
            }
            if (typeinfo_cast<CCMenuItem*>(child)) {
                checkPosition(child);
                continue;
            }

            modifyButtons(child);
        }
    }
}

class SceneHandler : public CCNode {
public:
    static SceneHandler* create() {
        auto ret = new SceneHandler();
        ret->autorelease();
        return ret;
    }

    CCScene* m_currentScene = nullptr;
    
    void checkForChildrenChange(float dt) {
        modifyButtons(this);
    }

    void update(float dt) {
        auto scene = CCDirector::get()->getRunningScene();
        if (auto trans = typeinfo_cast<CCTransitionScene*>(scene)) {
            scene = trans->m_pInScene;
        }
        if (scene != m_currentScene) {
            m_currentScene = scene;
            m_currentScene->schedule(schedule_selector(SceneHandler::checkForChildrenChange));
        }
    }
};

class $nodeModify(GJShopLayer) {
    void modify() {
        for (auto node : getChildrenExt()) {
            if (typeinfo_cast<CCMenu*>(node)) {
                modifyButtons(node);
            }
        }
    }
};

class $nodeModify(ModsLayer) {
    void modify() {
        for (auto node : getChildrenExt()) {
            if (typeinfo_cast<CCMenu*>(node)) {
                modifyButtons(node);
            }
        }
        manualOffsetIfExists(this, "page-menu", -30);
    }
};

class $nodeModify(AchievementsLayer) {
    void modify() {
        modifyButtons(this);
    }
};

class $nodeModify(OptionsLayer) {
    void modify() {
        modifyButtons(this);
    }
};

class $nodeModify(GJMoreGamesLayer) {
    void modify() {
        modifyButtons(this);
    }
};

class $nodeModify(StatsLayer) {
    void modify() {
        modifyButtons(this);
    }
};

class $nodeModify(MoreOptionsLayer) {
    void modify() {
        modifyButtons(this);
    }
};

class $nodeModify(GJGarageLayer) {
    void modify() {
        manualOffsetIfExists(this, "stars-label", -30);
        manualOffsetIfExists(this, "stars-icon", -30);
        manualOffsetIfExists(this, "moons-label", -30);
        manualOffsetIfExists(this, "moons-icon", -30);
        manualOffsetIfExists(this, "coins-label", -30);
        manualOffsetIfExists(this, "coins-icon", -30);
        manualOffsetIfExists(this, "user-coins-label", -30);
        manualOffsetIfExists(this, "user-coins-icon", -30);
        manualOffsetIfExists(this, "orbs-label", -30);
        manualOffsetIfExists(this, "orbs-icon", -30);
        manualOffsetIfExists(this, "diamonds-label", -30);
        manualOffsetIfExists(this, "diamonds-icon", -30);
        manualOffsetIfExists(this, "diamond-shards-label", -30);
        manualOffsetIfExists(this, "diamond-shards-icon", -30);
    }
};

class $nodeModify(GauntletSelectLayer) {

    void modify() {
        checkPositionIfExists(this, "back-menu");
        checkPositionIfExists(this, "bottom-left-menu");
        checkPositionIfExists(this, "bottom-right-menu");
        checkPositionIfExists(this, "top-right-menu");

        if (auto node = getChildByID("scroll-buttons-menu")) {
            checkPositionIfExists(node, "left-button");
            checkPositionIfExists(node, "right-button");
        }
    }
};

class $nodeModify(LevelInfoLayer) {

    void modify() {
        if (auto node = getChildByID("garage-menu")) {
            
            float width = 0;
            for (auto btn : node->getChildrenExt()) {
                if (!node->getLayout()) {
                    manualOffset(btn, -30);
                }
                width += btn->getScaledContentWidth() + 5;
            }
            width -= 5;

            node->setContentWidth(width);

            if (node->getLayout()) {
                manualOffset(node, -30);
            }

            node->updateLayout();
        }

        if (auto node = getChildByID("other-menu")) {
            manualOffsetIfExists(this, "favorite-button", 30);
            manualOffsetIfExists(this, "move-up-button", -30);
            manualOffsetIfExists(this, "move-down-button", -30);
            manualOffsetIfExists(this, "folder-button", -30);
            manualOffsetIfExists(this, "list-button", -30);
        }

        checkPositionIfExists(this, "actions-menu");
    }
};

class $nodeModify(EditorPauseLayer) {

    void modify() {
        checkPositionIfExists(this, "actions-menu");
        checkPositionIfExists(this, "options-menu");
        checkPositionIfExists(this, "info-menu");

        if (auto node = getChildByID("small-actions-menu")) {
            manualOffset(node, -15);
        }

        if (auto node = getChildByID("settings-menu")) {
            manualOffset(node, -18);
        }

    }
};

class $nodeModify(EditorUI) {

    void modify() {
        checkPositionIfExists(this, "zoom-menu");
        checkPositionIfExists(this, "playback-menu");
        checkPositionIfExists(this, "playtest-menu");
        checkPositionIfExists(this, "editor-buttons-menu");
        checkPositionIfExists(this, "layer-menu");
        checkPositionIfExists(this, "undo-menu");
        checkPositionIfExists(this, "settings-menu");

        manualOffsetIfExists(this, "link-menu", 30);
        manualOffsetIfExists(this, "object-info-label", 30);
        manualOffsetIfExists(this, "alphalaneous.tinker/length-container", 30);
        manualOffsetIfExists(this, "razoom.named_editor_layers/menu", -30);
    }
};

class $nodeModify(LevelSelectLayer) {

    void modify() {
        if (auto node = getChildByID("info-menu")) {
            for (auto btn : node->getChildrenExt()) {
                checkPosition(btn);
            }
        }

        if (auto node = getChildByID("back-menu")) {
            if (auto btn = node->getChildByID("back-button")) {
                checkPosition(btn);
            }
        }

        if (auto node = getChildByID("arrows-menu")) {
            if (auto btn = node->getChildByID("left-button")) {
                checkPosition(btn);
            }
            if (auto btn = node->getChildByID("right-button")) {
                checkPosition(btn);
            }
        }
    }
};

class $nodeModify(SecretRewardsLayer) {

    void modify() {
        if (auto node = getChildByID("exit-menu")) {
            if (auto btn = node->getChildByType<CCMenuItemSpriteExtra*>(0)) {
                checkPosition(btn);
            }
        }

        if (auto node = getChildByID("page-navigation")) {
            if (auto btn = node->getChildByID("left")) {
                checkPosition(btn);
            }
            if (auto btn = node->getChildByID("right")) {
                checkPosition(btn);
            }
        }
    }
};

class $nodeModify(MenuLayer) {

    void modify() {
        checkPositionIfExists(this, "profile-menu");
        checkPositionIfExists(this, "right-side-menu");
        checkPositionIfExists(this, "profile-button");
        checkPositionIfExists(this, "social-media-menu");
        checkPositionIfExists(this, "more-games-menu");
        checkPositionIfExists(this, "top-right-menu");
        checkPositionIfExists(this, "side-menu");
        checkPositionIfExists(this, "close-menu");

        if (auto node = getChildByID("player-username")) {
            if (auto profile = getChildByID("profile-menu")) {
                if (auto profileButton = profile->getChildByID("profile-button")) {
                    auto world = profileButton->convertToWorldSpaceAR({0, 0});
                    node->setPositionX(world.x);
                }
            }
        }

    }
};
    
$on_mod(Loaded) {
    Loader::get()->queueInMainThread([]{
        CCSize winSize = CCDirector::get()->getWinSize();

        #ifdef GEODE_IS_WINDOWS
            g_doSafeArea = true;
            CCScheduler::get()->scheduleUpdateForTarget(SceneHandler::create(), INT_MAX, false);
        #endif
        if (winSize.width > 569) {
            g_doSafeArea = true;
            CCScheduler::get()->scheduleUpdateForTarget(SceneHandler::create(), INT_MAX, false);
        }
    });
}*/


#include <Geode/modify/AppDelegate.hpp>

class $modify(MyAppDelegate, AppDelegate) {
    void setupGLView() {
        AppDelegate::setupGLView();
        m_unk0ed = CCEGLView::get()->getFrameSize().aspect() == (19.5 / 9);
    }
};
