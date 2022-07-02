#include "../include/Quaternion.h"
#include <glm/gtc/type_ptr.hpp> 

quaternion quaternion::identity()
{
    return glm::identity<glm::quat>();
}

quaternion quaternion::conjugate(quaternion const& input)
{
    return { glm::conjugate(input.value) };
}

quaternion quaternion::inverse(quaternion const& input)
{
    return glm::inverse(input.value);
}

bool quaternion::is_normalized(quaternion const& input)
{
    return 1.0f - glm::dot(input.value, input.value) < std::numeric_limits<value_type>::epsilon();
}

glm::vec3 quaternion::rotate(quaternion const& quat, glm::vec3 const& vec)
{
    quaternion result = quat * quaternion{ vec.x, vec.y, vec.z, 0 } * quaternion::inverse(quat);
    return { result.value.x, result.value.y, result.value.z };
}

quaternion::matrix_type quaternion::to_matrix(quaternion const& quat)
{
    return glm::mat4_cast(quat.value);
}

quaternion quaternion::from_matrix(matrix_type const& mat)
{
    return glm::quat_cast(mat);
}

quaternion quaternion::lerp(quaternion const& a, quaternion const& b, value_type weight)
{
    return glm::lerp(a.value, b.value, weight);
}

quaternion quaternion::normalize(quaternion const& input)
{
    return { glm::normalize(input.value) };
}

quaternion::value_type quaternion::angle_radians(quaternion const& lhs, quaternion const& rhs)
{
    return std::acos(dot(lhs, rhs));
}

quaternion::value_type quaternion::angle_degrees(quaternion const& lhs, quaternion const& rhs)
{
    return degrees(angle_radians(lhs, rhs));
}

quaternion::value_type quaternion::dot(quaternion const& lhs, quaternion const& rhs)
{
    return {glm::dot(lhs.value, rhs.value)};
}

quaternion quaternion::from_euler(euler const& euler_angle_radians)
{
    quaternion quatAroundX = quaternion{{1.0, 0.0, 0.0}, euler_angle_radians.x};
    quaternion quatAroundY = quaternion{{0.0, 1.0, 0.0}, euler_angle_radians.y};
    quaternion quatAroundZ = quaternion{{0.0, 0.0, 1.0}, euler_angle_radians.z};
    quaternion finalOrientation = quatAroundX * quatAroundY * quatAroundZ;
    
    return finalOrientation;
}

// euler return is already in degrees.
quaternion::euler quaternion::to_euler(quaternion const& q)
{
    return glm::eulerAngles(q.value);
}

std::tuple<glm::vec3, quaternion::value_type> quaternion::to_axis_angle(quaternion q1)
{
    return { glm::axis(q1.value), glm::angle(q1.value) };
}

quaternion quaternion::from_axis_angle(glm::vec3 axis_normalized, quaternion::value_type angle)
{
    return { axis_normalized, angle };
}

// Quarternion given an axis angle : make sure direction is normalized.
quaternion::quaternion(glm::vec3 dir_normalized, value_type angle_radians)
    : value { glm::angleAxis(angle_radians, dir_normalized) }
{
}

quaternion::quaternion(value_type x, value_type y, value_type z, value_type w)
    : value { x, y, z, w }
{
}

quaternion operator*(quaternion const& lhs, quaternion const& rhs)
{
    return lhs.value * rhs.value;
}

quaternion quaternion::normalize()
{
    *this = quaternion::normalize(*this);
    return *this;
}

quaternion::const_reference quaternion::operator[](size_type index) const
{
    return value[static_cast<glm::length_t>(index)];
}

quaternion::reference quaternion::operator[](size_type index)
{
    return value[static_cast<glm::length_t>(index)];
}