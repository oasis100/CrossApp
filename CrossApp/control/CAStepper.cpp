//
//  CAStepper.cpp
//  CrossApp
//
//  Created by dai xinping on 14-8-15.
//  Copyright (c) 2014年 cocos2d-x. All rights reserved.
//

#include "CAStepper.h"
#include "basics/CAScheduler.h"
#include "view/CAImageView.h"

NS_CC_BEGIN

CAStepper::CAStepper()
: m_bAutoRepeat(true)
, m_bContinuous(true)
, m_bWraps(false)
, m_value(0)
, m_minimumValue(0)
, m_maximumValue(100)
, m_stepValue(0)
//, m_tintColor(ccc4(0, 0, 255, 255))
, m_pDecrementImageView(NULL)
, m_pIncrementImageView(NULL)
, m_pBackgroundImageView(NULL)
, m_actionType(ActionNone)
{
    memset(m_pBackgroundImage, 0x00, sizeof(m_pBackgroundImage));
    memset(m_pIncrementImage, 0x00, sizeof(m_pIncrementImage));
    memset(m_pDecrementImage, 0x00, sizeof(m_pDecrementImage));
}

CAStepper::~CAStepper()
{
    for (int i=0; i<CAControlStateAll; i++) {
        CC_SAFE_RELEASE(m_pBackgroundImage[i]);
    }
    for (int i=0; i<CAControlStateAll; i++) {
        CC_SAFE_RELEASE(m_pIncrementImage[i]);
    }
    for (int i=0; i<CAControlStateAll; i++) {
        CC_SAFE_RELEASE(m_pDecrementImage[i]);
    }
    
    CC_SAFE_RELEASE(m_pBackgroundImageView);
    CC_SAFE_RELEASE(m_pIncrementImageView);
    CC_SAFE_RELEASE(m_pDecrementImageView);
}

CAStepper* CAStepper::create()
{
    CAStepper* page = new CAStepper();
    
    if (page && page->init())
    {
        page->autorelease();
        return page;
    }
    
    CC_SAFE_DELETE(page);
    return NULL;
}

CAStepper* CAStepper::createWithFrame(const CCRect& rect)
{
    CAStepper* page = new CAStepper();
    
    if (page && page->initWithFrame(rect))
    {
        page->autorelease();
        return page;
    }
    
    CC_SAFE_DELETE(page);
    return NULL;
}

CAStepper* CAStepper::createWithCenter(const CCRect& rect)
{
    CAStepper* page = new CAStepper();
    
    if (page && page->initWithCenter(rect))
    {
        page->autorelease();
        return page;
    }
    
    CC_SAFE_DELETE(page);
    return NULL;
}

bool CAStepper::init()
{
    if (!CAControl::init())
    {
        return false;
    }
    
    setTouchEnabled(true);
    
    setBackgroundImage(CAImage::create("source_material/stepper_bg.png"), CAControlStateAll);
    setIncrementImage(CAImage::create("source_material/stepper_increase.png"), CAControlStateAll);
    setDecrementImage(CAImage::create("source_material/stepper_decrease.png"), CAControlStateAll);
    
    return true;
}

bool CAStepper::initWithFrame(const CCRect& rect)
{
    if (!CAControl::initWithFrame(rect))
    {
        return false;
    }
    
    return true;
}

bool CAStepper::initWithCenter(const CCRect& rect)
{
    if (!CAControl::initWithCenter(rect))
    {
        return false;
    }
    
    return true;
}

void CAStepper::setBackgroundImage(CrossApp::CAImage *image, CAControlState state)
{
    if (state == CAControlStateAll) {
        for (int i=0; i<CAControlStateAll; i++) {
            CC_SAFE_RELEASE_NULL(m_pBackgroundImage[i]);
            m_pBackgroundImage[i] = image;
            CC_SAFE_RETAIN(m_pBackgroundImage[i]);        
        }
    } else {
        CC_SAFE_RELEASE_NULL(m_pBackgroundImage[state]);
        m_pBackgroundImage[state] = image;
        CC_SAFE_RETAIN(m_pBackgroundImage[state]);        
    }
}

