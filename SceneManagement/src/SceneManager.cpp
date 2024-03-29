#include "..\include\SceneManager.h"

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
    Terminate();
}

bool SceneManager::HasActiveScene() const
{
    return m_activeScene != nullptr;
}

bool SceneManager::HasLoadingScene() const
{
    return m_loadingScene != nullptr;
}

std::weak_ptr<IScene> SceneManager::GetActiveScene() const
{
    return m_activeScene;
}

std::weak_ptr<IScene> SceneManager::GetScene(key_type id) const
{
    if (!HasScene(id))
        return std::weak_ptr<IScene>();

    return m_scenes.at(id);
}

std::weak_ptr<IScene> SceneManager::GetScene(std::string_view name) const
{
    key_type key = util::StringHash::GenerateFNV1aHash(name);
    return GetScene(key);
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
        m_nextScene = m_scenes.at(id);

    return true;
}

bool SceneManager::ChangeScene(key_type id)
{
    return SetActiveScene(id);
}

bool SceneManager::ChangeScene(std::string_view name)
{
    // calls SceneManager::ChangeScene(key_type id)
    key_type key = util::StringHash::GenerateFNV1aHash(name);
    return ChangeScene(key);
}

bool SceneManager::ChangeScene(std::weak_ptr<IScene> scene)
{
    std::shared_ptr<IScene> target = scene.lock();
    return target != nullptr && SetActiveScene(target->GetID());
    //m_nextScene = scene;
}

bool SceneManager::ReloadActiveScene()
{
    if (HasActiveScene())
    {
        m_reload = true;
        return true;
    }

    return false;
}

void SceneManager::Init()
{
    if (m_nextScene != m_activeScene)
    {
        m_activeScene = m_nextScene;

        LoadScene(m_activeScene);
    }
}

void SceneManager::Update()
{
    {
        // Check For Scene Reloading note : intentional deferred to make it work with scripts.
        if (m_reload)
        {
            UnloadScene(m_activeScene);
            LoadScene(m_activeScene);
            m_reload = false;
        }

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
                    UnloadScene(m_activeScene);

                    // set active scene to new scene after its finished loading
                    m_activeScene = m_nextScene;

                    // explicitly init active scene (no need to load cause its already loaded)
                    m_activeScene->Init();

                    m_loading = false;
                }
            }
            // implied (!m_loading && m_activeScene)
            // B) Currently not loading and looking to change scene.
            else if (m_nextScene || m_activeScene)
            {
                UnloadScene(m_activeScene);

                // only go to loading scene if theres one and if next scene exist as well.
                // also make sure that loading scene is not the direct next scene.
                if (m_loadingScene && m_nextScene && m_loadingScene != m_nextScene && m_activeScene != m_loadingScene)
                {
                    m_activeScene = m_loadingScene;
                    
                    //explicitly start loading next scene (async)
                    m_nextScene->LoadScene();
                    m_nextScene->m_isLoaded = true;

                    //load and init loading scene
                    LoadScene(m_activeScene);

                    m_loading = true;
                }
                // Perform the old method : directly change to assigned scene and start loading
                else
                {
                    m_activeScene = m_nextScene;
                    
                    LoadScene(m_activeScene);
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
    /*UnloadScene(m_activeScene);
    UnloadScene(m_loadingScene);
    UnloadScene(m_nextScene);*/

    for (auto& [key, scene] : m_scenes)
    {
        UnloadScene(scene);
        scene.reset();
    }
    
    m_activeScene = m_nextScene = m_loadingScene = nullptr;
    
    m_scenes.clear();
}

bool SceneManager::RemoveScene(std::string_view filename)
{
    key_type key = util::StringHash::GenerateFNV1aHash(filename);
    
    return RemoveScene(key);
}

bool SceneManager::RemoveScene(key_type id)
{
    if (m_scenes.contains(id))
    {
        m_scenes.at(id)->m_id = REMOVED;
        m_scenes.erase(id);
        return true;
    }

    return false;
}

void SceneManager::LoadScene(std::shared_ptr<IScene> scene)
{
    if (scene && !scene->IsLoaded())
    {
        scene->LoadScene();
        scene->m_isLoaded = true;

        scene->Init();
    }
}

void SceneManager::UnloadScene(std::shared_ptr<IScene> scene)
{
    if (scene && scene->IsLoaded())
    {
        scene->Exit();

        scene->UnloadScene();
        scene->m_isLoaded = false;
    }
}
