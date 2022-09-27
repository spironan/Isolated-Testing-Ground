#include "..\include\Transform.h"
#include "..\include\Transform.h"
#include "..\include\Transform.h"
#include "Quaternion/include/Transform.h"
#include <glm/ext/matrix_transform.hpp>
#include <iomanip>

#include <glm/gtx/matrix_decompose.hpp>

/*-----------------------------------------------------------------------------*/
/* Getter Functions                                                            */
/*-----------------------------------------------------------------------------*/

//Transform3D::vec3 Transform3D::GetPosition() const { return Position; }

//Transform3D::quat Transform3D::GetRotationQuat() const { return m_orientation.value; }

Transform3D::vec3 Transform3D::GetEulerAnglesDeg() const { return glm::degrees(GetEulerAnglesRad()); }

Transform3D::vec3 Transform3D::GetEulerAnglesRad() const { return quaternion::to_euler(Orientation); }

Transform3D::mat4 Transform3D::GetRotationMatrix() const { return quaternion::to_matrix(Orientation); }

//Transform3D::vec3 Transform3D::GetScale() const { return m_scale; }

Transform3D::vec3 Transform3D::Right() const { return Transform[0]; }

Transform3D::vec3 Transform3D::Up() const { return Transform[1]; }

Transform3D::vec3 Transform3D::Forward() const { return Transform[2]; }

Transform3D::mat4 Transform3D::GetMatrix() const { return Transform; }

/*-----------------------------------------------------------------------------*/
/* Setter Functions                                                            */
/*-----------------------------------------------------------------------------*/

//Transform3D::vec3& Transform3D::Position() { return Position; }
//
//Transform3D::vec3& Transform3D::Scale() { return m_scale; }

//void Transform3D::SetPosition(vec3 pos) { Position = pos; }

void Transform3D::SetRotation(quat quaternion)
{
    Orientation.value = glm::normalize(quaternion.value);
}

//void Transform3D::SetScale(vec3 scale) { m_scale = scale; }

void Transform3D::SetTransform(mat4 desired_transform)
{
    Transform = desired_transform;
    DecomposeValues(Transform, Scale, Orientation.value, Position);
}

void Transform3D::CalculateTransform()
{
    auto t = glm::translate(glm::mat4{ 1.f }, Position);
    auto r = glm::mat4{ quaternion::to_matrix(Orientation) };
    auto s = glm::scale(glm::mat4{ 1.f }, Scale);

    Transform = t * r * s;
}

void Transform3D::DecomposeValues(mat4 matrix, glm::vec3& scale, glm::quat& orientation, glm::vec3& position)
{
    glm::vec3 unusedSkew;
    glm::vec4 unusedPerspective;
    glm::decompose(matrix, scale, orientation, position, unusedSkew, unusedPerspective);

}

void Transform3D::LookAt(vec3 target)
{
    Transform = glm::lookAt(Position, target, Up());
    DecomposeValues(Transform, Scale, Orientation.value, Position);
}


std::ostream& operator<< (std::ostream& os, Transform3D::vec3 vector)
{
    os << std::fixed << std::setprecision(4);

    os << "(" << vector.x << "," << vector.y << "," << vector.z << ")";
    return os;
}

std::ostream& operator<< (std::ostream& os, Transform3D::mat4 matrix)
{
    os << std::fixed << std::setprecision(4) << "\n";
    auto mat = glm::transpose(matrix);
    for (int i = 0; i < 4; ++i)
    {
        os << "[";
        for (int j = 0; j < 4; ++j)
        {
            os << " " << mat[i][j];
        }
        os << "]\n";
    }

    return os;
}