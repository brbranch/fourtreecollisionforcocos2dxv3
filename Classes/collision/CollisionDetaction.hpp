/**
 ForTreeCollision v0.0.1
 CollisionDetection.hpp

 cocos2d-x 3.x用の4分木衝突判定クラス。
 以下公開されているソースコードをcocos2d-x v3用に修正＆拡張したものです。
 https://github.com/gear1554/CollisionDetectionTest

 Copyright (c) 2016 Kazuki Oda

 This software is released under the MIT License.
 http://opensource.org/licenses/mit-license.php
*/

#ifndef ForTreeCollisionDetection_hpp
#define ForTreeCollisionDetection_hpp

#include <stdio.h>
#include <unordered_set>
#include "cocos2d.h"
#define CLINER4TREEMANAGER_MAXLEVEL 9

/**
 * 衝突判定のノード。
 */
class CollisionNode : public cocos2d::Ref {
   private:
    /** 衝突判定オブジェクト */
    CC_SYNTHESIZE_READONLY(cocos2d::Node*, m_node, Node);
    /** グループ */
    CC_SYNTHESIZE_READONLY(unsigned int, m_group, Group);

   public:
    /** デストラクタ。 */
    virtual ~CollisionNode() {
        CC_SAFE_RELEASE_NULL(m_node);
    }
    /** コンストラクタ。 */
    CollisionNode() : m_node(nullptr), m_group(0) {
    }
    /** 同一グループかどうか */
    bool is(unsigned int group) {
        return m_group == group;
    }

    /** BitMaskを返す。 */
    unsigned int bitmask(unsigned int bit) {
        return m_group & bit;
    }

    /** 同一グループならtrueを返す。 */
    bool isSameGroup(CollisionNode* node) {
        return m_group == node->m_group;
    }

    /** nodeをフィールドから除去する。 */
    void removeFromParent() {
        if (m_node) {
            m_node->removeFromParent();
        }
    }

    /** インスタンスを生成する。 */
    static CollisionNode* create(cocos2d::Node* node, unsigned int group) {
        auto ref = new CollisionNode();
        node->retain();
        ref->autorelease();
        ref->m_node = node;
        ref->m_group = group;
        return ref;
    }
};

/**
 * 衝突判定ノードのリスト。
 */
class CollisionNodeList : public cocos2d::Ref {
   private:
    /** ノードリスト */
    cocos2d::Vector<CollisionNode*> m_list;

   public:
    /** インスタンスを生成する。 */
    CREATE_FUNC(CollisionNodeList);
    /** リストの数を得る。 */
    size_t count() const {
        return m_list.size();
    }
    /** リストをクリアする。 */
    void clear() {
        m_list.clear();
    }
    /** イテレータの定義 */
    cocos2d::Vector<CollisionNode*>::iterator begin() {
        return m_list.begin();
    }
    /** イテレータの定義 */
    cocos2d::Vector<CollisionNode*>::iterator end() {
        return m_list.end();
    }
    /** イテレータの定義 */
    cocos2d::Vector<CollisionNode*>::const_iterator begin() const {
        return m_list.begin();
    }
    /** イテレータの定義 */
    cocos2d::Vector<CollisionNode*>::const_iterator end() const {
        return m_list.end();
    }
    /** オブジェクトが存在するか */
    bool containsObject(CollisionNode* node) {
        return m_list.contains(node);
    }
    /** オブジェクトを追加する。 */
    void addObject(CollisionNode* node) {
        m_list.pushBack(node);
    }
    /** インデックスのオブジェクトを返す。 */
    CollisionNode* objectAtIndex(size_t size) {
        return m_list.at(size);
    }
    /** Nodeに紐づくオブジェクトを除去する。 */
    void erase(cocos2d::Node* node) {
        if (auto _node = find(node)) {
            m_list.eraseObject(_node);
        }
    }
    /** Nodeに紐づくオブジェクトを探す。 */
    CollisionNode* find(cocos2d::Node* node) {
        auto it = std::find_if(
            m_list.begin(), m_list.end(),
            [node](CollisionNode* one) { return one->getNode() == node; });
        if (it != end()) {
            return *it;
        }
        return nullptr;
    }

    /** オブジェクトを除去する。 */
    void erase(CollisionNode* node) {
        m_list.eraseObject(node);
    }
    /** 最後のオブジェクトを除去する。 */
    void removeLastObject() {
        m_list.popBack();
    }

