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

bool SceneManager::HasLoadingScene() const
{
    return m_loadingScene != nullptr;
}

std::shared_ptr<IScene> SceneManager::GetActiveScene() const
{
    return m_activeScene;
}

bool SceneManager::HasScene(key_type id) const
{
    return m_scenes.contains(id);
}

bool SceneManager::IsActiveScene(key_type id) const
{
    return HasActiveScene() && m_activeScene->GetID() == id;
}

bool SceneManager::SetLoadingScene(key_type id)
{
    if (!HasScene(id))
        return false;

    m_loadingScene = m_scenes.at(id);

    return true;
}

bool SceneManager::SetActiveScene(key_type id)
{
    if(!HasScene(id))
        return false;

    // Reload scene if we are already at this scene
    if (IsActiveScene(id))
        ReloadActiveScene();
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
            m_activeScene->LoadScene();
        }
    }
}

void SceneManager::Update()
{
    {
        //Check For Scene Changes
        if (m_nextScene != m_activeScene)
        {
            // 2 possibilities 
            // A) Currently in the midst of loading
            if (m_loading)
            {
                // next scene is guaranteed to exist at this point.
                auto progress = m_nextScene->GetProgress();
                if (progress.Completed)
                {
                    // unload the loading-scene
                    m_activeScene->UnloadScene();
                    // set active scene to new scene after its finished loading
                    m_activeScene = m_nextScene;
                    
                    m_loading = false;
                }
            }
            // implied (!m_loading && m_activeScene)
            // B) Currently not loading and looking to change scene.
            else if (m_activeScene)
            {
                m_activeScene->UnloadScene();
            
                // only go to loading scene, if theres one and if next scene exist as well.
                if (m_loadingScene && m_nextScene && m_activeScene != m_loadingScene)
                {
                    m_activeScene = m_loadingScene;
                    
                    //start loading next scene
                    m_nextScene->LoadScene();

                    //load loading scene
                    m_activeScene->LoadScene();
                    m_loading = true;
                }
                // Perform the old method : directly change to assigned scene and start loading
                else
                {
                    m_activeScene = m_nextScene;

                    if (m_activeScene)
                    {
                        m_activeScene->LoadScene();
                    }
                }
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
    if(m_activeScene)
        m_activeScene->UnloadScene();
    
    if (m_loadingScene)
        m_loadingScene->UnloadScene();

    m_activeScene = m_nextScene = m_loadingScene = nullptr;
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
