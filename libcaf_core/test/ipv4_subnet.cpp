/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright 2011-2018 Dominik Charousset                                     *
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

#define CAF_SUITE ipv4_subnet
#include "caf/test/dsl.hpp"

#include "caf/ipv4_subnet.hpp"

using namespace caf;

namespace {

ipv4_address addr(uint8_t oct1, uint8_t oct2, uint8_t oct3, uint8_t oct4) {
  return ipv4_address({oct1, oct2, oct3, oct4});
}

ipv4_subnet operator/(ipv4_address addr, uint8_t prefix) {
  return {addr, prefix};
}

} // namespace <anonymous>

CAF_TEST(constructing) {
  ipv4_subnet zero{addr(0, 0, 0, 0), 32};
  CAF_CHECK_EQUAL(zero.network_address(), addr(0, 0, 0, 0));
  CAF_CHECK_EQUAL(zero.prefix_length(), 32u);
  ipv4_subnet local{addr(127, 0, 0, 0), 8};
  CAF_CHECK_EQUAL(local.network_address(), addr(127, 0, 0, 0));
  CAF_CHECK_EQUAL(local.prefix_length(), 8u);
}

CAF_TEST(constains) {
  ipv4_subnet local{addr(127, 0, 0, 0), 8};
  CAF_CHECK(local.contains(addr(127, 0, 0, 1)));
  CAF_CHECK(local.contains(addr(127, 1, 2, 3)));
  CAF_CHECK(local.contains(addr(127, 128, 0, 0) / 9));
  CAF_CHECK(local.contains(addr(127, 0, 0, 0) / 8));
  CAF_CHECK(!local.contains(addr(127, 0, 0, 0) / 7));
}

CAF_TEST(ordering) {
  CAF_CHECK_EQUAL(addr(192, 168, 168, 0) / 24, addr(192, 168, 168, 0) / 24);
  CAF_CHECK_NOT_EQUAL(addr(192, 168, 168, 0) / 25, addr(192, 168, 168, 0) / 24);
  CAF_CHECK_LESS(addr(192, 168, 167, 0) / 24, addr(192, 168, 168, 0) / 24);
  CAF_CHECK_LESS(addr(192, 168, 168, 0) / 24, addr(192, 168, 168, 0) / 25);
}
