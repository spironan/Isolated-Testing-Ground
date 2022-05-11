#include "Quaternion.h"
#include <glm/gtc/type_ptr.hpp>

quaternion quaternion::identity()
{
    return { 0.0f, 0.0f, 0.0f, 1.0f };
}

quaternion quaternion::conjugate(quaternion const& input)
{
    return glm::conjugate(input.value);
    //return { -input.x, -input.y, -input.z, input.w };
}

quaternion quaternion::inverse(quaternion const& input)
{
    if (!is_normalized(input))
    {
        //Should assert here if quat is not magnitude of 1!
        assert(is_normalized(input), "quat magnitude must be 1 when inversing!");
    }
    return conjugate(input);
}

bool quaternion::is_normalized(quaternion const& input)
{
    return 1.0f - glm::dot(input.value, input.value) < std::numeric_limits<float>::epsilon();
    //return 1.0f - (input.x * input.x + input.y * input.y + input.z * input.z + input.w * input.w) < std::numeric_limits<float>::epsilon();
}

//glm::vec3 quaternion::rotate(quaternion const& quat, glm::vec3 const& vec)
//{
//    quaternion result = quat * quaternion{ vec.x, vec.y, vec.z, 0 } *quaternion::inverse(quat);
//    return { result.x, result.y, result.z };
//}

quaternion::matrix_type quaternion::to_matrix(quaternion const& quat)
{
    return glm::mat4_cast(quat.value);
    /*matrix_type result{};

    result[0][0] = 1.0f - (2 * quat.y * quat.y) - (2 * quat.z * quat.z);
    result[1][1] = 1.0f - (2 * quat.x * quat.x) - (2 * quat.z * quat.z);
    result[2][2] = 1.0f - (2 * quat.x * quat.x) - (2 * quat.y * quat.y);

    result[0][1] = 2 * quat.x * quat.y - 2 * quat.z * quat.w;
    result[0][2] = 2 * quat.x * quat.z + 2 * quat.y * quat.w;

    result[1][0] = 2 * quat.x * quat.y + 2 * quat.z * quat.w;
    result[1][2] = 2 * quat.y * quat.z - 2 * quat.x * quat.w;

    result[2][0] = 2 * quat.x * quat.z - 2 * quat.y * quat.w;
    result[2][1] = 2 * quat.y * quat.z + 2 * quat.x * quat.w;

    return result;*/
}

quaternion quaternion::from_matrix(matrix_type const& mat)
{
    return glm::quat_cast(mat);

    //quaternion result = quaternion::identity();

    //float trace = mat[0][0] + mat[1][1] + mat[2][2];
    //// check the diagonal
    //if (trace > 0.0f)
    //{
    //    value_type s = std::sqrt(trace + 1.0f);
    //    result[3] = s * 0.5f;
    //    value_type t = 0.5f / s;
    //    result[0] = (mat[2][1] - mat[1][2]) * t;
    //    result[1] = (mat[0][2] - mat[2][0]) * t;
    //    result[2] = (mat[1][0] - mat[0][1]) * t;
    //}
    //else
    //{
    //    // diagonal is negative
    //    int i = 0;
    //    if (mat[1][1] > mat[0][0]) i = 1;
    //    if (mat[2][2] > mat[i][i]) i = 2;
    //    static const int NEXT[3] = { 1, 2, 0 };
    //    int j = NEXT[i];
    //    int k = NEXT[j];
    //    value_type s = std::sqrt((mat[i][j] - (mat[j][j] + mat[k][k])) + 1.0f);
    //    result[i] = s * 0.5f;
    //    value_type t;
    //    if (s != 0.0) t = 0.5f / s;
    //    else t = s;
    //    result[3] = (mat[k][j] - mat[j][k]) * t;
    //    result[j] = (mat[j][i] + mat[i][j]) * t;
    //    result[k] = (mat[k][i] + mat[i][k]) * t;
    //}

    //return result;
}

quaternion quaternion::lerp(quaternion const& a, quaternion const& b, value_type weight)
{
    return glm::lerp(a.value, b.value, weight);

    /*value_type oneMinusWeight = 1.0f - weight;
    auto x = oneMinusWeight * a.x + weight * b.x;
    auto y = oneMinusWeight * a.y + weight * b.y;
    auto z = oneMinusWeight * a.z + weight * b.z;
    auto w = oneMinusWeight * a.w + weight * b.w;
    return normalize({ x,y,z,w });*/
}

