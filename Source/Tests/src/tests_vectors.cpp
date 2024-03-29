#include "thirdparty_catch_amalgamated.hpp"
#include "base_vectors.h"

TEST_CASE("Sisyphus::Base::vec3_t tests", "[Base::vec3_t]")
{
    {
        Sisyphus::Base::vec3_t a {1, 2, 3};
        INFO("Testing vec3_t creation failed: " << a << " should be [1 2 3]");
        REQUIRE((EQUAL_FLOATS(a.x, 1.0f) && EQUAL_FLOATS(a.y, 2.0f) && EQUAL_FLOATS(a.z, 3.0f)));
    }
    {
        Sisyphus::Base::vec3_t a {1, 2, 3};
        a *= 3.0f;
        INFO("Testing vec3_t *= 3 failed: " << a << " should be [3 6 9]");
        REQUIRE(a == Sisyphus::Base::vec3_t {3.0f, 6.0f, 9.0f});
    }
    {
        Sisyphus::Base::vec3_t a {1, 2, -2.5f};
        Sisyphus::Base::vec3_t b {-2, -4, 5};
        INFO("Testing vec3_t * -2 failed: " << a << " * " << -2 << " != " << b);
        REQUIRE(a * -2 == b);
    }
    {
        Sisyphus::Base::vec3_t a {12.4f, 2.2f, -2.5f};
        a /= -2.0f;
        Sisyphus::Base::vec3_t c {-6.2f, -1.1f, 1.25f};
        INFO("Testing vec3_t /= -2 failed: " << a << " != " << c);
        REQUIRE(a == c);
    }
    {
        Sisyphus::Base::vec3_t a {12.4f, 2.2f, -2.5f};
        Sisyphus::Base::vec3_t c {6.2f, 1.1f, -1.25f};
        INFO("Testing vec3_t / 2 failed: " << a << " / " << 2 << " != " << c);
        REQUIRE(a / 2 == c);
    }
    {
        Sisyphus::Base::vec3_t a {1, 2, 3};
        Sisyphus::Base::vec3_t b {-1, 3, 4};
        a += b;
        Sisyphus::Base::vec3_t c {0, 5, 7};
        INFO("Testing vec3_t += failed: " << a << " != " << c);
        REQUIRE(a == c);
    }
    {
        Sisyphus::Base::vec3_t a {1, 2, 3};
        Sisyphus::Base::vec3_t b {-1, 3, 4};
        a -= b;
        Sisyphus::Base::vec3_t c {2, -1, -1};
        INFO("Testing vec3_t -= failed: " << a << " != " << c);
        REQUIRE(a == c);
    }
    {
        Sisyphus::Base::vec3_t a {1, 2, 3};
        Sisyphus::Base::vec3_t b {-1, 3, 4};
        Sisyphus::Base::vec3_t c {0, 5, 7};
        INFO("Testing vec3_t + failed: " << a << " + " << b << " != " << c);
        REQUIRE(a + b == c);
    }
    {
        Sisyphus::Base::vec3_t a {12.4f, 2.1f, -2.5f};
        Sisyphus::Base::vec3_t c {0.0f, 0.0f, 0.0f};
        INFO("Testing vec3_t - failed: " << a << " - " << a << " != " << c);
        REQUIRE(a - a == c);
    }
    {
        Sisyphus::Base::vec3_t a {12.4f, 2.1f, -2.5f};
        float                  magnitude = sqrt(12.4f * 12.4f + 2.1f * 2.1f + 2.5f * 2.5f);
        INFO(
            "Testing vec3_t magnitude failed: ||" << a << "|| -> " << a.calculate_magnitude()
                                                  << " and it is != " << magnitude);
        REQUIRE(EQUAL_FLOATS(a.calculate_magnitude(), magnitude));
    }
    {
        Sisyphus::Base::vec3_t a {12.4f, 2.1f, -2.5f};
        Sisyphus::Base::vec3_t u = a.calculate_normalized();
        float                  magnitude = sqrt(12.4f * 12.4f + 2.1f * 2.1f + 2.5f * 2.5f);
        Sisyphus::Base::vec3_t c {12.4f / magnitude, 2.1f / magnitude, -2.5f / magnitude};
        INFO("Testing vec3_t norm failed: norm(" << a << ") -> " << u << " and it is != " << c);
        REQUIRE(u == c);
    }
}

