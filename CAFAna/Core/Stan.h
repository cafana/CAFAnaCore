/// Stan.h -- include Stan headers.
///  (They trigger some warnings that we want to suppress,
///   so centralize all the stuff turning them off into this one spot.)

#pragma once

#ifdef EIGEN_MAJOR_VERSION
#ifndef STAN_MATH_PRIM_FUN_EIGEN_HPP
#error "It is strongly advised not to include Eigen headers before stan_math headers, please rearrange your includes"
#endif
#endif

// Supress a deprecation warning from tbb/task_scheduler_init.h that otherwise
// gets triggered a lot. Hopefully stan updates before tbb does.
#define __TBB_show_deprecation_message_task_scheduler_init_H

// Stan's dependencies trigger some warnings...
#pragma GCC diagnostic push
#if __GNUC__ >= 6
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#endif
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#include "stan/math/rev.hpp"
#pragma GCC diagnostic pop