   private:
    bool init() {
        return true;
    }
};

/**
 * 衝突判定ノードのマップ。
 */
class CollisionNodeMap : public cocos2d::Ref {
   private:
    /** マップ */
    cocos2d::Map<size_t, CollisionNodeList*> m_list;
    /** マップのサイズ */
    size_t m_size;

   public:
    /** コンストラクタ。 */
    CollisionNodeMap() : m_list(), m_size(0){};
    /** インスタンスを生成する。 */
    CREATE_FUNC(CollisionNodeMap);
    /** 配列内のリスト内要素をクリアする。 */
    void refresh() {
        for (auto space : m_list) {
            space.second->clear();
        }
    }
    /** 全てのオブジェクトを除去する。 */
    void removeAllObjects() {
        m_list.clear();
        m_size = 0;
    }
    /** オブジェクトを追加する。 */
    void addObject(CollisionNodeList* list) {
        m_list.insert(m_size, list);
        m_size++;
    }
    /** 要素数を返す。 */
    size_t count() {
        return m_size;
    }
    /** 要素を得る。 */
    CollisionNodeList* get(size_t index) {
        return m_list.at(index);
    }

   private:
    bool init() {
        return true;
    }
};

/** 衝突判定用の関数定義 */
using DetectCollisionFunc = std::function<void(CollisionNode*, CollisionNode*)>;
/**
 * 4分木衝突判定クラス。
 */
class CollisionDetaction : public cocos2d::Ref {
   public:
    /**
     * コンストラクタ。
     */
    CollisionDetaction();

    /**
     * デストラクタ。
     */
    ~CollisionDetaction();

   private:
    /** 衝突判定 */
    DetectCollisionFunc m_func;
    /** m_field内のexit時のキャッシュ */
    std::function<void()> m_onexit;
    /** 衝突判定フィールド */
    cocos2d::Node* m_field;
    /** 4分木分割配列 */
    CollisionNodeMap* m_spaceArray;
    /** 衝突判定オブジェクトリスト */
    CollisionNodeList* m_gameObjectArray;
    /** 4分木分割の最小セルのサイズ */
    cocos2d::Size m_unitSize;
    /** セルの数 */
    int m_dwCellNum;
    /** 4分木分割のレベル */
    int m_uiLevel;
    /** 4分木分割のレベルに応じた要素数 */
    unsigned int m_iPow[CLINER4TREEMANAGER_MAXLEVEL + 1];

   public:
    /** シングルトンを得る。 */
    static CollisionDetaction* getDefaultDetaction();
    /** 衝突判定クラスを作成する。 */
    static CollisionDetaction* create();
    /**
      初期化。
      @param field 衝突判定フィールドのターゲット
      @param level 4分木分割の階層レベル。レベル9より多くは指定できない
      @param autoclean fieldが親ノードから除去された際に自動で後処理をする
      @param func 衝突判定時の処理

      [Notice]
      autocleanは、fieldのsetonExitTransitionDidStartCallbackを
      使用しています。事前に登録されているものはキャッシュしますが、
      この処理を呼び出した後にfield::setonExitTransitionDidStartCallbackを
      呼び出して登録した場合はリセットされるためautocleanされません。
     */
    virtual bool init(cocos2d::Node* field, int level, bool autoclean,
                      const DetectCollisionFunc& func);
    /** リセット。 */
    virtual void reset();

    /** 衝突判定を更新。 */
    virtual void update();

    /** ノードを追加する。 */
    virtual void add(CollisionNode* node);
    /** ノードを除去する。 */
    virtual void remove(CollisionNode* node);
    /** ノードを除去する。 */
    virtual void remove(cocos2d::Node* node);

   protected:
    /** 4分木空間に配置する。 */
    virtual void updateSpaceStatus(CollisionNode* collisionObject);

    /** 衝突判定を行う。 */
    virtual void scanCollisionDetection(int spaceIndex,
                                        CollisionNodeList* stackArray);

    /** 衝突したかどうかを判別する。 */
    virtual void checkHit(CollisionNode* CollisionObject1,
                          CollisionNode* collisionObject2);

    /** リスト同士で衝突判定を行う。*/
    virtual void checkHitSpaceCell(CollisionNodeList* array1,
                                   CollisionNodeList* array2);
};

#endif /* ForTreeCollisionDetection_hpp */
