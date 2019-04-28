/* 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. 
 *
 */

#define BOOST_TEST_MODULE "TestOptions"

#include "options.h"
#include <vector>
#include <boost/test/unit_test.hpp>

using namespace dockerized_restinio;

BOOST_AUTO_TEST_CASE(test_default_options) {
  Options options;

  BOOST_CHECK_EQUAL(options.address(), "0.0.0.0");
  BOOST_CHECK_EQUAL(options.port(), 8080);
}

BOOST_AUTO_TEST_CASE(test_parsing_options) {
  std::vector<char*> argv;
  argv.push_back((char*) "test");
  argv.push_back((char*) "--port");
  argv.push_back((char*) "8085");
  argv.push_back((char*) "--address");
  argv.push_back((char*) "127.0.0.1");

  Options options;
  options.parse(argv.size(), argv.data());

  BOOST_CHECK_EQUAL(options.address(), "127.0.0.1");
  BOOST_CHECK_EQUAL(options.port(), 8085);
}