#include "matrix.hpp"
#include "matrix.hpp"
#include <sstream>

#include "../simple_test.h"


using Cube = control_02::FixedSizesMatrix<int, 3, 3>;

template <size_t M>
using Column = control_02::FixedSizesMatrix<int, M, 1>;

template <size_t N>
using Row = control_02::FixedSizesMatrix<int, 1, N>;

using Point = control_02::FixedSizesMatrix<int, 1, 1>;

constexpr Cube M({{1, 0, 0}, {0, 1, 0}, {0, 0, 1}});

static_assert(M == Cube({{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}));

static_assert(M.get(0, 0) == 1);
static_assert(M.get(1, 1) == 1);
static_assert(M.get(2, 2) == 1);

static_assert(M.det() == 1);

static_assert(M + Cube(0) == M);
static_assert(M + Cube(1) == Cube({{2, 1, 1}, {1, 2, 1}, {1, 1, 2}}));

static_assert(M * M == M);

constexpr Cube M2({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
static_assert(M * M2 == M2);
static_assert(M2 * M == M2);


constexpr Column<3> C({{1}, {1}, {1}});
constexpr Row<3> R({{1, 1, 1}});

static_assert(R * C == Point({{3}}));
static_assert(C * R == Cube(1));


template <typename M>
std::string to_string(const M& m) {
    std::stringstream ss;
    m.print(ss);
    return ss.str();
}

TEST(FixedSizesMatrix, print_matrices) {
    EXPECT_EQ("1 0 0 \n0 1 0 \n0 0 1 \n", to_string(M));
    EXPECT_EQ("1 \n1 \n1 \n", to_string(C));
    EXPECT_EQ("1 1 1 \n", to_string(R));
}

TESTING_MAIN()
