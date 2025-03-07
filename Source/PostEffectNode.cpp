//
//  PostEffectNode.cpp
//  WordsGame
//
//  Created by Mikhail Shulepov on 02/09/16.
//
//

#include "PostEffectNode.h"
#include "2d/RenderTexture.h"
#include "axmol.h"

using namespace ax;

PostEffectNode::~PostEffectNode() {}

void PostEffectNode::setStaticDrawing(bool var)
{
    staticDrawing = var;
    dirty         = true;
}

bool PostEffectNode::getStaticDrawing() const
{
    return staticDrawing;
}

void PostEffectNode::checkRenderTextures()
{
    if (sizeDirty)
    {
        renderTextures.clear();
        rtSprites.clear();
        sizeDirty = false;
    }

    const size_t requiredRenderTextures = passes.size() + (staticDrawing ? 1 : 0);

    if (requiredRenderTextures != renderTextures.size())
    {
        rtSprites.clear();
    }

    while (requiredRenderTextures < renderTextures.size())
    {
        renderTextures.popBack();
    }

    while (requiredRenderTextures > renderTextures.size())
    {
        // if (renderTextures.size() < 2) {
        // create new RenderTexture
        const Size size   = getContentSize();
        RenderTexture* rt = RenderTexture::create(size.width, size.height);
        renderTextures.pushBack(rt);
        /*} else {
            //Use PingPong drawing
            const size_t i = renderTextures.size() % 2;
            RenderTexture *rt = RenderTexture::create(renderTextures.at(i));
            renderTextures.pushBack(rt);
        }*/
    }

    if (rtSprites.empty())
    {
        recreateRtSprites();
    }
}

void PostEffectNode::recreateRtSprites()
{
    size_t pass = 0;
    for (RenderTexture* rt : renderTextures)
    {
        Sprite* spr = Sprite::createWithTexture(rt->getSprite()->getTexture());
        spr->setAnchorPoint(Point::ZERO);
        spr->setBlendFunc(rt->getSprite()->getBlendFunc());

        spr->setOpacityModifyRGB(rt->getSprite()->isOpacityModifyRGB());
        spr->getTexture()->setAntiAliasTexParameters();

        if (pass < passes.size())
        {
            spr->setProgramState(passes.at(pass));
        }
        spr->setFlippedY(true);
        rtSprites.pushBack(spr);
        pass += 1;
    }
}

void PostEffectNode::visit(ax::Renderer* renderer, const ax::Mat4& parentTransform, uint32_t parentFlags)
{
    if (!postEffectsEnabled || passes.empty() || _children.empty())
    {
        Node::visit(renderer, parentTransform, parentFlags);
        return;
    }

    // quick return if not visible. children won't be drawn.
    if (!_visible)
    {
        return;
    }

    uint32_t flags = processParentFlags(parentTransform, parentFlags);

    _director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    _director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);

    auto visibleByCamera = isVisitableByVisitingCamera();
    if (visibleByCamera)
    {
        draw(renderer, _modelViewTransform, flags);
        rtSprites.back()->visit(renderer, _modelViewTransform, parentFlags);
    }
    _director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void PostEffectNode::draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags)
{
    if (!postEffectsEnabled)
    {
        Node::draw(renderer, transform, flags);
        return;
    }

    if (!staticDrawing)
    {
        redraw(renderer);
    }
    else if (shouldRedraw)
    {
        redraw(renderer);
        shouldRedraw = false;
    }
}

void PostEffectNode::redraw(Renderer* renderer)
{
    if (passes.empty() || _children.empty())
    {
        return;
    }

    if (dirty)
    {
        checkRenderTextures();
        dirty = false;
    }

    RenderTexture* firstPass = renderTextures.front();
    firstPass->begin();
    sortAllChildren();
    for (int i = 0; i < _children.size(); i++)
    {
        auto node = _children.at(i);
        node->visit(renderer, Mat4::IDENTITY, 0);
    }
    firstPass->end();

    for (size_t i = 1; i < renderTextures.size(); ++i)
    {
        Sprite* prevResult = rtSprites.at(i - 1);

        RenderTexture* rt = renderTextures.at(i);
        rt->begin();
        prevResult->visit(renderer, Mat4::IDENTITY, 0);
        rt->end();
    }
}
