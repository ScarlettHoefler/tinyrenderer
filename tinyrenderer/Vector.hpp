//
//  Vector.hpp
//  tinyrenderer
//
//  Created by Scarlett Hoefler on 2/3/19.
//  Copyright © 2019 Scarlett Hoefler. All rights reserved.
//

#ifndef Vector_hpp
#define Vector_hpp

#include <cmath>
#include <cassert>

template<typename T>
struct Vector2 {
    union {
        struct { T u, v; };
        struct { T x, y; };
        T raw[2];
    };
    
    Vector2()
    : Vector2(0,0)
    {
    }
    
    Vector2(T x, T y)
    : x(x), y(y)
    {
    }
};

typedef Vector2<int> Vector2i;
typedef Vector2<float> Vector2f;

template<typename T>
struct Vector3 {
    union {
        struct { T x, y, z; };
        T raw[3];
    };
    
    Vector3()
    : Vector3(0,0,0)
    {
    }
    
    Vector3(T x, T y, T z)
    : x(x), y(y), z(z)
    {
    }
    
    Vector3<T> cross(const Vector3 &other) const;
    float dot(const Vector3 &rhs) const;
    Vector3<T> normalized() const;
    float magnitude() const;
    
    Vector3<T> operator-(const Vector3 &rhs) const;
    Vector3<T> operator*(float rhs) const;
    
    T& operator[](int index) const;
};

typedef Vector3<float> Vector3f;

template<typename T>
Vector3<T> Vector3<T>::cross(const Vector3<T> &rhs) const {
    Vector3<T> result;
    result.x = (y * rhs.z) - (z * rhs.y);
    result.y = (z * rhs.x) - (x * rhs.z);
    result.z = (x * rhs.y) - (y * rhs.x);
    return result;
}

template<typename T>
float Vector3<T>::dot(const Vector3<T> &rhs) const {
    return x*rhs.x + y*rhs.y + z*rhs.z;
}

template<typename T>
Vector3<T> Vector3<T>::normalized() const {
    float magnitude = std::sqrt(x*x + y*y + z*z);
    assert(magnitude > 0);
    
    Vector3<T> result;
    result.x = static_cast<T>(x / magnitude);
    result.y = static_cast<T>(y / magnitude);
    result.z = static_cast<T>(z / magnitude);
    return result;
}

template<typename T>
float Vector3<T>::magnitude() const {
    return std::sqrt(x*x + y*y + z*z);
}

template<typename T>
Vector3<T> Vector3<T>::operator-(const Vector3 &rhs) const {
    return Vector3(x-rhs.x, y-rhs.y, z-rhs.z);
}

template<typename T>
Vector3<T> Vector3<T>::operator*(float rhs) const {
    return Vector3(x*rhs, y*rhs, z*rhs);
}

template<typename T>
Vector3<T> operator*(float x, const Vector3<T> &rhs) {
    return rhs * x;
}

template<typename T>
T& Vector3<T>::operator[](int index) const {
    return raw[index];
}


#endif /* Vector_hpp */
