set(BOARD f6721b_evb_176pin)
include($ENV{UGELIS_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(NONE)

ugelis_include_directories(
    ${UGELIS_BASE}/include/ust/plugin/test
    ${UGELIS_BASE}/include/ust/core
)

target_sources(app PRIVATE src/main.c)
