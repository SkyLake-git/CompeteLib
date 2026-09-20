#pragma once
#include <gmp.h>
#include <iostream>

namespace initializer {
#ifdef ONLINE_JUDGE
    static bool _synced_stdio = std::ios_base::sync_with_stdio(false);
    static bool _untied_cin = std::cin.tie(nullptr);
#endif

    static bool _gmp_precision = []() {
        mpf_set_default_prec(256);
        return true;
    }();
}
