//
//  SampleLayer.hpp
//  CollisionSample
//
//  Created by Kazuki Oda on 2016/07/03.
//
//

#ifndef SampleLayer_hpp
#define SampleLayer_hpp

#include <stdio.h>
#include "cocos-ext.h"
#include "cocos2d.h"
#include "collision/CollisionDetaction.hpp"
class SampleLayer : public cocos2d::LayerColor {
   private:
    CC_SYNTHESIZE_RETAIN(cocos2d::Layer*, m_other, Layer);
    cocos2d::Label* m_label;
    cocos2d::SpriteBatchNode* m_node;
    CollisionDetaction* m_detection;
    double m_max;
    int m_num;
    bool m_use;
    int m_scale;
    cocos2d::extension::ScrollView* m_scroll;

   public:
    static cocos2d::Scene* createScene(int num, bool use, int scale);
    static SampleLayer* create(int num, bool use, int scale);
    void detectCollision(CollisionNode* collisionObject1,
                         CollisionNode* collisionObject2);

   protected:
    SampleLayer()
        : m_max(0)
        , m_num(100)
        , m_use(false)
        , m_scale(1)
        , m_scroll(nullptr)
        , m_node(nullptr)
        , m_detection(nullptr)
        , m_label(nullptr)
        , m_other(nullptr){};
    virtual ~SampleLayer() {
        CC_SAFE_RELEASE_NULL(m_scroll);
        CC_SAFE_RELEASE_NULL(m_node);
        CC_SAFE_RELEASE_NULL(m_other);
        CC_SAFE_RELEASE_NULL(m_label);
        // CC_SAFE_DELETE(m_detection);
    };

    bool init(int num, bool use, int scale);
    bool touchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void touchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void touchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    void update(float f) override;

   private:
    cocos2d::MenuItemSprite* generateButton(
        const std::string& caption, const cocos2d::ccMenuCallback& callback);
};

#endif /* SampleLayer_hpp */
