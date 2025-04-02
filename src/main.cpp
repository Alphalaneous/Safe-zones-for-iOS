#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/SecretRewardsLayer.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/GauntletSelectLayer.hpp>
#include <alphalaneous.alphas_geode_utils/include/NodeModding.h>
#include <alphalaneous.alphas_geode_utils/include/Utils.h>

using namespace geode::prelude;

bool g_doSafeArea = false;

std::vector<std::string> g_exclusions = {
	"MenuLayer",
	"LevelEditorLayer",
	"DailyLevelPage",
	"SecretRewardsLayer"
};

std::vector<std::string> g_exclusionsClass = {
	"LevelSelectLayer",
	"GauntletSelectLayer",
	"GJPromoPopup"
};

void manualOffset(CCNode* node, float offset) {
	if (!node || !node->getParent() || !g_doSafeArea) return;
	CCPoint worldPosition = node->convertToWorldSpaceAR({0, 0});
	worldPosition.x += offset;
	CCPoint newNodePos = node->getParent()->convertToNodeSpace(worldPosition);
	newNodePos.y = node->getPositionY();

	node->setPosition(newNodePos);
}

void checkPosition(CCNode* node) {
	if (!node || !node->getParent() || utils::string::contains(node->getParent()->getID(), "-navigation-menu") || !g_doSafeArea) return;

	CCPoint worldPosition = node->convertToWorldSpaceAR({0, 0});
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	float scaledWidth = node->getContentSize().width * node->getScaleX();
	CCPoint anchor = node->getAnchorPoint();

	float leftEdge = worldPosition.x - (scaledWidth * anchor.x);
	float rightEdge = worldPosition.x + (scaledWidth * (1.0f - anchor.x));

	float distanceFromLeft = leftEdge;  
	float distanceFromRight = winSize.width - rightEdge;  
	bool isCloserToLeft = (distanceFromLeft < distanceFromRight);

	CCPoint adjustedWorldPos = worldPosition;

	if (isCloserToLeft) {
		if (leftEdge < 40) {
			adjustedWorldPos.x = 40 + (scaledWidth * anchor.x);
		}
	} else {
		if (rightEdge > winSize.width - 40) {
			adjustedWorldPos.x = (winSize.width - 40) - (scaledWidth * (1.0f - anchor.x));
		}
	}

	CCPoint newNodePos = node->getParent()->convertToNodeSpace(adjustedWorldPos);
	newNodePos.y = node->getPositionY();

	node->setPosition(newNodePos);
}

