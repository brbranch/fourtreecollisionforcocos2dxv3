//
//  SampleMainLayer.cpp
//  CollisionSample
//
//  Created by Kazuki Oda on 2016/07/04.
//
//

#include "SampleLayer.hpp"
#include "SampleMainLayer.hpp"
USING_NS_CC;

Scene *SampleMainLayer::createScene() {
    auto scene = Scene::create();
    auto layer = SampleMainLayer::create();
    scene->addChild(layer);
    return scene;
}

bool SampleMainLayer::init() {
    if (!LayerColor::initWithColor(Color4B::WHITE)) {
        return false;
    }
    // t->addCollisionTarget(f);

    auto win = Director::getInstance()->getVisibleSize();
    auto o = Director::getInstance()->getVisibleOrigin();
    cocos2d::Vector<cocos2d::MenuItem *> list;
    // 数
    {
        auto num = Label::createWithSystemFont("Sprite", "Arrial", 20);
        num->setPosition(75, win.height - 50);
        num->setColor(Color3B::BLACK);
        addChild(num);
    }
    // 4分木
    {
        auto num = Label::createWithSystemFont("4分木利用", "Arrial", 20);
        num->setPosition(200, win.height - 50);
        num->setColor(Color3B::BLACK);
        addChild(num);
    }
    // 画面サイズ
    {
        auto num = Label::createWithSystemFont("フィールド", "Arrial", 20);
        num->setPosition(340, win.height - 50);
        num->setColor(Color3B::BLACK);
        addChild(num);
    }

    for (int i = 100; i <= 500; i += 100) {
        auto callback = [this, i](Ref *ref) {
            m_num = i;
            CCLOG("num: %d", i);
            for (auto m : m_nums) {
                if (m != ref) {
                    m->setOpacity(140);
                } else {
                    m->setOpacity(255);
                }
            }
        };
        auto str = StringUtils::format("%d個", i);
        auto menu = generateButton(str, callback);
        if (i == 100) {
            m_num = i;
            menu->setOpacity(255);
        } else {
            menu->setOpacity(140);
        }
        menu->setPosition(menu->getContentSize().width / 2,
                          win.height - 50 - i);
        list.pushBack(menu);
        m_nums.pushBack(menu);
    }
    // 4分木使うかどうか
    for (bool i : {true, false}) {
        auto callback = [this, i](Ref *ref) {
            m_usefortree = i;
            CCLOG("useForTree: %s", i ? "true" : "false");
            for (auto m : m_uses) {
                if (m != ref) {
                    m->setOpacity(140);
                } else {
                    m->setOpacity(255);
                }
            }
        };
        auto str = StringUtils::format("%s", i ? "する" : "しない");
        auto menu = generateButton(str, callback);
        if (i) {
            m_usefortree = i;
            menu->setOpacity(255);
        } else {
            menu->setOpacity(140);
        }
        menu->setPosition(menu->getContentSize().width * 1.5,
                          win.height - 50 - (i ? 100 : 200));
        list.pushBack(menu);
        m_uses.pushBack(menu);
    }

    // フィールド
    for (int i : {1, 2, 3}) {
        auto callback = [this, i](Ref *ref) {
            m_winSize = i;
            for (auto m : m_winsizes) {
                if (m != ref) {
                    m->setOpacity(140);
                } else {
                    m->setOpacity(255);
                }
            }
        };
        auto str = StringUtils::format("%d倍", i);
        auto menu = generateButton(str, callback);
        if (i == 1) {
            m_winSize = i;
            menu->setOpacity(255);
        } else {
            menu->setOpacity(140);
        }
        menu->setPosition(menu->getContentSize().width * 2.5,
                          win.height - 50 - (i * 100));
        list.pushBack(menu);
        m_winsizes.pushBack(menu);
    }
    auto callback = [this](Ref *ref) {
        auto scene = SampleLayer::createScene(m_num, m_usefortree, m_winSize);
        Director::getInstance()->replaceScene(scene);
    };
    auto go = generateButton("Go", callback);
    go->setPosition(win.width - 130, win.height - 50);
    list.pushBack(go);

    auto menu = Menu::createWithArray(list);
    menu->setContentSize(win);
    menu->setAnchorPoint(Point::ZERO);
    menu->setPosition(Point::ZERO);
    this->addChild(menu);
    return true;
}

MenuItemSprite *SampleMainLayer::generateButton(
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