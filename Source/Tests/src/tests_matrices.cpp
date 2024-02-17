#include "thirdparty_catch_amalgamated.hpp"
#include "base_matrices.h"
#include "base_utils.h"

TEST_CASE("Temple::Base::mat3_t tests", "[Base::mat3_t]")
{
    Temple::Base::mat3_t default_matrix {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
    Temple::Base::mat3_t zero_matrix = {};
    SECTION("mat3_t default constructor - all values should be zero")
    {
        Temple::Base::mat3_t M = {};
        REQUIRE(
            (EQUAL_FLOATS(0.0f, M.r0.calculate_magnitude()) && EQUAL_FLOATS(0.0f, M.r1.calculate_magnitude()) &&
             EQUAL_FLOATS(0.0f, M.r2.calculate_magnitude())));
    }
    SECTION("mat3_t constructor with 9 floats and equality operator")
    {
        Temple::Base::mat3_t M {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
        REQUIRE(
            (EQUAL_FLOATS(1.0f, M(0, 0)) && EQUAL_FLOATS(2.0f, M(0, 1)) && EQUAL_FLOATS(3.0f, M(0, 2)) &&
             EQUAL_FLOATS(4.0f, M(1, 0)) && EQUAL_FLOATS(5.0f, M(1, 1)) && EQUAL_FLOATS(6.0f, M(1, 2)) &&
             EQUAL_FLOATS(7.0f, M(2, 0)) && EQUAL_FLOATS(8.0f, M(2, 1)) && EQUAL_FLOATS(9.0f, M(2, 2)) &&
             M == default_matrix));
    }
    SECTION("mat3_t constructor with 3 vec3_t (rows)")
    {
        Temple::Base::vec3_t a {1.0f, 2.0f, 3.0f};
        Temple::Base::vec3_t b {4.0f, 5.0f, 6.0f};
        Temple::Base::vec3_t c {7.0f, 8.0f, 9.0f};
        Temple::Base::mat3_t M {a.x, a.y, a.z, b.x, b.y, b.z, c.x, c.y, c.z};
        REQUIRE(M == default_matrix);
    }
    SECTION("mat3_t constructor with another mat3_t&")
    {
        Temple::Base::mat3_t N {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
        Temple::Base::mat3_t M = N;
        REQUIRE((M == default_matrix && M == N));
    }
    SECTION("mat3_t inequality")
    {
        Temple::Base::mat3_t M(default_matrix);
        M(0, 0) += 0.001f;
        REQUIRE(M != default_matrix);
    }
    SECTION("Simple mat3_t arythmetic operations")
    {
        Temple::Base::mat3_t M(default_matrix);
        Temple::Base::mat3_t N = 2.0f * M;
        Temple::Base::mat3_t M2 {2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 18.0f};
        REQUIRE(N == M2);
        M *= 2.0f;
        REQUIRE(M == M2);
        N = M / 2.0f;
        REQUIRE(N == default_matrix);
        M /= 2.0f;
        REQUIRE(M == default_matrix);
        M -= default_matrix;
        REQUIRE(M == zero_matrix);
        M += default_matrix;
        REQUIRE(M == default_matrix);
        M = 2.0f * M - N;
        REQUIRE(M == default_matrix);
        N *= 0.5f;
        M = M + 3.0f * M - N / 0.25f;
        REQUIRE(M == 4.0f * N);
        M = -(-M);
        REQUIRE(M == 4.0f * N);
    }
    SECTION("mat3_t multiplications for vec3_t and another mat3_t")
    {
        SECTION("  Example 1")
        {
            Temple::Base::mat3_t A {1, 2, 3, 4, 5, 6, 7, 8, 9};
            Temple::Base::mat3_t B {9, 8, 7, 6, 5, 4, 3, 2, 1};
            Temple::Base::mat3_t result = A * B;
            Temple::Base::mat3_t expected {30, 24, 18, 84, 69, 54, 138, 114, 90};
            REQUIRE(result == expected);
        }

        SECTION("  Example 2")
        {
            Temple::Base::mat3_t C {-1, 0, 3, 2, -4, 1, 3, 2, -2};
            Temple::Base::mat3_t D {3, -2, 1, 1, 4, -3, -1, 0, 5};
            Temple::Base::mat3_t result = C * D;
            Temple::Base::mat3_t expected {-6, 2, 14, 1, -20, 19, 13, 2, -13};
            REQUIRE(result == expected);
        }

        SECTION("  Example 3")
        {
            Temple::Base::mat3_t A {2, -1, 3, 0, 1, -2, -1, 2, 1};
            Temple::Base::vec3_t v {3, -1, 2};
            Temple::Base::vec3_t result = A * v;
            Temple::Base::vec3_t expected {13, -5, -3};
            REQUIRE(result == expected);
        }

        SECTION("  Example 4")
        {
            Temple::Base::mat3_t B {-1, 2, 0, 3, -1, -2, 1, 0, 2};
            Temple::Base::vec3_t w {-2, 1, 3};
            Temple::Base::vec3_t result = B * w;
            Temple::Base::vec3_t expected {4, -13, 4};
            REQUIRE(result == expected);
        }
    }
    SECTION("mat3_t rotation")
    {
        SECTION("  Example 1")
        {
            Temple::Base::vec3_t n {1.0f, 2.0f, 3.0f};
            n = n / n.calculate_magnitude();
            Temple::Base::mat3_t mrot = Temple::Base::mat3_t::calculate_rotation_matrix(n,
                                                                                        1.0f); // angle in radians
            Temple::Base::mat3_t expected =
                Temple::Base::mat3_t {0.5731379f,  -0.6090066f, 0.5482918f, 0.7403488f, 0.6716445f,
                                      -0.0278793f, -0.3512785f, 0.4219059f, 0.8358222f};
            REQUIRE(mrot == expected);
        }

        SECTION("  Example 2")
        {
            Temple::Base::vec3_t n {-0.5f, 5.0f, 0.31f};
            n = n / n.calculate_magnitude(); // should be unit-vector
            Temple::Base::mat3_t mrot = Temple::Base::mat3_t::calculate_rotation_matrix(n,
                                                                                        1.0f); // angle in radians
            Temple::Base::mat3_t expected =
                Temple::Base::mat3_t {0.5448365f, -0.0971558f, 0.8328949f,  0.0064717f, 0.9937229f,
                                      0.1116827f, -0.8385173f, -0.0554585f, 0.5420452f};
            REQUIRE(mrot == expected);
        }

        SECTION("  Example 3")
        {
            Temple::Base::vec3_t i {1.0f, 0.0f, 0.0f};
            Temple::Base::vec3_t j {0.0f, 1.0f, 0.0f};
            Temple::Base::vec3_t k {0.0f, 0.0f, 1.0f};
            float                angle = 3.14 * 0.5f;
            Temple::Base::mat3_t mi = Temple::Base::mat3_t::calculate_rotation_matrix(i, angle);
            Temple::Base::mat3_t mj = Temple::Base::mat3_t::calculate_rotation_matrix(j, angle);
            Temple::Base::mat3_t mk = Temple::Base::mat3_t::calculate_rotation_matrix(k, angle);

            Temple::Base::mat3_t mi0 = Temple::Base::mat3_t::calculate_rotation_matrix_around_x(angle);
            Temple::Base::mat3_t mj0 = Temple::Base::mat3_t::calculate_rotation_matrix_around_y(angle);
            Temple::Base::mat3_t mk0 = Temple::Base::mat3_t::calculate_rotation_matrix_around_z(angle);

            REQUIRE((mi == mi0 && mj == mj0 && mk == mk0));
        }
    }
}

TEST_CASE("Temple::Base::mat4_t tests", "[Base::mat4_t]")
{
    Temple::Base::mat4_t default_matrix {1.0f, 2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f,
                                         9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f};
    Temple::Base::mat4_t zero_matrix = {};
    SECTION("mat4_t default constructor - all values should be zero")
    {
        Temple::Base::mat4_t M = {};
        REQUIRE(
            (EQUAL_FLOATS(0.0f, M.r0.calculate_magnitude()) && EQUAL_FLOATS(0.0f, M.r1.calculate_magnitude()) &&
             EQUAL_FLOATS(0.0f, M.r2.calculate_magnitude()) && EQUAL_FLOATS(0.0f, M.r3.calculate_magnitude())));
    }
    SECTION("mat4_t constructor with 16 floats and equality operator")
    {
        Temple::Base::mat4_t M {1.0f, 2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f,
                                9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f};
        REQUIRE(
            (EQUAL_FLOATS(1.0f, M(0, 0)) && EQUAL_FLOATS(2.0f, M(0, 1)) && EQUAL_FLOATS(3.0f, M(0, 2)) &&
             EQUAL_FLOATS(4.0f, M(0, 3)) && EQUAL_FLOATS(5.0f, M(1, 0)) && EQUAL_FLOATS(6.0f, M(1, 1)) &&
             EQUAL_FLOATS(7.0f, M(1, 2)) && EQUAL_FLOATS(8.0f, M(1, 3)) && EQUAL_FLOATS(9.0f, M(2, 0)) &&
             EQUAL_FLOATS(10.0f, M(2, 1)) && EQUAL_FLOATS(11.0f, M(2, 2)) && EQUAL_FLOATS(12.0f, M(2, 3)) &&
             EQUAL_FLOATS(13.0f, M(3, 0)) && EQUAL_FLOATS(14.0f, M(3, 1)) && EQUAL_FLOATS(15.0f, M(3, 2)) &&
             EQUAL_FLOATS(16.0f, M(3, 3)) && M == default_matrix));
    }
    SECTION("mat4_t constructor with 4 vec4_t (rows)")
    {
        Temple::Base::vec4_t a {1.0f, 2.0f, 3.0f, 4.0f};
        Temple::Base::vec4_t b {5.0f, 6.0f, 7.0f, 8.0f};
        Temple::Base::vec4_t c {9.0f, 10.0f, 11.0f, 12.0f};
        Temple::Base::vec4_t d {13.0f, 14.0f, 15.0f, 16.0f};
        Temple::Base::mat4_t M {a.x, a.y, a.z, a.w, b.x, b.y, b.z, b.w, c.x, c.y, c.z, c.w, d.x, d.y, d.z, d.w};
        REQUIRE(M == default_matrix);
    }
    SECTION("mat4_t constructor with another mat4_t&")
    {
        Temple::Base::mat4_t N {1.0f, 2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f,
                                9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f};
        Temple::Base::mat4_t M = N;
        REQUIRE((M == default_matrix && M == N));
    }
    SECTION("mat4_t inequality")
    {
        Temple::Base::mat4_t M = default_matrix;
        M(0, 0) += 0.001f;
        REQUIRE(M != default_matrix);
    }
    SECTION("Simple mat4_t arythmetic operations")
    {
        Temple::Base::mat4_t M(default_matrix);
        Temple::Base::mat4_t N = 2.0f * M;
        Temple::Base::mat4_t M2 {2.0f,  4.0f,  6.0f,  8.0f,  10.0f, 12.0f, 14.0f, 16.0f,
                                 18.0f, 20.0f, 22.0f, 24.0f, 26.0f, 28.0f, 30.0f, 32.0f};
        REQUIRE(N == M2);
        M *= 2.0f;
        REQUIRE(M == M2);
        N = M / 2.0f;
        REQUIRE(N == default_matrix);
        M /= 2.0f;
        REQUIRE(M == default_matrix);
        M -= default_matrix;
        REQUIRE(M == zero_matrix);
        M += default_matrix;
        REQUIRE(M == default_matrix);
        M = 2.0f * M - N;
        REQUIRE(M == default_matrix);
        N *= 0.5f;
        M = M + 3.0f * M - N / 0.25f;
        REQUIRE(M == 4.0f * N);
        M = -(-M);
        REQUIRE(M == 4.0f * N);
    }
    SECTION("mat4_t multiplications for vec4_t and another mat4_t")
    {
        SECTION("  Example 1")
        {
            Temple::Base::mat4_t A {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

            Temple::Base::mat4_t B {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

            Temple::Base::mat4_t result = A * B;

            Temple::Base::mat4_t expected {80, 70, 60, 50, 240, 214, 188, 162, 400, 358, 316, 274, 560, 502, 444, 386};

            REQUIRE(result == expected);
        }

        SECTION("  Example 2")
        {
            Temple::Base::mat4_t C {-1, 2, 3, 4, 2, -4, 1, 3, 3, 2, -2, 2, 8, -2, 10, 6};

            Temple::Base::mat4_t D {3, -2, 1, 7, 1, 4, -3, -3, -1, 3, 5, 5, -5, 5, 9, -2};

            Temple::Base::mat4_t result = C * D;

            Temple::Base::mat4_t expected {-24, 39, 44, -6, -14, -2, 46, 25, 3, 6, 5, 1, -18, 36, 118, 100};

            REQUIRE(result == expected);
        }

        SECTION("  Example 3")
        {
            Temple::Base::mat4_t A {8.7f, 2.57f, 9.02f, 3.27f, 10.23f, 2.95f, 3.96f, 6.98f,
                                    9.5f, 4.8f,  2.4f,  7.09f, 1.29f,  1.79f, 6.11f, 5.74f};
            Temple::Base::vec4_t v {4.7f, 10.81f, 8.3f, 9.42f};
            Temple::Base::vec4_t result = A * v;
            Temple::Base::vec4_t expected {174.3411f, 178.5901f, 183.2458f, 130.1967f};
            REQUIRE(result == expected);
        }

        SECTION("  Example 4")
        {
            Temple::Base::mat4_t B {8.7f, -2.57f, 9.02f, -3.27f, 10.23f, -2.95f, 3.96f, 6.98f,
                                    9.5f, 4.8f,   -2.4f, 7.09f,  1.29f,  -1.79f, 6.11f, 5.74f};
            Temple::Base::vec4_t w {4.7f, 10.81f, 8.3f, 9.42f};
            Temple::Base::vec4_t result = B * w;
            Temple::Base::vec4_t expected {57.1709f, 114.8111f, 143.4058f, 91.4969f};
            REQUIRE(result == expected);
        }
    }

    SECTION("mat4_t rotation")
    {
        SECTION("  Example 1")
        {
            Temple::Base::vec4_t n {1.0f, 2.0f, 3.0f, 0.0f};
            n = n / n.calculate_magnitude();
            Temple::Base::mat4_t mrot = Temple::Base::mat4_t::calculate_rotation_matrix(n,
                                                                                        1.0f); // angle in radians
            Temple::Base::mat4_t expected = Temple::Base::mat4_t {
                0.5731379f,  -0.6090066f, 0.5482918f, 0.0f, 0.7403488f, 0.6716445f, -0.0278793f, 0.0f,
                -0.3512785f, 0.4219059f,  0.8358222f, 0.0f, 0.0f,       0.0f,       0.0f,        1.0f};
            REQUIRE(mrot == expected);
        }

        SECTION("  Example 2")
        {
            Temple::Base::vec4_t n {-0.5f, 5.0f, 0.31f, 0.0f};
            n = n / n.calculate_magnitude(); // should be unit-vector
            Temple::Base::mat4_t mrot = Temple::Base::mat4_t::calculate_rotation_matrix(n,
                                                                                        1.0f); // angle in radians
            Temple::Base::mat4_t expected = Temple::Base::mat4_t {
                0.5448365f,  -0.0971558f, 0.8328949f, 0.0f, 0.0064717f, 0.9937229f, 0.1116827f, 0.0f,
                -0.8385173f, -0.0554585f, 0.5420452f, 0.0f, 0.0f,       0.0f,       0.0f,       1.0f};
            REQUIRE(mrot == expected);
        }

        SECTION("  Example 3")
        {
            Temple::Base::vec4_t i {1.0f, 0.0f, 0.0f, 0.0f};
            Temple::Base::vec4_t j {0.0f, 1.0f, 0.0f, 0.0f};
            Temple::Base::vec4_t k {0.0f, 0.0f, 1.0f, 0.0f};
            float                angle = 3.14 * 0.5f;
            Temple::Base::mat4_t mi = Temple::Base::mat4_t::calculate_rotation_matrix(i, angle);
            Temple::Base::mat4_t mj = Temple::Base::mat4_t::calculate_rotation_matrix(j, angle);
            Temple::Base::mat4_t mk = Temple::Base::mat4_t::calculate_rotation_matrix(k, angle);

            Temple::Base::mat4_t mi0 = Temple::Base::mat4_t::calculate_rotation_matrix_around_x(angle);
            Temple::Base::mat4_t mj0 = Temple::Base::mat4_t::calculate_rotation_matrix_around_y(angle);
            Temple::Base::mat4_t mk0 = Temple::Base::mat4_t::calculate_rotation_matrix_around_z(angle);

            REQUIRE((mi == mi0 && mj == mj0 && mk == mk0));
        }
    }
    SECTION("mat4_t projection")
    {
        SECTION("  Example 1")
        {
            float                z_near = 10.0f;
            float                z_far = 100.0f;
            Temple::Base::mat4_t proj = Temple::Base::mat4_t::calculate_projection_matrix(
                90.0 * Temple::Base::pi / 180.0f, 1.0f, z_near, z_far);
            Temple::Base::vec4_t v_near {0.0f, 0.0f, z_near, 1.0f};
            Temple::Base::vec4_t v_far {0.0f, 0.0f, z_far, 1.0f};
            Temple::Base::vec4_t v_near_projected = proj * v_near;
            Temple::Base::vec4_t v_far_projected = proj * v_far;
            REQUIRE(
                (EQUAL_FLOATS(v_near_projected.z / v_near_projected.w, 1.0f) && EQUAL_FLOATS(v_far_projected.z, 0.0f)));
        }
    }
}
