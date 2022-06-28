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

// Interface class
class IScene
{
public:
    // ID used to index the scene in scene manager
    using ID_type = std::size_t;

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

    ID_type GetID() const { return m_id; }
private:
    friend class SceneManager;
    ID_type m_id;
};