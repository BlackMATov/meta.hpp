#pragma once

#include "doctest.h"

// https://github.com/onqtam/doctest/issues/126
#include <iostream>

#define STATIC_CHECK(...)\
    static_assert(__VA_ARGS__, #__VA_ARGS__);\
    CHECK(__VA_ARGS__)

#define STATIC_CHECK_FALSE(...)\
    static_assert(!(__VA_ARGS__), "!(" #__VA_ARGS__ ")");\
    CHECK(!(__VA_ARGS__))
