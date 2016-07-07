/**
 ForTreeCollision v0.0.1
 CollisionComponent.hpp

 Copyright (c) 2016 Kazuki Oda

 This software is released under the MIT License.
 http://opensource.org/licenses/mit-license.php
*/

#include "CollisionComponent.hpp"
USING_NS_CC;

CollisionComponent* CollisionComponent::create(unsigned int group) {
    return create(CollisionDetaction::getDefaultDetaction(), group);
}
CollisionComponent* CollisionComponent::create(CollisionDetaction* detaction) {
    return create(detaction, UINT_MAX);
}
CollisionComponent* CollisionComponent::create() {
    return create(CollisionDetaction::getDefaultDetaction(), UINT_MAX);
}
CollisionComponent* CollisionComponent::create(CollisionDetaction* detaction,
                                               unsigned int group) {
    auto ref = new CollisionComponent();
    if (ref && ref->initWithDetaction(detaction, group)) {
        ref->autorelease();
        return ref;
    }
    CC_SAFE_RELEASE(ref);
    return nullptr;
}

CollisionComponent::~CollisionComponent() {
    CC_SAFE_RELEASE_NULL(m_node);
    CC_SAFE_RELEASE_NULL(m_detaction);
}

bool CollisionComponent::initWithDetaction(CollisionDetaction* detaction,
                                           unsigned int group) {
    if (!Component::init()) {
        return false;
    }
    CC_SAFE_RETAIN(detaction);
    m_detaction = detaction;
    m_group = group;
    return true;
}

void CollisionComponent::onEnter() {
    Component::onEnter();
    m_node = CollisionNode::create(getOwner(), m_group);
    m_node->retain();
    m_detaction->add(m_node);
}

void CollisionComponent::onExit() {
    Component::onExit();
    if (m_node->getReferenceCount() > 1) {
        m_detaction->remove(m_node);
    }
    CC_SAFE_RELEASE_NULL(m_detaction);
    CC_SAFE_RELEASE_NULL(m_node);
}