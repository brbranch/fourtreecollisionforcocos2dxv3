/**
 ForTreeCollision v0.0.1
 CollisionDetection.cpp

 cocos2d-x 3.x用の4分木衝突判定クラス。
 以下公開されているソースコードをcocos2d-x v3用に修正＆改造したものです。
 https://github.com/gear1554/CollisionDetectionTest

 Copyright (c) 2016 Kazuki Oda

 This software is released under the MIT License.
 http://opensource.org/licenses/mit-license.php
*/

#include "CollisionDetaction.hpp"
#include "CollisionUtils.hpp"
#include "collision_point.h"
USING_NS_CC;
static CollisionDetaction *s_instance = nullptr;

CollisionDetaction::CollisionDetaction()
    : m_field(nullptr)
    , m_unitSize()
    , m_dwCellNum(0)
    , m_uiLevel(0)
    , m_spaceArray(nullptr)
    , m_gameObjectArray(nullptr) {
    int i;
    m_iPow[0] = 1;
    for (i = 1; i < CLINER4TREEMANAGER_MAXLEVEL + 1; i++) {
        m_iPow[i] = m_iPow[i - 1] * 4;
    }
};

CollisionDetaction::~CollisionDetaction() {
    reset();
}

CollisionDetaction *CollisionDetaction::create() {
    auto ref = new CollisionDetaction();
    ref->autorelease();
    return ref;
}

void CollisionDetaction::add(CollisionNode *node) {
    if (m_gameObjectArray == nullptr) {
        return;
    }
    m_gameObjectArray->addObject(node);
}

void CollisionDetaction::remove(cocos2d::Node *node) {
    if (m_gameObjectArray == nullptr) {
        return;
    }
    m_gameObjectArray->erase(node);
}

void CollisionDetaction::remove(CollisionNode *node) {
    if (m_gameObjectArray == nullptr) {
        return;
    }
    m_gameObjectArray->erase(node);
}

void CollisionDetaction::reset() {
    if (m_field) {
        m_field->setonExitTransitionDidStartCallback(m_onexit);
    }
    CC_SAFE_RELEASE_NULL(m_gameObjectArray);
    CC_SAFE_RELEASE_NULL(m_field);
    CC_SAFE_RELEASE_NULL(m_spaceArray);
    m_onexit = nullptr;
    m_func = nullptr;
    CCLOG("reset");
}

bool CollisionDetaction::init(cocos2d::Node *field, int level, bool autoclean,
                              const DetectCollisionFunc &func) {
    CCASSERT(level < CLINER4TREEMANAGER_MAXLEVEL,
             "レベルがMAXLEVELを超えています。");
    reset();

    m_func = func;

    m_gameObjectArray = CollisionNodeList::create();
    m_gameObjectArray->retain();

    m_spaceArray = CollisionNodeMap::create();
    m_spaceArray->retain();

    m_field = field;
    m_field->retain();

    if (autoclean) {
        m_onexit = m_field->getonExitTransitionDidStartCallback();
        auto callback = [this]() {
            if (m_onexit) {
                m_onexit();
            }
            this->reset();
        };
        m_field->setonExitTransitionDidStartCallback(callback);
    }

    Size winSize = field->getContentSize();
    m_dwCellNum = (m_iPow[level + 1] - 1) / 3;
    m_uiLevel = level;
    m_unitSize = Size((float)(winSize.width / (1 << level)),
                      (float)(winSize.height / (1 << level)));
    for (int i = 0; i < m_dwCellNum; i++) {
        auto spaceCellArray = CollisionNodeList::create();
        m_spaceArray->addObject(spaceCellArray);
    }
    return true;
}

// Update Collision detection
void CollisionDetaction::update() {
    if (m_spaceArray == nullptr) {
        return;
    }
    // Refresh space status
    m_spaceArray->refresh();

    if (!m_gameObjectArray || m_gameObjectArray->count() < 2) {
        return;
    }

    // Update game objects's space status
    for (auto node : *m_gameObjectArray) {
        updateSpaceStatus(node);
    }

    // Scan collision detection
    auto ary = CollisionNodeList::create();
    scanCollisionDetection(0, ary);
}
#pragma mark -
#pragma mark 4分木空間計算

