/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright (C) 2011 - 2016                                                  *
 * Dominik Charousset <dominik.charousset (at) haw-hamburg.de>                *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#include "caf/config.hpp"

#define CAF_SUITE simple_timeout
#include "caf/test/unit_test.hpp"

#include <chrono>
#include <memory>

#include "caf/all.hpp"

using namespace caf;

namespace {

using ms = std::chrono::milliseconds;

using reset_atom = atom_constant<atom("reset")>;
using timer = typed_actor<reacts_to<reset_atom>>;

struct timer_state {
  bool had_reset = false;
};

timer::behavior_type timer_impl(timer::stateful_pointer<timer_state> self) {
  self->delayed_send(self, ms(100), reset_atom::value);
  return {
    [=](reset_atom) {
      CAF_MESSAGE("timer reset");
      self->state.had_reset = true;
    },
    after(ms(600)) >> [=] {
      CAF_MESSAGE("timer expired");
      CAF_REQUIRE(self->state.had_reset);
      self->quit();
    }
  };
}

timer::behavior_type timer_impl2(timer::pointer self) {
  auto had_reset = std::make_shared<bool>(false);
  self->delayed_anon_send(self, ms(100), reset_atom::value);
  return {
    [=](reset_atom) {
      CAF_MESSAGE("timer reset");
      *had_reset = true;
    },
    after(ms(600)) >> [=] {
      CAF_MESSAGE("timer expired");
      CAF_REQUIRE(*had_reset);
      self->quit();
    }
  };
}

struct config : actor_system_config {
  config() {
    scheduler_policy = atom("testing");
  }
};

struct fixture {
  config cfg;
  actor_system system;
  scoped_actor self;
  scheduler::test_coordinator& sched;

  fixture()
      : system(cfg),
        self(system),
        sched(dynamic_cast<scheduler::test_coordinator&>(system.scheduler())) {
    CAF_REQUIRE(sched.jobs.empty());
    CAF_REQUIRE(sched.delayed_messages.empty());
  }

  ~fixture() {
    sched.run_dispatch_loop();
  }
};

} // namespace <anonymous>

CAF_TEST_FIXTURE_SCOPE(simple_timeout_tests, fixture)

CAF_TEST(duration_conversion) {
  duration d1{time_unit::milliseconds, 100};
  std::chrono::milliseconds d2{100};
  duration d3{d2};
  CAF_CHECK_EQUAL(d1.count, d2.count());
  CAF_CHECK_EQUAL(d1, d3);
}

CAF_TEST(single_timeout) {
  system.spawn(timer_impl);
}

CAF_TEST(single_anon_timeout) {
  system.spawn(timer_impl2);
}

CAF_TEST_FIXTURE_SCOPE_END()
