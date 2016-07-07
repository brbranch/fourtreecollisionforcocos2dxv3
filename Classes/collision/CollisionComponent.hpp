/**
 ForTreeCollision v0.0.1
 CollisionComponent.hpp

 Copyright (c) 2016 Kazuki Oda

 This software is released under the MIT License.
 http://opensource.org/licenses/mit-license.php
*/

#ifndef CollisionComponent_hpp
#define CollisionComponent_hpp

#include <stdio.h>
#include "CollisionDetaction.hpp"
#include "cocos2d.h"

/**
  衝突判定クラスに自身を同期するコンポーネント。
  衝突判定を行うNodeにaddComponentすることで、
  フィールドに追加された際と除去された際に自動で衝突判定クラスへの追加／除去が
  行われます。
  (使用例)
   -----------------------------
   spr = Sprite::create();
   spr->addComponent(CollisionComponent::create());
   field->addChild(spr); // このタイミングでCollisionDetactionに登録される
   field->removeChild(spr); // このタイミングでCollisionDetactionから除去される
   -----------------------------
   (他のremoveChildメソッドや、Node::removeFromParent(),
    RemoveSelfを利用した場合も同様に除去されます)
 */
class CollisionComponent : public cocos2d::Component {
   private:
    /** 衝突判定クラス。 */
    CollisionDetaction* m_detaction;
    /** 衝突判定用ノード。 */
    CollisionNode* m_node;
    /** グループ。敵や味方、弾などの判別として利用 */
    unsigned int m_group;

   public:
    /** コンストラクタ。 */
    CollisionComponent()
        : m_detaction(nullptr), m_node(nullptr), m_group(UINT_MAX){};
    /** デストラクタ。 */
    virtual ~CollisionComponent();
    /** インスタンスを作成する。*/
    static CollisionComponent* create();
    /** インスタンスを作成する。*/
    static CollisionComponent* create(unsigned int group);
    /** インスタンスを作成する。*/
    static CollisionComponent* create(CollisionDetaction* detaction);
    /** インスタンスを作成する。*/
    static CollisionComponent* create(CollisionDetaction* detaction,
                                      unsigned int group);

    /** Nodeがフィールドに追加された際に呼ばれる。 */
    void onEnter() override;
    /** Nodeがフィールドから除去された際に呼ばれる。 */
    void onExit() override;

   protected:
    /** 初期化。 */
    virtual bool initWithDetaction(CollisionDetaction* detaction,
                                   unsigned int group);
};

#endif /* CollisionComponent_hpp */