quaternion quaternion::normalize(quaternion const& input)
{
    return { glm::normalize(input.value) };
    /*auto oneOverLength = 1.0f / std::sqrtf(input.x * input.x + input.y * input.y + input.z * input.z + input.w * input.w);
    return { input.x * oneOverLength, input.y * oneOverLength , input.z * oneOverLength , input.w * oneOverLength };*/
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
    return { glm::dot(lhs.value, rhs.value) };
    //return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

quaternion quaternion::from_euler(euler const& euler_angle_radians)
{
    quaternion quatAroundX = quaternion{ {1.0, 0.0, 0.0}, euler_angle_radians.x };
    quaternion quatAroundY = quaternion{ {0.0, 1.0, 0.0}, euler_angle_radians.y };
    quaternion quatAroundZ = quaternion{ {0.0, 0.0, 1.0}, euler_angle_radians.z };
    quaternion finalOrientation = quatAroundX * quatAroundY * quatAroundZ;

    return finalOrientation;

    //// yaw (Z), pitch (Y), roll (X)
    //// Abbreviations for the various angular functions
    //value_type cy = std::cosf(angle_radians.x * 0.5f);
    //value_type sy = std::sinf(angle_radians.x * 0.5f);
    //value_type cp = std::cosf(angle_radians.y * 0.5f);
    //value_type sp = std::sinf(angle_radians.y * 0.5f);
    //value_type cr = std::cosf(angle_radians.z * 0.5f);
    //value_type sr = std::sinf(angle_radians.z * 0.5f);

    //quaternion q = quaternion::identity();
    //q.w = cr * cp * cy + sr * sp * sy;
    //q.x = sr * cp * cy - cr * sp * sy;
    //q.y = cr * sp * cy + sr * cp * sy;
    //q.z = cr * cp * sy - sr * sp * cy;

    //return q;
}

quaternion::euler quaternion::to_euler(quaternion const& q)
{
    return glm::eulerAngles(q.value);

    //// yaw (Z), pitch (Y), roll (X)
    //euler angles{ 0.f };

    //// roll (x-axis rotation)
    //value_type sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    //value_type cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    //angles.x = std::atan2(sinr_cosp, cosr_cosp);

    //// pitch (y-axis rotation)
    //value_type sinp = 2 * (q.w * q.y - q.z * q.x);
    //if (std::abs(sinp) >= 1)
    //    angles.y = std::copysign(pi<value_type>() / 2.0f, sinp); // use 90 degrees if out of range
    //else
    //    angles.y = std::asin(sinp);

    //// yaw (z-axis rotation)
    //value_type siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    //value_type cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    //angles.z = std::atan2(siny_cosp, cosy_cosp);

    //return angles;
}

// Quarternion given an axis angle : make sure direction is normalized.
quaternion::quaternion(glm::vec3 dir_normalized, value_type angle_radians)
    : value { glm::angleAxis(angle_radians, dir_normalized) }
/*: x{ dir_normalized.x * std::sin(angle_radians / 2.0f) }
    , y{ dir_normalized.y * std::sin(angle_radians / 2.0f) }
    , z{ dir_normalized.z * std::sin(angle_radians / 2.0f) }
    , w{ std::cos(angle_radians / 2.0f) }*/
{
}

quaternion::quaternion(value_type x, value_type y, value_type z, value_type w)
    : value { x, y, z, w }
//: x{ x }, y{ y }, z{ z }, w{ w }
{
}

quaternion operator*(quaternion const& lhs, quaternion const& rhs)
{
    return lhs.value * rhs.value;

    /*auto Pv = glm::vec3{ lhs.x, lhs.y, lhs.z };
    auto Qv = glm::vec3{ rhs.x, rhs.y, rhs.z };
    auto vecRes = lhs.w * (Qv)+rhs.w * (Pv)+glm::cross(Pv, Qv);
    auto scalarRes = lhs.w * rhs.w - glm::dot(Pv, Qv);
    return { vecRes, scalarRes };*/
}

quaternion quaternion::normalize()
{
    *this = quaternion::normalize(*this);
    return *this;
}

quaternion::const_reference quaternion::operator[](size_type index) const
{
    return value[index];

    /*assert(index > 3, " index should not be larger than 4 ");
    switch (index)
    {
    case 0: return x;
    case 1: return y;
    case 2: return z;
    case 3: return w;

    default: return x;
    }*/
}

quaternion::reference quaternion::operator[](size_type index)
{
    return value[index];

    /*assert(index > 3, " index should not be larger than 4 ");
    switch (index)
    {
    case 0: return x;
    case 1: return y;
    case 2: return z;
    case 3: return w;

    default: return x;
    }*/
}

quaternion::value_type quaternion::angle() const
{
    return glm::angle(value);
}