CAImage* CAStepper::getBackgroundImageForState(CAControlState state)
{
    return m_pBackgroundImage[state];
}

void CAStepper::setIncrementImage(CrossApp::CAImage *image, CAControlState state)
{
    if (state == CAControlStateAll) {
        for (int i=0; i<CAControlStateAll; i++) {
            CC_SAFE_RELEASE_NULL(m_pIncrementImage[i]);
            m_pIncrementImage[i] = image;
            CC_SAFE_RETAIN(m_pIncrementImage[i]);        
        }
    } else {
        CC_SAFE_RELEASE_NULL(m_pIncrementImage[state]);
        m_pIncrementImage[state] = image;
        CC_SAFE_RETAIN(m_pIncrementImage[state]);
    }
}

CAImage* CAStepper::getIncrementImageForState(CAControlState state)
{
    return m_pIncrementImage[state];
}

void CAStepper::setDecrementImage(CrossApp::CAImage *image, CAControlState state)
{
    if (state == CAControlStateAll) {
        for (int i=0; i<CAControlStateAll; i++) {
            CC_SAFE_RELEASE_NULL(m_pDecrementImage[i]);
            m_pDecrementImage[i] = image;
            CC_SAFE_RETAIN(m_pDecrementImage[i]);
        }
    } else {
        CC_SAFE_RELEASE_NULL(m_pDecrementImage[state]);
        m_pDecrementImage[state] = image;
        CC_SAFE_RETAIN(m_pDecrementImage[state]);
    }
}

CAImage* CAStepper::getDecrementImageForState(CAControlState state)
{
    return m_pDecrementImage[state];
}

bool CAStepper::ccTouchBegan(CATouch *pTouch, CAEvent *pEvent)
{
    m_actionType = ActionNone; // lazy init;
    
    if (getBounds().containsPoint(convertToNodeSpace(pTouch->getLocation()))) {
        
        click(pTouch);
        action();
        
        switch (m_actionType) {
            case ActionDecrease:
                m_pDecrementImageView->setImage(m_pDecrementImage[CAControlStateHighlighted]);
                m_pIncrementImageView->setImage(m_pIncrementImage[CAControlStateNormal]);
                break;
                
            case ActionIncrease:
                m_pDecrementImageView->setImage(m_pDecrementImage[CAControlStateNormal]);
                m_pIncrementImageView->setImage(m_pIncrementImage[CAControlStateHighlighted]);
                break;
                
            case ActionNone:
                m_pDecrementImageView->setImage(m_pDecrementImage[CAControlStateNormal]);
                m_pIncrementImageView->setImage(m_pIncrementImage[CAControlStateNormal]);                
                break;
            default:
                break;
        }
        
        CAScheduler::schedule(schedule_selector(CAStepper::repeat), this, 2.0f);
        return true;
    }
    
    return false;
}

void CAStepper::ccTouchMoved(CATouch *pTouch, CAEvent *pEvent)
{
    if (getBounds().containsPoint(convertToNodeSpace(pTouch->getLocation()))) {
        
        int prevAction = m_actionType;
        
        click(pTouch);
        
        if (prevAction != m_actionType) {
            switch (m_actionType) {
                case ActionDecrease:
                    m_pDecrementImageView->setImage(m_pDecrementImage[CAControlStateHighlighted]);
                    m_pIncrementImageView->setImage(m_pIncrementImage[CAControlStateNormal]);
                    break;
                    
                case ActionIncrease:
                    m_pDecrementImageView->setImage(m_pDecrementImage[CAControlStateNormal]);
                    m_pIncrementImageView->setImage(m_pIncrementImage[CAControlStateHighlighted]);
                    break;
                    
                case ActionNone:
                    m_pDecrementImageView->setImage(m_pDecrementImage[CAControlStateNormal]);
                    m_pIncrementImageView->setImage(m_pIncrementImage[CAControlStateNormal]);                
                    break;
                default:
                    break;
            }            
        }
    }
    m_actionType = ActionNone;
}