void modifyButtons(CCNode* node) {
	for (CCNode* child : CCArrayExt<CCNode*>(node->getChildren())) {
		if (!child->getUserObject("checked"_spr)) {
			child->setUserObject("checked"_spr, CCBool::create(true));
			
			if (std::find(g_exclusions.begin(), g_exclusions.end(), child->getID()) != g_exclusions.end()) continue;
			if (std::find(g_exclusionsClass.begin(), g_exclusionsClass.end(), AlphaUtils::Cocos::getClassName(child)) != g_exclusionsClass.end()) continue;

			if (std::string_view(child->getID()) == "level-count-label") {
				checkPosition(child);
				continue;
			}
			if (CCMenu* menu = typeinfo_cast<CCMenu*>(child)) {
				if (menu->getLayout()) {
					checkPosition(child);
					continue;
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
		auto scene = CCDirector::sharedDirector()->getRunningScene();
		if (CCTransitionScene* trans = typeinfo_cast<CCTransitionScene*>(scene)) {
			scene = public_cast(trans, m_pInScene);
		}
		if (scene != m_currentScene) {
			m_currentScene = scene;
			m_currentScene->schedule(schedule_selector(SceneHandler::checkForChildrenChange));
		}
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

class $modify(MyGauntletSelectLayer, GauntletSelectLayer) {

    bool init(int p0) {
		if (!GauntletSelectLayer::init(p0)) return false;

		if (CCNode* node = getChildByID("back-menu")) {
			checkPosition(node);
		}

		if (CCNode* node = getChildByID("bottom-left-menu")) {
			checkPosition(node);
		}

		if (CCNode* node = getChildByID("bottom-right-menu")) {
			checkPosition(node);
		}

		if (CCNode* node = getChildByID("top-right-menu")) {
			checkPosition(node);
		}

		if (CCNode* node = getChildByID("scroll-buttons-menu")) {
			if (CCNode* btn = node->getChildByID("left-button")) {
				checkPosition(btn);
			}
			if (CCNode* btn = node->getChildByID("right-button")) {
				checkPosition(btn);
			}
		}


		return true;
	}

};

class $modify(MyLevelInfoLayer, LevelInfoLayer) {

    bool init(GJGameLevel* level, bool challenge) {
		if (!LevelInfoLayer::init(level, challenge)) return false;

		if (CCNode* node = getChildByID("garage-menu")) {
			for (CCNode* btn : CCArrayExt<CCNode*>(node->getChildren())) {
				checkPosition(btn);
			}
		}

		if (CCNode* node = getChildByID("other-menu")) {
			if (CCNode* node2 = node->getChildByID("favorite-button")) {
				manualOffset(node2, 30);
			}
			if (CCNode* node2 = node->getChildByID("move-up-button")) {
				manualOffset(node2, -30);
			}
			if (CCNode* node2 = node->getChildByID("move-down-button")) {
				manualOffset(node2, -30);
			}
			if (CCNode* node2 = node->getChildByID("folder-button")) {
				manualOffset(node2, -30);
			}
			if (CCNode* node2 = node->getChildByID("list-button")) {
				manualOffset(node2, -30);
			}
		}

		if (CCNode* node = getChildByID("actions-menu")) {
			checkPosition(node);
		}

		return true;
	}

};

class $modify(MyEditorPauseLayer, EditorPauseLayer) {

    bool init(LevelEditorLayer* editorLayer) {
		if (!EditorPauseLayer::init(editorLayer)) return false;

		if (CCNode* node = getChildByID("small-actions-menu")) {
			manualOffset(node, -15);
		}

		if (CCNode* node = getChildByID("actions-menu")) {
			checkPosition(node);
		}

		if (CCNode* node = getChildByID("options-menu")) {
			checkPosition(node);
		}

		if (CCNode* node = getChildByID("settings-menu")) {
			manualOffset(node, -18);
		}

		if (CCNode* node = getChildByID("info-menu")) {
			checkPosition(node);
		}

		return true;
	}
};

class $modify(MyEditorUI, EditorUI) {

	static void onModify(auto& self) {
        (void) self.setHookPriority("EditorUI::init", -10000); 
    }

    bool init(LevelEditorLayer* editorLayer) {
		if (!EditorUI::init(editorLayer)) return false;

		if (CCNode* node = getChildByID("zoom-menu")) {
			checkPosition(node);
		}

		if (CCNode* node = getChildByID("link-menu")) {
			manualOffset(node, 30);
		}

		if (CCNode* node = getChildByID("playback-menu")) {
			checkPosition(node);
		}

		if (CCNode* node = getChildByID("playtest-menu")) {
			checkPosition(node);
		}

		if (CCNode* node = getChildByID("editor-buttons-menu")) {
			checkPosition(node);
		}

		if (CCNode* node = getChildByID("layer-menu")) {
			checkPosition(node);
		}

		if (CCNode* node = getChildByID("undo-menu")) {
			checkPosition(node);
		}

		if (CCNode* node = getChildByID("settings-menu")) {
			checkPosition(node);
		}


		return true;
	}
};

class $modify(MyLevelSelectLayer, LevelSelectLayer) {

    bool init(int page) {
		if (!LevelSelectLayer::init(page)) return false;

		if (CCNode* node = getChildByID("info-menu")) {
			for (CCNode* btn : CCArrayExt<CCNode*>(node->getChildren())) {
				checkPosition(btn);
			}
		}

		if (CCNode* node = getChildByID("back-menu")) {
			if (CCNode* btn = node->getChildByID("back-button")) {
				checkPosition(btn);
			}
		}

		if (CCNode* node = getChildByID("arrows-menu")) {
			if (CCNode* btn = node->getChildByID("left-button")) {
				checkPosition(btn);
			}
			if (CCNode* btn = node->getChildByID("right-button")) {
				checkPosition(btn);
			}
		}

		return true;
	}

};

class $modify(MySecretRewardsLayer, SecretRewardsLayer) {

    bool init(bool p0) {
		if (!SecretRewardsLayer::init(p0)) return false;

		if (CCNode* node = getChildByID("exit-menu")) {
			if (CCNode* btn = node->getChildByType<CCMenuItemSpriteExtra*>(0)) {
				checkPosition(btn);
			}
		}

		if (CCNode* node = getChildByID("page-navigation")) {
			if (CCNode* btn = node->getChildByID("left")) {
				checkPosition(btn);
			}
			if (CCNode* btn = node->getChildByID("right")) {
				checkPosition(btn);
			}
		}
		
		return true;
	}
};

class $modify(MyMenuLayer, MenuLayer) {

	bool init() {
		if (!MenuLayer::init()) return false;

		if (CCNode* node = getChildByID("profile-menu")) {
			checkPosition(node);
		}

		if (CCNode* node = getChildByID("right-side-menu")) {
			checkPosition(node);
		}

		if (CCNode* node = getChildByID("profile-button")) {
			checkPosition(node);
		}

		if (CCNode* node = getChildByID("social-media-menu")) {
			checkPosition(node);
		}

		if (CCNode* node = getChildByID("more-games-menu")) {
			checkPosition(node);
		}

		if (CCNode* node = getChildByID("player-username")) {
			checkPosition(node);
		}

		if (CCNode* node = getChildByID("top-right-menu")) {
			checkPosition(node);
		}

		if (CCNode* node = getChildByID("side-menu")) {
			checkPosition(node);
		}

		if (CCNode* node = getChildByID("close-menu")) {
			checkPosition(node);
		}

		return true;
	}

};
	
$execute {
	Loader::get()->queueInMainThread([]{
		CCSize winSize = CCDirector::get()->getWinSize();

		//if (winSize.width > 569) {
			g_doSafeArea = true;
			CCScheduler::get()->scheduleUpdateForTarget(SceneHandler::create(), INT_MAX, false);
		//}
	});
}