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
}
