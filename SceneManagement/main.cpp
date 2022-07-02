#include "src/Hash.h"
#include "include/SceneManager.h"

#include <string>
#include <iostream>

#define PRINT(name) std::cout << "[" << (name) << "] : " << __FUNCTION__ << std::endl;

class Scene : public IScene
{
public:
    std::string name;

    Scene(const char* name)
        : name{ name }
        , IScene()
    {
        PRINT(name);
    }

    virtual void Init() override 
    {
        PRINT(name);
    }

    virtual void Update() override 
    {
        PRINT(name);
    }

    virtual void LateUpdate() override 
    {
        PRINT(name);
    }

    virtual void Render() override 
    {
        PRINT(name);
    }

    virtual void EndOfFrameUpdate() override 
    {
        PRINT(name);
    }

    virtual void Exit() override 
    {
        PRINT(name);
    }

    virtual void LoadScene() override 
    {
        PRINT(name);
    }

    virtual void UnloadScene() override 
    {
        PRINT(name);
    }

    virtual void ReloadScene() override 
    {
        PRINT(name);
    }

    virtual LoadStatus GetProgress() override 
    {
        PRINT(name);

        return LoadStatus{true, 1.f};
    }
};

int main()
{
    {
        SceneManager sceneManager;

        auto [menu_success, menu_key] = sceneManager.CreateNewScene<Scene>("Start Menu", "Start Menu");
        auto [loading_success, load_key] = sceneManager.CreateNewScene<Scene>("Loading Scene", "Loading Scene");
        auto [main_success, main_key] = sceneManager.CreateNewScene<Scene>("Main Scene", "Main Scene");
        
        if (loading_success)
        {
            bool succeed = sceneManager.SetLoadingScene(load_key);
            std::cout << succeed << std::endl;
        }

        if (menu_success)
        {
            bool succeed = sceneManager.SetActiveScene(menu_key);
            std::cout << succeed << std::endl;
        }

        sceneManager.Init();

        std::cout << "=========================================================\n";
        sceneManager.Update();
        std::cout << "=========================================================\n";

        std::cout << "changing to Main Scene " << sceneManager.ChangeScene(main_key) << std::endl;

        std::cout << "=========================================================\n";
        sceneManager.Update();
        std::cout << "=========================================================\n";

        std::cout << "=========================================================\n";
        sceneManager.Update();
        std::cout << "=========================================================\n";

        std::cout << "Reloading " << sceneManager.ChangeScene(main_key) << std::endl;

        std::cout << "=========================================================\n";
        sceneManager.Update();
        std::cout << "=========================================================\n";

        std::cout << "=========================================================\n";
        sceneManager.Update();
        std::cout << "=========================================================\n";

        std::cout << "Change Scene To Loading " << sceneManager.ChangeScene(load_key) << std::endl;

        std::cout << "=========================================================\n";
        sceneManager.Update();
        std::cout << "=========================================================\n";

        std::cout << "=========================================================\n";
        sceneManager.Update();
        std::cout << "=========================================================\n";

        std::cout << "=========================================================\n";
        std::cout << "Terminating\n";
        sceneManager.Terminate();
        std::cout << "=========================================================\n";
    }

    return 0;
}
