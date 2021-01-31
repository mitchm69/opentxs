// Copyright (c) 2010-2020 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "Helpers.hpp"  // IWYU pragma: associated

#include <gtest/gtest.h>

#include "opentxs/Proto.tpp"
#include "opentxs/blockchain/block/bitcoin/Script.hpp"  // IWYU pragma: keep
#include "opentxs/blockchain/client/HeaderOracle.hpp"
#include "opentxs/protobuf/BlockchainP2PHello.pb.h"
#include "opentxs/protobuf/Check.hpp"
#include "opentxs/protobuf/verify/BlockchainP2PHello.hpp"

namespace
{
TEST_F(Regtest_fixture_sync, init_opentxs) {}

TEST_F(Regtest_fixture_sync, start_chains) { EXPECT_TRUE(Start()); }

TEST_F(Regtest_fixture_sync, connect_peers) { EXPECT_TRUE(Connect()); }

TEST_F(Regtest_fixture_sync, sync_genesis)
{
    sync_req_.expected_ += 2;
    const auto& chain = client_.Blockchain().GetChain(test_chain_);
    const auto pos = chain.HeaderOracle().BestChain();

    EXPECT_TRUE(sync_req_.request(pos));
    ASSERT_TRUE(sync_req_.wait());

    {
        const auto& msg = sync_req_.get(++sync_req_.checked_);
        const auto body = msg.Body();

        ASSERT_EQ(body.size(), 2);

        const auto hello = ot::proto::Factory<Hello>(body.at(0));

        EXPECT_TRUE(ot::proto::Validate(hello, ot::VERBOSE));
        ASSERT_EQ(hello.state_size(), 1);
        EXPECT_TRUE(sync_req_.check(hello.state(0), pos));
        EXPECT_EQ(std::string{body.at(1).Bytes()}, sync_server_update_public_);
    }

    {
        const auto& msg = sync_req_.get(++sync_req_.checked_);
        const auto body = msg.Body();

        ASSERT_EQ(body.size(), 2);

        const auto hello = ot::proto::Factory<Hello>(body.at(0));

        EXPECT_TRUE(ot::proto::Validate(hello, ot::VERBOSE));
        ASSERT_EQ(hello.state_size(), 1);
        EXPECT_TRUE(sync_req_.check(hello.state(0), pos));
        EXPECT_EQ(body.at(1).size(), 0);
    }
}

TEST_F(Regtest_fixture_sync, mine)
{
    constexpr auto count{10};
    sync_sub_.expected_ += count;

    EXPECT_TRUE(Mine(0, count));
    EXPECT_TRUE(sync_sub_.wait());

    const auto& chain = client_.Blockchain().GetChain(test_chain_);
    const auto best = chain.HeaderOracle().BestChain();

    EXPECT_EQ(best.first, 10);
    EXPECT_EQ(best.second, mined_blocks_.get(9).get());
}

TEST_F(Regtest_fixture_sync, sync_full)
{
    sync_req_.expected_ += 2;
    const auto& chain = client_.Blockchain().GetChain(test_chain_);
    const auto genesis = Position{0, chain.HeaderOracle().BestHash(0)};

    EXPECT_TRUE(sync_req_.request(genesis));
    ASSERT_TRUE(sync_req_.wait());

    {
        const auto& msg = sync_req_.get(++sync_req_.checked_);
        const auto body = msg.Body();

        ASSERT_EQ(body.size(), 2);

        const auto hello = ot::proto::Factory<Hello>(body.at(0));

        EXPECT_TRUE(ot::proto::Validate(hello, ot::VERBOSE));
        ASSERT_EQ(hello.state_size(), 1);
        EXPECT_TRUE(sync_req_.check(hello.state(0), 9));
        EXPECT_EQ(std::string{body.at(1).Bytes()}, sync_server_update_public_);
    }

    {
        const auto& msg = sync_req_.get(++sync_req_.checked_);
        const auto body = msg.Body();

        ASSERT_GE(body.size(), 2);

        const auto hello = ot::proto::Factory<Hello>(body.at(0));

        EXPECT_TRUE(ot::proto::Validate(hello, ot::VERBOSE));
        ASSERT_EQ(hello.state_size(), 1);
        EXPECT_TRUE(sync_req_.check(hello.state(0), 9));
        EXPECT_EQ(body.at(1).size(), 0);
        ASSERT_EQ(body.size(), 12);
        EXPECT_TRUE(sync_req_.check(body.at(2), 0));
        EXPECT_TRUE(sync_req_.check(body.at(3), 1));
        EXPECT_TRUE(sync_req_.check(body.at(4), 2));
        EXPECT_TRUE(sync_req_.check(body.at(5), 3));
        EXPECT_TRUE(sync_req_.check(body.at(6), 4));
        EXPECT_TRUE(sync_req_.check(body.at(7), 5));
        EXPECT_TRUE(sync_req_.check(body.at(8), 6));
        EXPECT_TRUE(sync_req_.check(body.at(9), 7));
        EXPECT_TRUE(sync_req_.check(body.at(10), 8));
        EXPECT_TRUE(sync_req_.check(body.at(11), 9));
    }
}

TEST_F(Regtest_fixture_sync, sync_partial)
{
    sync_req_.expected_ += 2;
    const auto start = Position{6, mined_blocks_.get(6).get()};

    EXPECT_TRUE(sync_req_.request(start));
    ASSERT_TRUE(sync_req_.wait());

    {
        const auto& msg = sync_req_.get(++sync_req_.checked_);
        const auto body = msg.Body();

        ASSERT_EQ(body.size(), 2);

        const auto hello = ot::proto::Factory<Hello>(body.at(0));

        EXPECT_TRUE(ot::proto::Validate(hello, ot::VERBOSE));
        ASSERT_EQ(hello.state_size(), 1);
        EXPECT_TRUE(sync_req_.check(hello.state(0), 9));
        EXPECT_EQ(std::string{body.at(1).Bytes()}, sync_server_update_public_);
    }

    {
        const auto& msg = sync_req_.get(++sync_req_.checked_);
        const auto body = msg.Body();

        ASSERT_GE(body.size(), 2);

        const auto hello = ot::proto::Factory<Hello>(body.at(0));

        EXPECT_TRUE(ot::proto::Validate(hello, ot::VERBOSE));
        ASSERT_EQ(hello.state_size(), 1);
        EXPECT_TRUE(sync_req_.check(hello.state(0), 9));
        EXPECT_EQ(body.at(1).size(), 0);
        ASSERT_EQ(body.size(), 6);
        EXPECT_TRUE(sync_req_.check(body.at(2), 6));
        EXPECT_TRUE(sync_req_.check(body.at(3), 7));
        EXPECT_TRUE(sync_req_.check(body.at(4), 8));
        EXPECT_TRUE(sync_req_.check(body.at(5), 9));
    }
}

TEST_F(Regtest_fixture_sync, reorg)
{
    constexpr auto count{4};
    sync_sub_.expected_ += count;

    EXPECT_TRUE(Mine(8, count));
    EXPECT_TRUE(sync_sub_.wait());

    const auto& chain = client_.Blockchain().GetChain(test_chain_);
    const auto best = chain.HeaderOracle().BestChain();

    EXPECT_EQ(best.first, 12);
    EXPECT_EQ(best.second, mined_blocks_.get(13).get());
}

TEST_F(Regtest_fixture_sync, sync_reorg)
{
    sync_req_.expected_ += 2;
    const auto start = Position{10, mined_blocks_.get(9).get()};

    EXPECT_TRUE(sync_req_.request(start));
    ASSERT_TRUE(sync_req_.wait());

    {
        const auto& msg = sync_req_.get(++sync_req_.checked_);
        const auto body = msg.Body();

        ASSERT_EQ(body.size(), 2);

        const auto hello = ot::proto::Factory<Hello>(body.at(0));

        EXPECT_TRUE(ot::proto::Validate(hello, ot::VERBOSE));
        ASSERT_EQ(hello.state_size(), 1);
        EXPECT_TRUE(sync_req_.check(hello.state(0), 13));
        EXPECT_EQ(std::string{body.at(1).Bytes()}, sync_server_update_public_);
    }

    {
        const auto& msg = sync_req_.get(++sync_req_.checked_);
        const auto body = msg.Body();

        ASSERT_GE(body.size(), 2);

        const auto hello = ot::proto::Factory<Hello>(body.at(0));

        EXPECT_TRUE(ot::proto::Validate(hello, ot::VERBOSE));
        ASSERT_EQ(hello.state_size(), 1);
        EXPECT_TRUE(sync_req_.check(hello.state(0), 13));
        EXPECT_EQ(body.at(1).size(), 0);
        ASSERT_EQ(body.size(), 6);
        EXPECT_TRUE(sync_req_.check(body.at(2), 10));
        EXPECT_TRUE(sync_req_.check(body.at(3), 11));
        EXPECT_TRUE(sync_req_.check(body.at(4), 12));
        EXPECT_TRUE(sync_req_.check(body.at(5), 13));
    }
}

TEST_F(Regtest_fixture_sync, shutdown) { Shutdown(); }
}  // namespace
