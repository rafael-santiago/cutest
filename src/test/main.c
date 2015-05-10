#include "../cute.h"

//  INFO(Santiago): nothing to do here but it breaks the compilation
//                  if there is something wrong with the macro definition.
CUTE_TEST_CASE(test_decl)
CUTE_TEST_CASE_END

CUTE_TEST_CASE(test_basic_check_false)
CUTE_TEST_CASE_END

CUTE_TEST_CASE(entry)
    CUTE_RUN_TEST(test_decl);
CUTE_TEST_CASE_END


int main(int argc, char **argv) {
    CUTE_RUN(entry);
    return 0;
}
