#ifndef vec2_h
#define vec2_h

#include <math.h>

#define VEC_EPSILON 0.001f

struct vec2_t {
    float x;
    float y;
    
    void zero() {
        x = y = 0.0f;
    }
    
    float length() const {
        return sqrtf( x * x + y * y );
    }
    
    float lengthSquared() const {
        return x * x + y * y;
    }

    vec2_t normalized() const {
        vec2_t result;
        
        float sqr_length = x * x + y * y;
        float inv_length = 1.0f / sqrtf( sqr_length );
        result.x = x * inv_length;
        result.y = y * inv_length;
        
        return result;
    }

    bool compare( const vec2_t & a, float epsilon = VEC_EPSILON ) const
    {
        if ( fabsf( x - a.x ) > epsilon )
            return false;
                
        if ( fabsf( y - a.y ) > epsilon )
            return false;

        return true;
    }

    vec2_t rotated( float degrees )
    {
        float radians = degrees * M_PI / 180.0f;
        
        return (vec2_t){
            .x = cos(radians) * x - sin(radians) * y,
            .y = sin(radians) * x + cos(radians) * y
        };
    }
    
    vec2_t operator+( const vec2_t & a ) const { return (vec2_t){ x + a.x, y + a.y }; }
    vec2_t operator-( const vec2_t & a ) const { return (vec2_t){ x - a.x, y - a.y }; }
    vec2_t operator*( float a ) const { return (vec2_t){ x * a, y * a }; }
    void operator+=( const vec2_t & a ) { x += a.x; y += a.y; }
    void operator-=( const vec2_t & a ) { x -= a.x; y -= a.y; }
    void operator*=( float a ) { x *= a; y *= a; }
    bool operator==( vec2_t & a ) const { return a.x == x && a.y == y; }
    bool operator!=( vec2_t & a ) const { return a.x != x || a.y != y; }
    vec2_t operator-() const { return (vec2_t){ -x, -y }; }
    vec2_t operator/( float a ) const {
        float invalid = 1.0f / a;
        return (vec2_t){ x * invalid, y * invalid };
    }
    
    void operator/=( float a ) {
        float invalid = 1.0f / a;
        x *= invalid;
        y *= invalid;
    }
};

#endif /* vec2_h */
