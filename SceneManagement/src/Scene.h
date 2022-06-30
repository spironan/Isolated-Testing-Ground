/************************************************************************************//*!
\file           IScene.h
\project        Ouroboros
\author         Chua Teck Lee, c.tecklee, 390008420 | code contribution (100%)
\par            email: c.tecklee\@digipen.edu
\date           27th Jun 2022
\brief          Interface class that defines a scene

Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*//*************************************************************************************/
#pragma once

#include "Hash.h"

struct LoadStatus
{
    bool Completed = false;
    float Progress = 0.f;
};

// Interface class
class IScene
{
public:
    // ID used to index the scene in scene manager
    using ID_type = StringHash::size_type;

public:
    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void LateUpdate() = 0;
    virtual void Render() = 0;
    virtual void EndOfFrameUpdate() = 0;
    virtual void Exit() = 0;
    
    virtual void LoadScene() = 0;
    virtual void UnloadScene() = 0;
    virtual void ReloadScene() = 0;

    virtual LoadStatus GetProgress() = 0;

    ID_type GetID() const { return m_id; }
    bool IsLoaded() const { return m_isLoaded; }
private:
    friend class SceneManager;
    ID_type m_id;
    bool m_isLoaded = false;
};