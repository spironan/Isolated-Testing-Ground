#include <iostream>
#include "Transform.h"


void printvalues(Transform const& transform);

int main()
{
    {
        Transform rawTF;
        printvalues(rawTF);
    }

    {
        Transform transform;

        transform.Position()    = {1, 1, 1};
        transform.Scale()       = { 10, 10, 10 };
        transform.SetEulerAngles({ 10, 20, 30 });

        printvalues(transform);
    }

    {
        Transform transform;

        transform.Position()    = { .5f, 10, -1 };
        transform.Scale()       = { 10, .2f, -2.f };
        transform.SetEulerAngles({ 20, 20, 20});
        transform.CalculateLocalTransform();
        transform.SetGlobalTransform(transform.GetPosition(), transform.GetEulerAngles(), transform.GetScale());

        printvalues(transform);
    }
}

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