/**
 * 4分木空間に配置する。
 */
void CollisionDetaction::updateSpaceStatus(CollisionNode *collisionObject) {
    // レベルを計算(オーバーヘッドを抑えるためあえて外出ししないでいる)
    int level = -1;
    Point pos(collisionObject->getNode()->getPosition());
    Size size(collisionObject->getNode()->getContentSize());
    size.width = size.width / 2 * collisionObject->getNode()->getScaleX();
    size.height = size.height / 2 * collisionObject->getNode()->getScaleY();

    float left = pos.x - size.width;
    float top = pos.y + size.height;
    float right = pos.x + size.width;
    float bottom = pos.y - size.height;
    float width = m_unitSize.width;
    float height = m_unitSize.height;

    int LT = collision_get_point_elem(left, top, width, height);
    int RB = collision_get_point_elem(right, bottom, width, height);

    int Def = RB ^ LT;
    unsigned int HiLevel = 0;
    for (unsigned int i = 0; i < m_uiLevel; i++) {
        int Check = (Def >> (i * 2)) & 0x3;
        if (Check != 0) HiLevel = i + 1;
    }
    level = RB >> (HiLevel * 2);
    int AddNum = (m_iPow[m_uiLevel - HiLevel] - 1) / 3;
    level += AddNum;
    // レベル計算(ここまで)

    if (level < 0 || level >= m_dwCellNum) {
        return;
    }

    auto spaceCellArray = m_spaceArray->get(level);
    if (!spaceCellArray->containsObject(collisionObject)) {
        spaceCellArray->addObject(collisionObject);
    }
}

void CollisionDetaction::scanCollisionDetection(int spaceIndex,
                                                CollisionNodeList *stackArray) {
    if (!m_spaceArray->count()) {
        return;
    }

    auto spaceCellArray = m_spaceArray->get(spaceIndex);

    checkHitSpaceCell(spaceCellArray, spaceCellArray);
    checkHitSpaceCell(spaceCellArray, stackArray);

    bool childFlag = false;
    int ObjNum = 0;
    int nextSpaceIndex;
    for (int i = 0; i < 4; i++) {
        nextSpaceIndex = spaceIndex * 4 + 1 + i;
        // 子空間があるかどうか
        if (nextSpaceIndex >= 0 && nextSpaceIndex < m_dwCellNum) {
            if (!childFlag) {
                for (auto obj : *spaceCellArray) {
                    stackArray->addObject(obj);
                }
                ObjNum += spaceCellArray->count();
            }
            childFlag = true;
            scanCollisionDetection(nextSpaceIndex, stackArray);
        }
    }

    if (childFlag) {
        for (int i = 0; i < ObjNum; i++) {
            stackArray->removeLastObject();
        }
    }

    return;
}

void CollisionDetaction::checkHit(CollisionNode *collisionObject1,
                                  CollisionNode *collisionObject2) {
    if (m_func) {
        m_func(collisionObject1, collisionObject2);
    }
}

void CollisionDetaction::checkHitSpaceCell(CollisionNodeList *array1,
                                           CollisionNodeList *array2) {
    if (array1 == array2) {
        size_t ary1count = array1->count();
        for (int i = 0; i < ary1count; i++) {
            size_t ary2count = array2->count();
            for (int j = i + 1; j < ary2count; j++) {
                checkHit(array1->objectAtIndex(i), array2->objectAtIndex(j));
            }
        }
    } else {
        for (auto obj : *array1) {
            for (auto obj2 : *array2) {
                if (obj != obj2) {
                    checkHit(obj, obj2);
                }
            }
        }
    }
}

CollisionDetaction *CollisionDetaction::getDefaultDetaction() {
    if (s_instance == nullptr) {
        s_instance = new CollisionDetaction();
    }
    return s_instance;
}