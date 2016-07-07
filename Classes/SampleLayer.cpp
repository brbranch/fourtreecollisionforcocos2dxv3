//
//  SampleLayer2.cpp
//  CollisionSample
//
//  Created by Kazuki Oda on 2016/07/03.
//
//

#include "SampleLayer.hpp"
#include "SampleMainLayer.hpp"
#include "collision/CollisionComponent.hpp"
#include "collision/CollisionUtils.hpp"
USING_NS_CC;
Scene *SampleLayer::createScene(int num, bool use, int scale) {
    auto scene = Scene::create();
    auto layer = SampleLayer::create(num, use, scale);
    scene->addChild(layer);
    scene->addChild(layer->getLayer());
    return scene;
}

SampleLayer *SampleLayer::create(int num, bool use, int scale) {
    auto ref = new SampleLayer();
    if (ref && ref->init(num, use, scale)) {
        ref->autorelease();
        return ref;
    }
    CC_SAFE_DELETE(ref);
    return nullptr;
}

bool SampleLayer::init(int num, bool use, int scale) {
    if (!LayerColor::initWithColor(Color4B::WHITE)) {
        return false;
    }

    m_num = num;
    m_use = use;
    m_scale = scale;

    srand((unsigned int)time(nullptr));
    auto win = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    auto scroll = cocos2d::extension::ScrollView::create(win);
    scroll->setMaxScale(2.0);
    scroll->setMinScale(0.5);

    m_other = Layer::create();
    m_other->retain();
    m_label = Label::createWithSystemFont("test", "Arial", 40);
    m_label->setColor(Color3B::BLUE);
    m_label->setPosition(win.width / 2, win.height - 50);
    m_label->retain();
    m_other->addChild(m_label);

    auto menuItem = generateButton("Back", [](Ref *ref) {
        auto scene = SampleMainLayer::createScene();
        Director::getInstance()->replaceScene(scene);
    });
    auto menu = Menu::createWithItem(menuItem);
    menu->setContentSize(menuItem->getContentSize());
    menu->setAnchorPoint(Point::ANCHOR_MIDDLE);
    menu->setPosition(win.width - menu->getContentSize().width / 2,
                      win.height - 50);
    m_other->addChild(menu);

    win = win * m_scale;
    scroll->setContentSize(win);
    scroll->setTouchEnabled(true);
    scroll->setPosition(Point::ZERO);
    scroll->setAnchorPoint(Point::ZERO);
    addChild(scroll);

    m_node = SpriteBatchNode::create("sprite.png");
    m_node->retain();
    m_node->setContentSize(win);
    scroll->setContainer(m_node);
    m_scroll = scroll;
    m_scroll->retain();

    auto call1 = CC_CALLBACK_2(SampleLayer::detectCollision, this);
    m_detection = CollisionDetaction::getDefaultDetaction();
    m_detection->init(m_node, 4, true, call1);

    {
        auto sprite = Sprite::create("sprite.png");
        sprite->setPosition(win.width / 2 + origin.x,
                            win.height / 2 + origin.y);
        sprite->addComponent(CollisionComponent::create(2));
        // m_detection->add(sprite);
        m_node->addChild(sprite);
    }
    for (int i = 0; i < m_num; i++) {
        auto sprite = Sprite::create("sprite.png");
        auto pos = Point(
            win.width / 2 + CCRANDOM_MINUS1_1() * win.width / 2 + origin.x,
            win.height / 2 + CCRANDOM_MINUS1_1() * win.height / 2 + origin.y);
        sprite->setScaleX(CCRANDOM_0_1() * 0.3 + 0.1);
        sprite->setScaleY(CCRANDOM_0_1() + 0.1);
        sprite->setPosition(pos);
        sprite->setRotation(CCRANDOM_MINUS1_1() * 360.0);
        auto in = rand() % 2 + 1;
        sprite->addComponent(CollisionComponent::create(in));
        // m_detection->add(sprite);
        m_node->addChild(sprite);
    }
    {
        auto sprite = Sprite::create("sprite.png");
        sprite->setPosition(win.width / 4 + origin.x,
                            win.height / 4 + origin.y);
        sprite->setRotation(90);
        auto rotate = RotateBy::create(1.0, 45);
        auto repeat = RepeatForever::create(rotate);
        // m_detection->add(sprite);
        sprite->addComponent(CollisionComponent::create(2));
        m_node->addChild(sprite);
        sprite->runAction(repeat);
    }
    auto began = CC_CALLBACK_2(SampleLayer::touchBegan, this);
    auto move = CC_CALLBACK_2(SampleLayer::touchMoved, this);
    auto end = CC_CALLBACK_2(SampleLayer::touchEnded, this);
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = began;
    listener->onTouchMoved = move;
    listener->onTouchEnded = end;
    listener->setSwallowTouches(true);
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,
                                                                 this);
    this->scheduleUpdate();

    this->schedule(
        [this](float f) {
            auto time = Director::getInstance()->getSecondsPerFrame();
            auto str =
                StringUtils::format("衝突判定:%fs\n全体:%fs", m_max, time);
            m_label->setString(str);
        },
        0.1, "display");
    return true;
}

