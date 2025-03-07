//
//  PostEffectNode.hpp
//  WordsGame
//
//  Created by Mikhail Shulepov on 02/09/16.
//
//

#pragma once

#include "2d/Node.h"
#include "2d/RenderTexture.h"
#include "renderer/backend/ProgramState.h"

class PostEffectNode: public ax::Node {
public:
    static PostEffectNode *create() {
        auto ret = new PostEffectNode();
        ret->autorelease();
        return ret;
    }
    
    static PostEffectNode *create(const ax::Vector<ax::backend::ProgramState *> &passes) {
        auto ret = create();
        ret->setShaderPasses(passes);
        return ret;
    }
    
    virtual ~PostEffectNode() override;
    
    AX_SYNTHESIZE(bool, postEffectsEnabled, PostEffectsEnabled)
    AX_PROPERTY(bool, staticDrawing, StaticDrawing)
    
    void requestRedraw() {
        shouldRedraw = true;
    }
    
    void setShaderPasses(const ax::Vector<ax::backend::ProgramState *> &passes) {
        this->passes = passes;
        dirty = true;
    }
    
   ax::backend::ProgramState* getShaderPass(int pass) const {
        return passes.at(pass);
    }
    
    size_t getShaderPassesCount() const {
        return passes.size();
    }
    
    virtual void setContentSize(const ax::Size &size) override {
        Node::setContentSize(size);
        dirty = true;
        sizeDirty = true;
    }
    
    void draw(ax::Renderer *renderer, const ax::Mat4& transform, uint32_t flags) override;
    void visit(ax::Renderer *renderer, const ax::Mat4& parentTransform, uint32_t parentFlags) override;
    
private:
    void checkRenderTextures();
    void recreateRtSprites();
    void redraw(ax::Renderer *renderer);

private:
    ax::Vector<ax::backend::ProgramState*> passes;
    
    bool dirty = false;
    bool sizeDirty = false;
    ax::Vector<ax::RenderTexture *> renderTextures;
    ax::Vector<ax::Sprite *> rtSprites;
    bool shouldRedraw = true;
};
