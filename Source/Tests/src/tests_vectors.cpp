#include "thirdparty_catch_amalgamated.hpp"
#include "base_vectors.h"

TEST_CASE("Temple::Base::vec3 tests", "[Base::vec3]") {
    {
        Temple::Base::vec3 a(1, 2, 3);
        INFO("Testing vec3 creation failed: " << a << " should be [1 2 3]");
        REQUIRE((EQUAL_FLOATS(a.x, 1.0f) && EQUAL_FLOATS(a.y, 2.0f) && EQUAL_FLOATS(a.z, 3.0f)));
    }
    {
        Temple::Base::vec3 a(1, 2, 3);
        a *= 3.0f;
        INFO("Testing vec3 *= 3 failed: " << a << " should be [3 6 9]");
        REQUIRE(a == Temple::Base::vec3(3.0f, 6.0f, 9.0f));
    }
    {
        Temple::Base::vec3 a(1, 2, -2.5f);
        Temple::Base::vec3 b(-2, -4, 5);
        INFO("Testing vec3 * -2 failed: " << a << " * " << -2 << " != " << b);
        REQUIRE(a * -2 == b);
    }
    {
        Temple::Base::vec3 a(12.4f, 2.2f, -2.5f);
        a /= -2.0f;
        Temple::Base::vec3 c(-6.2f, -1.1f, 1.25f);
        INFO("Testing vec3 /= -2 failed: " << a << " != " << c);
        REQUIRE(a == c);
    }
    {
        Temple::Base::vec3 a(12.4f, 2.2f, -2.5f);
        Temple::Base::vec3 c(6.2f, 1.1f, -1.25f);
        INFO("Testing vec3 / 2 failed: " << a << " / " << 2 << " != " << c);
        REQUIRE(a / 2 == c);
    }
    {
        Temple::Base::vec3 a(1, 2, 3);
        Temple::Base::vec3 b(-1, 3, 4);
        a += b;
        Temple::Base::vec3 c(0, 5, 7);
        INFO("Testing vec3 += failed: " << a << " != " << c);
        REQUIRE(a == c);
    }
    {
        Temple::Base::vec3 a(1, 2, 3);
        Temple::Base::vec3 b(-1, 3, 4);
        a -= b;
        Temple::Base::vec3 c(2, -1, -1);
        INFO("Testing vec3 -= failed: " << a << " != " << c);
        REQUIRE(a == c);
    }
    {
        Temple::Base::vec3 a(1, 2, 3);
        Temple::Base::vec3 b(-1, 3, 4);
        Temple::Base::vec3 c(0, 5, 7);
        INFO("Testing vec3 + failed: " << a << " + " << b << " != " << c);
        REQUIRE(a + b == c);
    }
    {
        Temple::Base::vec3 a(12.4f, 2.1f, -2.5f);
        Temple::Base::vec3 c;
        INFO("Testing vec3 - failed: " << a << " - " << a << " != " << c);
        REQUIRE(a - a == c);
    }
    {
        Temple::Base::vec3 a(12.4f, 2.1f, -2.5f);
        float magnitude = sqrt(12.4f * 12.4f + 2.1f * 2.1f + 2.5f * 2.5f);
        INFO("Testing vec3 magnitude failed: ||" << a << "|| -> " << a.magnitude() << " and it is != " << magnitude);
        REQUIRE(EQUAL_FLOATS(a.magnitude(), magnitude));
    }
    {
        Temple::Base::vec3 a(12.4f, 2.1f, -2.5f);
        Temple::Base::vec3 u = a.norm();
        float magnitude = sqrt(12.4f * 12.4f + 2.1f * 2.1f + 2.5f * 2.5f);
        Temple::Base::vec3 c(12.4f / magnitude, 2.1f / magnitude, -2.5f / magnitude);
        INFO("Testing vec3 norm failed: norm(" << a << ") -> " << u << " and it is != " << c);
        REQUIRE(u == c);
    }
}

