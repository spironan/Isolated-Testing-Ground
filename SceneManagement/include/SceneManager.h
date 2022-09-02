/************************************************************************************//*!
\file           SceneManager.h
\project        Ouroboros
\author         Chua Teck Lee, c.tecklee, 390008420 | code contribution (100%)
\par            email: c.tecklee\@digipen.edu
\date           27th Jun 2022
\brief          SceneManager takes charge of scenes how they are loaded/unloaded and
                updated

Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*//*************************************************************************************/
#pragma once

#include "../src/Hash.h"
#include "IScene.h"

#include <map>
#include <memory>
#include <string>
#include <cassert>

class SceneManager final
{
public:
    using key_type = IScene::ID_type;
    using container_type = std::map<key_type, std::shared_ptr<IScene>>;
    
    template<typename derived>
    using result = std::tuple<bool, key_type, std::weak_ptr<derived>>;

    static constexpr key_type NO_SCENE = std::numeric_limits<key_type>::max();
    static constexpr key_type REMOVED  = std::numeric_limits<key_type>::max();

private:
    container_type m_scenes{};

    std::shared_ptr<IScene> m_activeScene = nullptr;
    std::shared_ptr<IScene> m_nextScene = nullptr;
    
    std::shared_ptr<IScene> m_loadingScene = nullptr;
    bool m_loading = false;

public:
    SceneManager();
    ~SceneManager();

    template <typename Derived, class ... Args>
    result<Derived> CreateNewScene(std::string_view name, Args&&...args)
    {
        static_assert(std::is_base_of_v<IScene, Derived>, "Scene must be derived from IScene class!");

        key_type key = util::StringHash::GenerateFNV1aHash(name);

        if (m_scenes.contains(key))
            return { false, NO_SCENE, std::weak_ptr<Derived>() };

        std::shared_ptr<Derived> dervied_scene = std::make_shared<Derived>(args...);
        std::shared_ptr<IScene> new_scene = dervied_scene;
        m_scenes.emplace(key, new_scene);
        new_scene->m_id = key;

        return { true, new_scene->GetID(), dervied_scene };
    }

    template <typename Derived>
    std::shared_ptr<Derived> GetActiveScene() const
    {
        return std::static_pointer_cast<Derived>(GetActiveScene().lock());
    }

    bool HasActiveScene() const;
    bool HasLoadingScene() const;
    std::weak_ptr<IScene> GetActiveScene() const;
    std::weak_ptr<IScene> GetScene(key_type id) const;
    bool HasScene(key_type id) const;
    bool IsActiveScene(key_type id) const;

    bool SetLoadingScene(key_type id);
    bool SetActiveScene(key_type id);
    bool ChangeScene(key_type id);
    bool ChangeScene(std::string_view name);
    bool ChangeScene(std::weak_ptr<IScene> scene);
    bool ReloadActiveScene();

    void Init();
    void Update();
    void Terminate();

    bool RemoveScene(std::string_view filename);
    bool RemoveScene(key_type id);

private:
    void LoadScene(std::shared_ptr<IScene> scene);
    void UnloadScene(std::shared_ptr<IScene> scene);
};

