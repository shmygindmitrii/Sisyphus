#include "thirdparty_catch_amalgamated.hpp"
#include "base_matrices.h"
#include "base_utils.h"

TEST_CASE("Temple::Base::mat3 tests", "[Base::mat3]") {
    Temple::Base::mat3 defaultMatrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
    Temple::Base::mat3 zeroMatrix;
    SECTION("mat3 default constructor - all values should be zero") {
        Temple::Base::mat3 M;
        REQUIRE((EQUAL_FLOATS(0.0f, M.c0.magnitude()) && EQUAL_FLOATS(0.0f, M.c1.magnitude()) && EQUAL_FLOATS(0.0f, M.c2.magnitude())));
    }
    SECTION("mat3 constructor with 9 floats and equality operator") {
        Temple::Base::mat3 M(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
        REQUIRE((EQUAL_FLOATS(1.0f, M(0, 0)) && EQUAL_FLOATS(2.0f, M(0, 1)) && EQUAL_FLOATS(3.0f, M(0, 2)) && 
                 EQUAL_FLOATS(4.0f, M(1, 0)) && EQUAL_FLOATS(5.0f, M(1, 1)) && EQUAL_FLOATS(6.0f, M(1, 2)) &&
                 EQUAL_FLOATS(7.0f, M(2, 0)) && EQUAL_FLOATS(8.0f, M(2, 1)) && EQUAL_FLOATS(9.0f, M(2, 2)) && 
                 M == defaultMatrix));
    }
    SECTION("mat3 constructor with 3 vec3 (columns)") {
        Temple::Base::vec3 a(1.0f, 4.0f, 7.0f);
        Temple::Base::vec3 b(2.0f, 5.0f, 8.0f);
        Temple::Base::vec3 c(3.0f, 6.0f, 9.0f);
        Temple::Base::mat3 M(a, b, c);
        REQUIRE(M == defaultMatrix);
    }
    SECTION("mat3 constructor with another mat3&") {
        Temple::Base::mat3 N(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
        Temple::Base::mat3 M(N);
        REQUIRE((M == defaultMatrix && M == N));
    }
    SECTION("mat3 inequality") {
        Temple::Base::mat3 M(defaultMatrix);
        M(0, 0) += 0.001f;
        REQUIRE(M != defaultMatrix);
    }
    SECTION("Simple mat3 arythmetic operations") {
        Temple::Base::mat3 M(defaultMatrix);
        Temple::Base::mat3 N = 2.0f * M;
        Temple::Base::mat3 M2(2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 18.0f);
        REQUIRE(N == M2);
        M *= 2.0f;
        REQUIRE(M == M2);
        N = M / 2.0f;
        REQUIRE(N == defaultMatrix);
        M /= 2.0f;
        REQUIRE(M == defaultMatrix);
        M -= defaultMatrix;
        REQUIRE(M == zeroMatrix);
        M += defaultMatrix;
        REQUIRE(M == defaultMatrix);
        M = 2.0f * M - N;
        REQUIRE(M == defaultMatrix);
        N *= 0.5f;
        M = M + 3.0f * M - N / 0.25f;
        REQUIRE(M == 4.0f * N);
        M = -(-M);
        REQUIRE(M == 4.0f * N);
    }
    SECTION("mat3 multiplications for vec3 and another mat3") {
        SECTION("  Example 1") {
            Temple::Base::mat3 A(1, 2, 3,
                                      4, 5, 6,
                                      7, 8, 9);

            Temple::Base::mat3 B(9, 8, 7,
                                      6, 5, 4,
                                      3, 2, 1);

            Temple::Base::mat3 result = A * B;

            Temple::Base::mat3 expected(  30,  24, 18,
                                               84,  69, 54,
                                              138, 114, 90);

            REQUIRE(result == expected);
        }

        SECTION("  Example 2") {
            Temple::Base::mat3 C(-1,  0,  3,
                                       2, -4,  1,
                                       3,  2, -2);

            Temple::Base::mat3 D( 3, -2,  1,
                                       1,  4, -3,
                                      -1,  0,  5);

            Temple::Base::mat3 result = C * D;

            Temple::Base::mat3 expected( -6,   2,  14,
                                               1, -20,  19,
                                              13,   2, -13);

            REQUIRE(result == expected);
        }

        SECTION("  Example 3") {
            Temple::Base::mat3 A(2, -1, 3, 0, 1, -2, -1, 2, 1);
            Temple::Base::vec3 v(3, -1, 2);
            Temple::Base::vec3 result = A * v;
            Temple::Base::vec3 expected(13, -5, -3);
            REQUIRE(result == expected);

        }

        SECTION("  Example 4") {
            Temple::Base::mat3 B(-1, 2, 0, 3, -1, -2, 1, 0, 2);
            Temple::Base::vec3 w(-2, 1, 3);
            Temple::Base::vec3 result = B * w;
            Temple::Base::vec3 expected(4, -13, 4);
            REQUIRE(result == expected);
        }
    }
    SECTION("mat3 rotation") {
        SECTION("  Example 1") {
            Temple::Base::vec3 n(1.0f, 2.0f, 3.0f);
            n = n / n.magnitude();
            Temple::Base::mat3 mrot = Temple::Base::mat3::rot(n, 1.0f); // angle in radians
            Temple::Base::mat3 expected = Temple::Base::mat3( 0.5731379f, -0.6090066f,  0.5482918f, 
                                                              0.7403488f,  0.6716445f, -0.0278793f, 
                                                             -0.3512785f,  0.4219059f,  0.8358222f );
            REQUIRE(mrot == expected);
        }

        SECTION("  Example 2") {
            Temple::Base::vec3 n(-0.5f, 5.0f, 0.31f);
            n = n / n.magnitude(); // should be unit-vector
            Temple::Base::mat3 mrot = Temple::Base::mat3::rot(n, 1.0f); // angle in radians
            Temple::Base::mat3 expected = Temple::Base::mat3( 0.5448365f, -0.0971558f, 0.8328949f,
                                                              0.0064717f,  0.9937229f, 0.1116827f,
                                                             -0.8385173f, -0.0554585f, 0.5420452f );
            REQUIRE(mrot == expected);
        }
    }
}

TEST_CASE("Temple::Base::mat4 tests", "[Base::mat4]") {
    Temple::Base::mat4 defaultMatrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
    Temple::Base::mat4 zeroMatrix;
    SECTION("mat4 default constructor - all values should be zero") {
        Temple::Base::mat4 M;
        REQUIRE((EQUAL_FLOATS(0.0f, M.c0.magnitude()) && EQUAL_FLOATS(0.0f, M.c1.magnitude()) && EQUAL_FLOATS(0.0f, M.c2.magnitude()) && EQUAL_FLOATS(0.0f, M.c3.magnitude())));
    }
    SECTION("mat4 constructor with 16 floats and equality operator") {
        Temple::Base::mat4 M(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
        REQUIRE((EQUAL_FLOATS(1.0f, M(0, 0))  && EQUAL_FLOATS(2.0f, M(0, 1))  && EQUAL_FLOATS(3.0f, M(0, 2))  && EQUAL_FLOATS(4.0f, M(0, 3)) && 
                 EQUAL_FLOATS(5.0f, M(1, 0))  && EQUAL_FLOATS(6.0f, M(1, 1))  && EQUAL_FLOATS(7.0f, M(1, 2))  && EQUAL_FLOATS(8.0f, M(1, 3)) &&
                 EQUAL_FLOATS(9.0f, M(2, 0))  && EQUAL_FLOATS(10.0f, M(2, 1)) && EQUAL_FLOATS(11.0f, M(2, 2)) && EQUAL_FLOATS(12.0f, M(2, 3)) &&
                 EQUAL_FLOATS(13.0f, M(3, 0)) && EQUAL_FLOATS(14.0f, M(3, 1)) && EQUAL_FLOATS(15.0f, M(3, 2)) && EQUAL_FLOATS(16.0f, M(3, 3)) &&
                 M == defaultMatrix));
    }
    SECTION("mat4 constructor with 4 vec4 (columns)") {
        Temple::Base::vec4 a(1.0f, 5.0f, 9.0f, 13.0f);
        Temple::Base::vec4 b(2.0f, 6.0f, 10.0f, 14.0f);
        Temple::Base::vec4 c(3.0f, 7.0f, 11.0f, 15.0f);
        Temple::Base::vec4 d(4.0f, 8.0f, 12.0f, 16.0f);
        Temple::Base::mat4 M(a, b, c, d);
        REQUIRE(M == defaultMatrix);
    }
    SECTION("mat4 constructor with another mat4&") {
        Temple::Base::mat4 N(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
        Temple::Base::mat4 M(N);
        REQUIRE((M == defaultMatrix && M == N));
    }
    SECTION("mat4 inequality") {
        Temple::Base::mat4 M(defaultMatrix);
        M(0, 0) += 0.001f;
        REQUIRE(M != defaultMatrix);
    }
    SECTION("Simple mat4 arythmetic operations") {
        Temple::Base::mat4 M(defaultMatrix);
        Temple::Base::mat4 N = 2.0f * M;
        Temple::Base::mat4 M2(2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 18.0f, 20.0f, 22.0f, 24.0f, 26.0f, 28.0f, 30.0f, 32.0f);
        REQUIRE(N == M2);
        M *= 2.0f;
        REQUIRE(M == M2);
        N = M / 2.0f;
        REQUIRE(N == defaultMatrix);
        M /= 2.0f;
        REQUIRE(M == defaultMatrix);
        M -= defaultMatrix;
        REQUIRE(M == zeroMatrix);
        M += defaultMatrix;
        REQUIRE(M == defaultMatrix);
        M = 2.0f * M - N;
        REQUIRE(M == defaultMatrix);
        N *= 0.5f;
        M = M + 3.0f * M - N / 0.25f;
        REQUIRE(M == 4.0f * N);
        M = -(-M);
        REQUIRE(M == 4.0f * N);
    }
    SECTION("mat4 multiplications for vec4 and another mat4") {
        SECTION("  Example 1") {
            Temple::Base::mat4 A(1, 2, 3, 4,
                                 5, 6, 7, 8,
                                 9, 10, 11, 12, 
                                 13, 14, 15, 16);

            Temple::Base::mat4 B(16, 15, 14, 13,
                                 12, 11, 10, 9,
                                 8, 7, 6, 5,
                                 4, 3, 2, 1);

            Temple::Base::mat4 result = A * B;

            Temple::Base::mat4 expected(80, 70, 60, 50,
                                        240, 214, 188, 162,
                                        400, 358, 316, 274,
                                        560, 502, 444, 386);

            REQUIRE(result == expected);
        }

        SECTION("  Example 2") {
            Temple::Base::mat4 C(-1,  2,  3,  4,
                                  2, -4,  1,  3,
                                  3,  2, -2,  2, 
                                  8, -2,  10, 6);

            Temple::Base::mat4 D(3, -2,  1,  7,
                                 1,  4, -3, -3,
                                -1,  3,  5,  5, 
                                -5,  5,  9, -2);

            Temple::Base::mat4 result = C * D;

            Temple::Base::mat4 expected(-24,  39, 44, -6,
                                        -14, -2,  46,  25,
                                          3,  6,  5,   1,
                                        -18,  36, 118, 100);

            REQUIRE(result == expected);
        }

        SECTION("  Example 3") {
            Temple::Base::mat4 A(8.7f, 2.57f, 9.02f, 3.27f, 10.23f, 2.95f, 3.96f, 6.98f, 9.5f, 4.8f, 2.4f, 7.09f, 1.29f, 1.79f, 6.11f, 5.74f);
            Temple::Base::vec4 v(4.7f, 10.81f, 8.3f, 9.42f);
            Temple::Base::vec4 result = A * v;
            Temple::Base::vec4 expected(174.3411f, 178.5901f, 183.2458f, 130.1967f);
            REQUIRE(result == expected);

        }

        SECTION("  Example 4") {
            Temple::Base::mat4 B(8.7f, -2.57f, 9.02f, -3.27f, 10.23f, -2.95f, 3.96f, 6.98f, 9.5f, 4.8f, -2.4f, 7.09f, 1.29f, -1.79f, 6.11f, 5.74f);
            Temple::Base::vec4 w(4.7f, 10.81f, 8.3f, 9.42f);
            Temple::Base::vec4 result = B * w;
            Temple::Base::vec4 expected(57.1709f, 114.8111f, 143.4058f, 91.4969f);
            REQUIRE(result == expected);
        }
    }

    SECTION("mat4 rotation") {
        SECTION("  Example 1") {
            Temple::Base::vec4 n(1.0f, 2.0f, 3.0f, 0.0f);
            n = n / n.magnitude();
            Temple::Base::mat4 mrot = Temple::Base::mat4::rot(n, 1.0f); // angle in radians
            Temple::Base::mat4 expected = Temple::Base::mat4( 0.5731379f, -0.6090066f,  0.5482918f, 0.0f,
                                                              0.7403488f,  0.6716445f, -0.0278793f, 0.0f,
                                                             -0.3512785f,  0.4219059f,  0.8358222f, 0.0f,
                                                              0.0f,        0.0f,        0.0f,       1.0f);
            REQUIRE(mrot == expected);
        }

        SECTION("  Example 2") {
            Temple::Base::vec4 n(-0.5f, 5.0f, 0.31f, 0.0f);
            n = n / n.magnitude(); // should be unit-vector
            Temple::Base::mat4 mrot = Temple::Base::mat4::rot(n, 1.0f); // angle in radians
            Temple::Base::mat4 expected = Temple::Base::mat4( 0.5448365f, -0.0971558f, 0.8328949f, 0.0f,
                                                              0.0064717f,  0.9937229f, 0.1116827f, 0.0f,
                                                             -0.8385173f, -0.0554585f, 0.5420452f, 0.0f,
                                                              0.0f,        0.0f,       0.0f,       1.0f );
            REQUIRE(mrot == expected);
        }
    }
}