void CAStepper::ccTouchEnded(CATouch *pTouch, CAEvent *pEvent)
{
    m_pDecrementImageView->setImage(m_pDecrementImage[CAControlStateNormal]);
    m_pIncrementImageView->setImage(m_pIncrementImage[CAControlStateNormal]);                
    
    CAScheduler::unschedule(schedule_selector(CAStepper::repeat), this);
}

void CAStepper::ccTouchCancelled(CATouch *pTouch, CAEvent *pEvent)
{
    CAScheduler::unschedule(schedule_selector(CAStepper::repeat), this);
}

void CAStepper::onEnter()
{
    CAControl::onEnter();
    
    // init background
    if (!m_pBackgroundImageView && m_pBackgroundImage[CAControlStateNormal]) {
        m_pBackgroundImageView = CAImageView::createWithImage(m_pBackgroundImage[CAControlStateNormal]);
        m_pBackgroundImageView->retain();
        m_pBackgroundImageView->setCenterOrigin(CCPoint(getFrame().size.width/2, getFrame().size.height/2));
        addSubview(m_pBackgroundImageView);
    }
    
    // init increment
    if (!m_pIncrementImageView && m_pIncrementImage[CAControlStateNormal]) {
        m_pIncrementImageView = CAImageView::createWithImage(m_pIncrementImage[CAControlStateNormal]);
        m_pIncrementImageView->retain();
        m_pIncrementImageView->setCenterOrigin(CCPoint(getFrame().size.width*3/4, getFrame().size.height/2));
        addSubview(m_pIncrementImageView);
    }
    
    // init decrement
    if (!m_pDecrementImageView && m_pDecrementImage[CAControlStateNormal]) {
        m_pDecrementImageView = CAImageView::createWithImage(m_pDecrementImage[CAControlStateNormal]);
        m_pDecrementImageView->retain();
        m_pDecrementImageView->setCenterOrigin(CCPoint(getFrame().size.width/4, getFrame().size.height/2));
        addSubview(m_pDecrementImageView);
    }
}

void CAStepper::onExit()
{
    CAControl::onExit();
}

void CAStepper::visit()
{
    CAControl::visit();
}

void CAStepper::action()
{
    bool bValueChange = true;
    
    switch (m_actionType) {
        case ActionIncrease: // increase
            if (m_value == m_maximumValue) {
                if (m_bWraps) {
                    m_value = m_minimumValue;
                } else {
                    bValueChange = false;
                }
                break;
            }
            m_value += m_stepValue;
            if (m_value > m_maximumValue) {
                m_value = m_maximumValue;
            }
            break;
            
        case ActionDecrease: // decrease
            if (m_value == m_minimumValue) {
                if (m_bWraps) {
                    m_value = m_maximumValue;
                } else {
                    bValueChange = false;
                }
                break;
            }
            m_value -= m_stepValue;
            if (m_value < m_minimumValue) {
                m_value = m_minimumValue;
            }
            break;
            
        case 0:
        default:
            bValueChange = false;
            break;
    }
    
    // send value change event
    if (bValueChange && m_bContinuous) {
        if ( m_pTarget[CAControlEventTouchValueChanged] && 
             m_selTouch[CAControlEventTouchValueChanged] ) 
        {
            (m_pTarget[CAControlEventTouchValueChanged]->*m_selTouch[CAControlEventTouchValueChanged])(this, CCPointZero);
        }
    }
}

void CAStepper::repeat(float dt)
{
    CAScheduler::unschedule(schedule_selector(CAStepper::repeat), this);
    CAScheduler::schedule(schedule_selector(CAStepper::repeat), this, 0.2f);

    action();
}

void CAStepper::click(CATouch* pTouch)
{
    CCRect increseRect, decreaseRect;
    increseRect = getBounds();
    increseRect.origin.x = getBounds().size.width/2;
    increseRect.size.width = getBounds().size.width/2;
    decreaseRect = increseRect;
    decreaseRect.origin.x = 0;
    
    if (increseRect.containsPoint(convertToNodeSpace(pTouch->getLocation()))) {
        m_actionType = ActionIncrease;
    } else if (decreaseRect.containsPoint(convertToNodeSpace(pTouch->getLocation()))) {
        m_actionType = ActionDecrease;
    } else {
        m_actionType = ActionNone;
    }
}

NS_CC_END