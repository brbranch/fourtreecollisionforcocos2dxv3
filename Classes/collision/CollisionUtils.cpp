/**
 ForTreeCollision v0.0.1
 CollisionUtils.cpp

 Copyright (c) 2016 Kazuki Oda

 This software is released under the MIT License.
 http://opensource.org/licenses/mit-license.php
*/

#include "CollisionUtils.hpp"
USING_NS_CC;

bool CollisionUtils::containsPoint(cocos2d::Node *target,
                                   const cocos2d::Point &point,
                                   const cocos2d::Size &wide) {
    if (target == nullptr) {
        return false;
    }
    // ざっくりと判定。BoundingBoxでfalseの場合はfalseを返す
    if (target->getBoundingBox().containsPoint(point) == false) {
        return false;
    }
    // もう少し細かく判定する。

    Point anchor(target->getAnchorPoint());
    Point pos(target->getPosition());
    Size size(target->getContentSize());
    size.width *= target->getScaleX();
    size.height *= target->getScaleY();

    double x1 = pos.x - size.width * anchor.x;
    double y1 = pos.y + size.height * (1 - anchor.y);
    double cx1 = x1 + (size.width / 2);
    double cy1 = y1 - (size.height / 2);
    double l = sqrt(pow(point.x - cx1, 2) + pow(point.y - cy1, 2));
    double r = CC_DEGREES_TO_RADIANS(-target->getRotation());
    double r2 = atan((point.y - cy1) / (point.x - cx1)) - r;
    double tx2 = l * cos(r2) + cx1;
    double ty2 = l * sin(r2) + cy1;

    if (x1 - wide.width <= tx2 &&               //
        tx2 <= x1 + size.width + wide.width &&  //
        y1 + wide.height >= ty2 &&              //
        ty2 >= y1 - size.height - wide.height) {
        return true;
    }

    return false;
}

bool CollisionUtils::intersectRect(cocos2d::Node *target, cocos2d::Node *self) {
    if (!target->getBoundingBox().intersectsRect(self->getBoundingBox())) {
        return false;
    }

    Size tSize(target->getContentSize());
    Size sSize(self->getContentSize());
    double tl = sqrt(pow(tSize.width, 2) + pow(tSize.height, 2));
    double sl = sqrt(pow(sSize.width, 2) + pow(sSize.height, 2));
    auto s = tl >= sl ? self : target;
    auto b = tl >= sl ? target : self;
    auto sCorner = getCorner(s);
    auto bCorner = getCorner(b);

    // 線分に入っているかどうかを見る
    for (auto line : sCorner.lines()) {
        for (auto line2 : bCorner.lines()) {
            if (cross(&line, &line2)) {
                return true;
            }
        }
    }
    return containsPoint(b, s->getPosition(), Size::ZERO) ||
           containsPoint(s, b->getPosition(), Size::ZERO);
}

CollisionCorner CollisionUtils::getCorner(cocos2d::Node *node) {
    Size size(node->getContentSize());
    size.width *= node->getScaleX();
    size.height *= node->getScaleY();

    double _atan1 = atan2(size.height / 2, size.width / 2);
    double _atan2 = atan2(size.height / 2, -size.width / 2);
    double _atan3 = -_atan1;
    double _atan4 = -_atan2;
    double l = sqrt(pow(size.width / 2, 2) + pow(size.height / 2, 2));

    double radian = CC_DEGREES_TO_RADIANS(-node->getRotation());
    Point pos(node->getPosition());

    return {Point(pos.x + l * cosf(radian + _atan1),
                  pos.y + l * sinf(radian + _atan1)),
            Point(pos.x + l * cosf(radian + _atan2),
                  pos.y + l * sinf(radian + _atan2)),
            Point(pos.x + l * cosf(radian + _atan3),
                  pos.y + l * sinf(radian + _atan3)),
            Point(pos.x + l * cosf(radian + _atan4),
                  pos.y + l * sinf(radian + _atan4)),
            pos};
}

bool CollisionUtils::cross(CollisionLine *one, CollisionLine *two) {
    float ax = one->start->x;
    float ay = one->start->y;
    float bx = one->end->x;
    float by = one->end->y;
    float cx = two->start->x;
    float cy = two->start->y;
    float dx = two->end->x;
    float dy = two->end->y;

    float ta = (cx - dx) * (ay - cy) + (cy - dy) * (cx - ax);
    float tb = (cx - dx) * (by - cy) + (cy - dy) * (cx - bx);
    float tc = (ax - bx) * (cy - ay) + (ay - by) * (ax - cx);
    float td = (ax - bx) * (dy - ay) + (ay - by) * (ax - dx);

    return tc * td < 0 && ta * tb < 0;
}
