/**
 ForTreeCollision v0.0.1
 CollisionUtils.hpp

 cocos2d-x 3.x用の衝突判定ユーティリティクラス。

 Copyright (c) 2016 Kazuki Oda

 This software is released under the MIT License.
 http://opensource.org/licenses/mit-license.php
*/

#ifndef CollisionUtils_hpp
#define CollisionUtils_hpp

#include <stdio.h>
#include "cocos2d.h"
/**
 * 線分
 */
typedef struct __line {
    cocos2d::Point* start;
    cocos2d::Point* end;
} CollisionLine;
/**
 * 四隅＋中心
 */
typedef struct __collision_corner {
    cocos2d::Point rightTop;
    cocos2d::Point leftTop;
    cocos2d::Point rightBottom;
    cocos2d::Point leftBottom;
    cocos2d::Point center;
    std::vector<cocos2d::Point*> points() {
        return {&rightTop, &leftTop, &rightBottom, &leftBottom, &center};
    }
    std::vector<CollisionLine> lines() {
        return {{&rightTop, &leftTop},
                {&rightTop, &rightBottom},
                {&rightBottom, &leftBottom},
                {&leftTop, &leftBottom}};
    }
} CollisionCorner;

/**
 * 衝突判定ユーティリティクラス。
 */
class CollisionUtils {
   public:
    /**
      矩形と点の衝突判定 (矩形の回転を考慮)
      @param target ターゲット
      @param point ポイントの位置
      @param wide 当たり判定を広げる
     */
    static bool containsPoint(cocos2d::Node* target,
                              const cocos2d::Point& point,
                              const cocos2d::Size& wide);

    /**
      矩形同士の衝突判定 (矩形同士の回転を考慮)
      @param target ターゲット
      @param self 自分自身
     */
    static bool intersectRect(cocos2d::Node* target, cocos2d::Node* self);

    /**
     回転を考慮した矩形の四隅の位置を返す。
     */
    static CollisionCorner getCorner(cocos2d::Node* node);

   private:
    /** 線がクロスしているかどうかを返す。 */
    static bool cross(CollisionLine* one, CollisionLine* two);
    /** インスタンス化禁止 */
    CC_DISALLOW_COPY_AND_ASSIGN(CollisionUtils);
    CollisionUtils(){};
};

#endif /* CollisionUtils_hpp */
