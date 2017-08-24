// Copyright Takatoshi Kondo 2017
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#if !defined(MQTT_TEST_SERVER_TLS_HPP)
#define MQTT_TEST_SERVER_TLS_HPP

#include <iostream>
#include <set>

#include <boost/lexical_cast.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/identity.hpp>

#include <mqtt_server_cpp.hpp>
#include "test_settings.hpp"

namespace mi = boost::multi_index;
namespace as = boost::asio;

struct ctx_init {
    ctx_init() : ctx(boost::asio::ssl::context::tlsv12) {
        ctx.set_options(
            boost::asio::ssl::context::default_workarounds |
            boost::asio::ssl::context::single_dh_use);
        std::string path = boost::unit_test::framework::master_test_suite().argv[0];
        std::size_t pos = path.find_last_of("/\\");
        std::string base = pos == std::string::npos ? "" : path.substr(0, pos + 1);
        std::cout << "base: " << base << std::endl;
        ctx.use_certificate_file(base + "server.crt.pem", boost::asio::ssl::context::pem);
        ctx.use_private_key_file(base + "server.key.pem", boost::asio::ssl::context::pem);
    }
    boost::asio::ssl::context ctx;
};

class test_server_tls : ctx_init {
public:
    test_server_tls(as::io_service& ios, test_broker& b)
        : server_(as::ip::tcp::endpoint(as::ip::tcp::v4(), broker_tls_port), std::move(ctx), ios), b_(b) {
        server_.set_error_handler(
            [](boost::system::error_code const& /*ec*/) {
            }
        );

        server_.set_accept_handler(
            [&](mqtt::server_tls<>::endpoint_t& ep) {
                b_.handle_accept(ep);
            }
        );

        server_.listen();
    }

    void close() {
        server_.close();
    }

private:
    mqtt::server_tls<> server_;
    test_broker& b_;
};

#endif // MQTT_TEST_SERVER_TLS_HPP
