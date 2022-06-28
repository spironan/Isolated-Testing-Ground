#include "SceneManager.h"

SceneManager::SceneManager()
{
    Init();
}

SceneManager::~SceneManager()
{
    Terminate();
}

bool SceneManager::HasActiveScene() const
{
    return m_activeScene.get() != nullptr;
}

std::shared_ptr<IScene> SceneManager::GetActiveScene() const
{
    return m_activeScene;
}

bool SceneManager::HasScene(key_type id) const
{
    return m_scenes.contains(id);
}

bool SceneManager::HasActiveScene(key_type id) const
{
    return HasActiveScene() && m_activeScene->GetID() == id;
}

bool SceneManager::SetActiveScene(key_type id)
{
    if(!HasScene(id))
        return false;

    // Reload scene
    if (m_activeScene && m_activeScene->GetID() == id)
        m_nextScene->ReloadScene();
    else
        m_nextScene = m_scenes.find(id)->second;

    return true;
}

bool SceneManager::ChangeScene(key_type id)
{
    return SetActiveScene(id);
}

bool SceneManager::ChangeScene(std::string_view name)
{
    // calls SceneManager::ChangeScene(key_type id)
    return ChangeScene(StringHash::GenerateFNV1aHash(name));
}

void SceneManager::ChangeScene(std::shared_ptr<IScene> scene)
{
    m_nextScene = scene;
}

bool SceneManager::ReloadActiveScene()
{
    if (HasActiveScene())
    {
        m_activeScene->ReloadScene();
        return true;
    }

    return false;
}

void SceneManager::Init()
{
    if (m_nextScene != m_activeScene)
    {
        m_activeScene = m_nextScene;

        if (m_activeScene)
        {
            m_activeScene->ReloadScene();
        }
    }
}

void SceneManager::Update()
{
    {
        //Check For Scene Changes
        if (m_nextScene != m_activeScene)
        {
            if (m_activeScene)
            {
                m_activeScene->UnloadScene();
            }

            m_activeScene = m_nextScene;

            if (m_activeScene)
            {
                m_activeScene->LoadScene();
            }
        }

        if (m_activeScene == nullptr) 
            return;

        {
            m_activeScene->Update();
        }
        {
            m_activeScene->LateUpdate();
        }
        {
            m_activeScene->Render();
        }
        {
            m_activeScene->EndOfFrameUpdate();
        }
    }
}

void SceneManager::Terminate()
{
    m_activeScene->UnloadScene();
    m_activeScene = m_nextScene = nullptr;
    m_scenes.clear();
}

bool SceneManager::RemoveScene(std::string_view filename)
{
    auto key = StringHash::GenerateFNV1aHash(filename);

    if (m_scenes.contains(key))
    {
        m_scenes.erase(key);
        return true;
    }

    return false;
}
