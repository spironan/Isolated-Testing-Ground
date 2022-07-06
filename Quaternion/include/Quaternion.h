#pragma once
#include <tuple>

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/trigonometric.hpp>

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
static constexpr T pi() { return static_cast<T>(3.14159253589793243846); }

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
static constexpr T rad_to_deg() { return static_cast<T>(180.0) / pi<T>(); }

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
static constexpr T deg_to_rad() { return pi<T>() / static_cast<T>(180.0); }

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
static constexpr T radians(T const& degrees) { return degrees * deg_to_rad<T>(); }

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
static constexpr T degrees(T const& radians) { return radians * rad_to_deg<T>(); }

struct quaternion
{
public:
    using euler = glm::vec3;
    using matrix_type = glm::mat3;

    using size_type = std::size_t;
    using value_type = glm::quat::value_type;
    using reference = value_type&;
    using const_reference = value_type const&;

public:
    static quaternion identity();

    static quaternion conjugate(quaternion const& input);

    static quaternion inverse(quaternion const& input);

    static bool is_normalized(quaternion const& input);

    static glm::vec3 rotate(quaternion const& quat, glm::vec3 const& vec);

    static matrix_type to_matrix(quaternion const& quat);

    static quaternion from_matrix(matrix_type const& mat);

    static quaternion lerp(quaternion const& a, quaternion const& b, value_type weight);

    static quaternion normalize(quaternion const& input);

    static value_type angle_radians(quaternion const& lhs, quaternion const& rhs);

    static value_type angle_degrees(quaternion const& lhs, quaternion const& rhs);

    static value_type dot(quaternion const& lhs, quaternion const& rhs);

    static quaternion from_euler(euler const& angle_radians);

    static euler to_euler(quaternion const& q);

    static std::tuple<glm::vec3, value_type> to_axis_angle(quaternion q1);

    static quaternion from_axis_angle(glm::vec3 axis_normalized, value_type angle);

public:

    quaternion(glm::quat quaternion)
        : value{ quaternion }
    {
    }

    // i j k w
    // x y z w
    // Quarternion given an axis angle : make sure direction is normalized.
    quaternion(glm::vec3 dir_normalized, value_type angle_radians);

    quaternion(value_type x, value_type y, value_type z, value_type w);

    friend quaternion operator*(quaternion const& lhs, quaternion const& rhs);

    quaternion normalize();

    value_type angle() const;

    const_reference operator[](size_type index) const;

    reference operator[](size_type index);

    glm::quat value;
    //value_type x, y, z, w;
};