TEST_CASE("Temple::Base::vec4 tests", "[Base::vec4]") {
    {
        Temple::Base::vec4 a(1, 2, 3, 4.65f);
        INFO("Testing vec4 creation failed: " << a << " should be [1 2 3 4.65]");
        REQUIRE((EQUAL_FLOATS(a.x, 1.0f) && EQUAL_FLOATS(a.y, 2.0f) && EQUAL_FLOATS(a.z, 3.0f) && EQUAL_FLOATS(a.w, 4.65f)));
    }
    {
        Temple::Base::vec4 a(1, 2, 3);
        INFO("Testing vec4 creation failed: " << a << " should be [1 2 3 0]");
        REQUIRE((EQUAL_FLOATS(a.x, 1.0f) && EQUAL_FLOATS(a.y, 2.0f) && EQUAL_FLOATS(a.z, 3.0f) && EQUAL_FLOATS(a.w, 0.0f)));
    }
    {
        Temple::Base::vec3 a(1, 2, 3);
        Temple::Base::vec4 b(a, 0);
        INFO("Testing vec4 creation failed: " << b << " should be [1 2 3 0]");
        REQUIRE((EQUAL_FLOATS(b.x, 1.0f) && EQUAL_FLOATS(b.y, 2.0f) && EQUAL_FLOATS(b.z, 3.0f) && EQUAL_FLOATS(b.w, 0.0f)));
    }
    {
        Temple::Base::vec4 a(1, 2, 3, 4.65f);
        a *= 3.0f;
        INFO("Testing vec4 *= 3 failed: " << a << " should be [3 6 9, 13.95]");
        REQUIRE(a == Temple::Base::vec4(3.0f, 6.0f, 9.0f, 13.95f));
    }
    {
        Temple::Base::vec4 a(1, 2, -2.5f, 3.7f);
        Temple::Base::vec4 b(-2, -4, 5, -7.4f);
        INFO("Testing vec4 * -2 by scalar failed: " << a << " * " << -2 << " != " << b);
        REQUIRE(a * -2 == b);
    }
    {
        Temple::Base::vec4 a(12.4f, 2.2f, -2.5f, 3.7f);
        a /= -2.0f;
        Temple::Base::vec4 c(-6.2f, -1.1f, 1.25f, -1.85f);
        INFO("Testing vec4 /= -2 failed: " << a << " != " << c);
        REQUIRE(a == c);
    }
    {
        Temple::Base::vec4 a(12.4f, 2.2f, -2.5f, 3.7f);
        Temple::Base::vec4 c(6.2f, 1.1f, -1.25f, 1.85f);
        INFO("Testing vec4 / 2 failed: " << a << " / " << 2 << " != " << c);
        REQUIRE(a / 2 == c);
    }
    {
        Temple::Base::vec4 a(1, 2, 3, 4);
        Temple::Base::vec4 b(-1, 3, 4, 6);
        a += b;
        Temple::Base::vec4 c(0, 5, 7, 10);
        INFO("Testing vec4 += failed: " << a << " != " << c);
        REQUIRE(a == c);
    }
    {
        Temple::Base::vec4 a(1, 2, 3, 6);
        Temple::Base::vec4 b(-1, 3, 4, 6);
        a -= b;
        Temple::Base::vec4 c(2, -1, -1, 0);
        INFO("Testing vec4 -= failed: " << a << " != " << c);
        REQUIRE(a == c);
    }
    {
        Temple::Base::vec4 a(1, 2, 3, 14);
        Temple::Base::vec4 b(-1, 3, 4, -4.2f);
        Temple::Base::vec4 c(0, 5, 7, 9.8f);
        INFO("Testing vec4 + failed: " << a << " + " << b << " != " << c);
        REQUIRE(a + b == c);
    }
    {
        Temple::Base::vec4 a(12.4f, 2.1f, -2.5f, 3.14f);
        Temple::Base::vec4 c;
        INFO("Testing vec4 - failed: " << a << " - " << a << " != " << c);
        REQUIRE(a - a == c);
    }
    {
        Temple::Base::vec4 a(12.4f, 2.1f, -2.5f, 3.9f);
        float magnitude = sqrt(12.4f * 12.4f + 2.1f * 2.1f + 2.5f * 2.5f + 3.9f * 3.9f);
        INFO("Testing vec4 magnitude failed: ||" << a << "|| -> " << a.magnitude() << " and it is != " << magnitude);
        REQUIRE(EQUAL_FLOATS(a.magnitude(), magnitude));
    }
    {
        Temple::Base::vec4 a(12.4f, 2.1f, -2.5f, 1.2f);
        Temple::Base::vec4 u = a.norm();
        float magnitude = sqrt(12.4f * 12.4f + 2.1f * 2.1f + 2.5f * 2.5f + 1.2f * 1.2f);
        Temple::Base::vec4 c(12.4f / magnitude, 2.1f / magnitude, -2.5f / magnitude, 1.2f / magnitude);
        INFO("Testing vec4 norm failed: norm(" << a << ") -> " << u << " and it is != " << c);
        REQUIRE(u == c);
    }
}