TEST_CASE("Sisyphus::Base::vec4_t tests", "[Base::vec4_t]")
{
    {
        Sisyphus::Base::vec4_t a {1, 2, 3, 4.65f};
        INFO("Testing vec4_t creation failed: " << a << " should be [1 2 3 4.65]");
        REQUIRE((
            EQUAL_FLOATS(a.x, 1.0f) && EQUAL_FLOATS(a.y, 2.0f) && EQUAL_FLOATS(a.z, 3.0f) && EQUAL_FLOATS(a.w, 4.65f)));
    }
    {
        Sisyphus::Base::vec4_t a {1, 2, 3};
        INFO("Testing vec4_t creation failed: " << a << " should be [1 2 3 0]");
        REQUIRE(
            (EQUAL_FLOATS(a.x, 1.0f) && EQUAL_FLOATS(a.y, 2.0f) && EQUAL_FLOATS(a.z, 3.0f) && EQUAL_FLOATS(a.w, 0.0f)));
    }
    {
        Sisyphus::Base::vec3_t a {1, 2, 3};
        Sisyphus::Base::vec4_t b {a.x, a.y, a.z, 0};
        INFO("Testing vec4_t creation failed: " << b << " should be [1 2 3 0]");
        REQUIRE(
            (EQUAL_FLOATS(b.x, 1.0f) && EQUAL_FLOATS(b.y, 2.0f) && EQUAL_FLOATS(b.z, 3.0f) && EQUAL_FLOATS(b.w, 0.0f)));
    }
    {
        Sisyphus::Base::vec4_t a {1, 2, 3, 4.65f};
        a *= 3.0f;
        INFO("Testing vec4_t *= 3 failed: " << a << " should be [3 6 9, 13.95]");
        REQUIRE(a == Sisyphus::Base::vec4_t {3.0f, 6.0f, 9.0f, 13.95f});
    }
    {
        Sisyphus::Base::vec4_t a {1, 2, -2.5f, 3.7f};
        Sisyphus::Base::vec4_t b {-2, -4, 5, -7.4f};
        INFO("Testing vec4_t * -2 by scalar failed: " << a << " * " << -2 << " != " << b);
        REQUIRE(a * -2 == b);
    }
    {
        Sisyphus::Base::vec4_t a {12.4f, 2.2f, -2.5f, 3.7f};
        a /= -2.0f;
        Sisyphus::Base::vec4_t c {-6.2f, -1.1f, 1.25f, -1.85f};
        INFO("Testing vec4_t /= -2 failed: " << a << " != " << c);
        REQUIRE(a == c);
    }
    {
        Sisyphus::Base::vec4_t a {12.4f, 2.2f, -2.5f, 3.7f};
        Sisyphus::Base::vec4_t c {6.2f, 1.1f, -1.25f, 1.85f};
        INFO("Testing vec4_t / 2 failed: " << a << " / " << 2 << " != " << c);
        REQUIRE(a / 2 == c);
    }
    {
        Sisyphus::Base::vec4_t a {1, 2, 3, 4};
        Sisyphus::Base::vec4_t b {-1, 3, 4, 6};
        a += b;
        Sisyphus::Base::vec4_t c {0, 5, 7, 10};
        INFO("Testing vec4_t += failed: " << a << " != " << c);
        REQUIRE(a == c);
    }
    {
        Sisyphus::Base::vec4_t a {1, 2, 3, 6};
        Sisyphus::Base::vec4_t b {-1, 3, 4, 6};
        a -= b;
        Sisyphus::Base::vec4_t c {2, -1, -1, 0};
        INFO("Testing vec4_t -= failed: " << a << " != " << c);
        REQUIRE(a == c);
    }
    {
        Sisyphus::Base::vec4_t a {1, 2, 3, 14};
        Sisyphus::Base::vec4_t b {-1, 3, 4, -4.2f};
        Sisyphus::Base::vec4_t c {0, 5, 7, 9.8f};
        INFO("Testing vec4_t + failed: " << a << " + " << b << " != " << c);
        REQUIRE(a + b == c);
    }
    {
        Sisyphus::Base::vec4_t a {12.4f, 2.1f, -2.5f, 3.14f};
        Sisyphus::Base::vec4_t c {0.0f, 0.0f, 0.0f, 0.0f};
        INFO("Testing vec4_t - failed: " << a << " - " << a << " != " << c);
        REQUIRE(a - a == c);
    }
    {
        Sisyphus::Base::vec4_t a {12.4f, 2.1f, -2.5f, 3.9f};
        float                  magnitude = sqrt(12.4f * 12.4f + 2.1f * 2.1f + 2.5f * 2.5f + 3.9f * 3.9f);
        INFO(
            "Testing vec4_t magnitude failed: ||" << a << "|| -> " << a.calculate_magnitude()
                                                  << " and it is != " << magnitude);
        REQUIRE(EQUAL_FLOATS(a.calculate_magnitude(), magnitude));
    }
    {
        Sisyphus::Base::vec4_t a {12.4f, 2.1f, -2.5f, 1.2f};
        Sisyphus::Base::vec4_t u = a.calculate_normalized();
        float                  magnitude = sqrt(12.4f * 12.4f + 2.1f * 2.1f + 2.5f * 2.5f + 1.2f * 1.2f);
        Sisyphus::Base::vec4_t c {12.4f / magnitude, 2.1f / magnitude, -2.5f / magnitude, 1.2f / magnitude};
        INFO("Testing vec4_t norm failed: norm(" << a << ") -> " << u << " and it is != " << c);
        REQUIRE(u == c);
    }
}
