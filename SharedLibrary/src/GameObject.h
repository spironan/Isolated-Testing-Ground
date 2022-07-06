#pragma once
#include "Scenegraph.h"
#include "Transform.h"


// class is an encapsulation of both scenegraph and transform as a means of testing
// not reflective of the final implementation and shouldnt be used for production.

using Entity = size_t;

class GameObject
{
private:
    static Entity index;
    Entity m_entt = index++;

    // every entity has a transform and scenegraph properties
    SceneNode m_node;
    Transform m_transform;

public:
    
    GameObject()
        : m_node{ "default", oo::UUID{} }
        , m_transform { }
    {
    }

    // for root creation only
    GameObject(SceneNode root_node)
        : m_node{ root_node }
        , m_transform{ }
    {
    }
    explicit GameObject(Entity entt);
    GameObject(std::string const& name);
    GameObject(std::string const& name, oo::UUID uuid);

    GameObject(GameObject const& other)
        : m_entt { other.m_entt }
        , m_node { other.m_node }
        , m_transform { other.m_transform }
    {
    }

    void Destroy();

    void Print() const;
    Entity GetEntity() const;
    Transform GetTransform() const { return m_transform; }
    SceneNode GetSceneNode() const { return m_node; }
    
    Transform& Transform() { return m_transform; }
    SceneNode& SceneNode() { return m_node; }

    void AddChild(GameObject& target);

    GameObject GetParent();
    std::vector<GameObject> GetChilds(bool includeItself = false);
};

void PrintGo(GameObject const* go);

extern "C"
{
    // global scenegraph
    extern Scenegraph g_sceneGraph;
    extern GameObject g_root;
    extern std::unordered_map<Scenegraph::Key, GameObject*> g_sceneData;
}