bool SampleLayer::touchBegan(cocos2d::Touch *touch, cocos2d::Event *event) {
    auto pos = m_node->convertToNodeSpace(touch->getLocation());
    for (auto node : m_node->getChildren()) {
        if (CollisionUtils::containsPoint(node, pos, Size::ZERO)) {
            node->retain();
            setUserData(node);
            m_scroll->setTouchEnabled(false);
            return true;
        }
    }
    return true;
}

void SampleLayer::touchMoved(cocos2d::Touch *touch, cocos2d::Event *event) {
    auto pos = m_node->convertToNodeSpace(touch->getLocation());
    if (auto node = (Node *)(getUserData())) {
        if (node->getParent() == nullptr) {
            node->release();
            setUserData(nullptr);
            return;
        }
        node->setPosition(pos);

        return;
    }
}

void SampleLayer::touchEnded(cocos2d::Touch *touch, cocos2d::Event *event) {
    if (auto node = (Node *)(getUserData())) {
        node->release();
        setUserData(nullptr);
    }
    m_scroll->setTouchEnabled(true);
    /*
    auto pos = touch->getLocation();
    for (auto node : m_node->getChildren()) {
        node->setColor(Color3B::WHITE);
    }*/
}

void SampleLayer::update(float f) {
    LayerColor::update(f);
    for (auto n : m_node->getChildren()) {
        n->setColor(Color3B::WHITE);
    }

    double time = utils::gettime();
    if (m_use) {
        m_detection->update();
    } else {
        auto nodes = m_node->getChildren();
        auto size = nodes.size();
        for (int i = 0; i < size; i++) {
            auto node1 = nodes.at(i);
            for (int j = i + 1; j < size; j++) {
                auto node2 = nodes.at(j);
                if (CollisionUtils::intersectRect(node1, node2)) {
                    node1->setColor(Color3B::RED);
                    node2->setColor(Color3B::RED);
                }
            }
        }
        /*
        for (auto node : m_node->getChildren()) {
            for (auto other : m_node->getChildren()) {
                if (node == other) {
                    continue;
                }
                if (CollisionUtils::intersectRect(node, other)) {
                    other->setColor(Color3B::RED);
                }
            }
        }*/
    }
    time = utils::gettime() - time;
    m_max = time;
}

void SampleLayer::detectCollision(CollisionNode *collisionObject1,
                                  CollisionNode *collisionObject2) {
    auto node1 = collisionObject1->getNode();
    auto node2 = collisionObject2->getNode();
    if (CollisionUtils::intersectRect(node1, node2) == false) {
        return;
    }
    node1->setColor(Color3B::RED);
    node2->setColor(Color3B::RED);

    /*
    if (collisionObject2->isSameGroup(collisionObject1)) {
        return;
    }
    if (collisionObject1->bitmask(1)) {
        node1->removeFromParent();
    }
    if (collisionObject2->bitmask(1)) {
        node2->removeFromParent();
    }*/
    // m_detection->remove(collisionObject2);
    // collisionObject2->setColor(Color3B::BLUE);
}

MenuItemSprite *SampleLayer::generateButton(
    const std::string &caption, const cocos2d::ccMenuCallback &callback) {
    auto generate = [this](const std::string &cap) {
        auto spr = Sprite::create("button.png");
        auto size = spr->getContentSize();
        auto text = Label::createWithSystemFont(cap, "Arial", 30);
        text->setPosition(Point(size.width / 2, size.height / 2));
        text->setColor(Color3B::BLACK);
        spr->addChild(text);
        return spr;
    };
    auto on = generate(caption);
    auto select = generate(caption);
    auto disable = generate(caption);
    select->setColor(Color3B(200, 230, 230));
    disable->setColor(Color3B::GRAY);
    return MenuItemSprite::create(on, select, disable, callback);
}