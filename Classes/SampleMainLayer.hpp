//
//  SampleMainLayer.hpp
//  CollisionSample
//
//  Created by Kazuki Oda on 2016/07/04.
//
//

#ifndef SampleMainLayer_hpp
#define SampleMainLayer_hpp

#include <stdio.h>
#include "cocos2d.h"

class SampleMainLayer : public cocos2d::LayerColor {
   public:
    static cocos2d::Scene* createScene();
    CREATE_FUNC(SampleMainLayer);
    bool m_usefortree;
    int m_num;
    int m_winSize;
    cocos2d::Vector<cocos2d::MenuItemSprite*> m_nums;
    cocos2d::Vector<cocos2d::MenuItemSprite*> m_uses;
    cocos2d::Vector<cocos2d::MenuItemSprite*> m_winsizes;

   protected:
    bool init() override;
    cocos2d::MenuItemSprite* generateButton(
        const std::string& caption, const cocos2d::ccMenuCallback& callback);
};

#endif /* SampleMainLayer_hpp */
