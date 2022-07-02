#include <iostream>
#include "src/GameObject.h"

void printvalues(Transform const& transform);

void printvalues(Transform const& transform)
{
    std::cout << "===========================================================\n";

    std::cout << "Local Pos " << transform.GetPosition() << "\n";
    std::cout << "Local Rot " << transform.GetEulerAngles() << "\n";
    std::cout << "Local Sca " << transform.GetScale() << "\n";
    std::cout << "Local Mat " << transform.GetLocalMatrix() << "\n";

    std::cout << "Global Pos " << transform.GetGlobalPosition() << "\n";
    std::cout << "Global Rot(Deg) " << transform.GetGlobalRotationDeg() << "\n";
    std::cout << "Global Rot(Rad)" << transform.GetGlobalRotationRad() << "\n";
    std::cout << "Global Rot(Mat)" << transform.GetGlobalRotationMatrix() << "\n";
    std::cout << "Global Sca" << transform.GetGlobalScale() << "\n";
    std::cout << "Global Mat" << transform.GetGlobalMatrix() << "\n";

    std::cout << "Has Changed " << transform.HasChanged() << "\n";
    std::cout << "Is Dirty " << transform.IsDirty() << "\n";

    std::cout << "Local Right " << transform.LocalRight() << "\n";
    std::cout << "Local Up " << transform.LocalUp() << "\n";
    std::cout << "Local Forward" << transform.LocalForward() << "\n";

    std::cout << "===========================================================\n";
}


int main()
{
    // Combined GameObject-Transform-SceneGraph Test
    
    GameObject go("go");
    go.Transform().Position() = { 1, 0, 0 };
    GameObject go1("go1");
    go1.Transform().Position() = { 0, 1, 0 };
    GameObject go2("go2");
    go2.Transform().Position() = { 0, 0, 1 };
    
    go.AddChild(go1);
    go1.AddChild(go2);
    
    // simulate transform update
    for (auto& elem : g_sceneGraph.GetRootChilds())
    {
        auto& tf = g_sceneData.at(elem)->Transform();
        if (tf.IsDirty())
        {
            tf.CalculateLocalTransform();
        }
        
        SceneNode& sceneNode = g_sceneData.at(elem)->SceneNode();
        
        GameObject parent = *g_sceneData.at(sceneNode.getParentHandle());
        auto parentTf = parent.GetTransform();
        tf.m_globalTransform = parentTf.m_globalTransform * tf.m_localTransform;

        for (auto& child : g_sceneGraph.GetChilds(&sceneNode))
        {
            g_sceneData.at(child)->Transform().m_dirty = true;
        }
        
    }

    //print values
    for (auto& child : g_sceneGraph.GetRootChilds())
    {
        PrintGo(g_sceneData.at(child));
        printvalues(g_sceneData.at(child)->GetTransform());
    }
    
    g_sceneGraph.Print();
}
