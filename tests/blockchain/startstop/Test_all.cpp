// Copyright (c) 2010-2021 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <gtest/gtest.h>
#include <memory>

#include "Helpers.hpp"
#include "opentxs/api/client/Blockchain.hpp"
#include "opentxs/api/client/Manager.hpp"
#include "opentxs/blockchain/BlockchainType.hpp"

namespace
{
TEST_F(Test_StartStop, init_opentxs) {}

TEST_F(Test_StartStop, all)
{
    EXPECT_TRUE(api_.Blockchain().Start(b::Type::Bitcoin, "127.0.0.2"));
    EXPECT_TRUE(
        api_.Blockchain().Start(b::Type::Bitcoin_testnet3, "127.0.0.2"));
    EXPECT_TRUE(api_.Blockchain().Start(b::Type::BitcoinCash, "127.0.0.2"));
    EXPECT_TRUE(
        api_.Blockchain().Start(b::Type::BitcoinCash_testnet3, "127.0.0.2"));
    EXPECT_FALSE(
        api_.Blockchain().Start(b::Type::Ethereum_frontier, "127.0.0.2"));
    EXPECT_FALSE(
        api_.Blockchain().Start(b::Type::Ethereum_ropsten, "127.0.0.2"));
    EXPECT_TRUE(api_.Blockchain().Start(b::Type::Litecoin, "127.0.0.2"));
    EXPECT_TRUE(
        api_.Blockchain().Start(b::Type::Litecoin_testnet4, "127.0.0.2"));
    EXPECT_TRUE(api_.Blockchain().Start(b::Type::PKT, "127.0.0.2"));
    EXPECT_TRUE(api_.Blockchain().Start(b::Type::UnitTest));
    EXPECT_TRUE(api_.Blockchain().Stop(b::Type::UnitTest));
    EXPECT_TRUE(api_.Blockchain().Stop(b::Type::PKT));
    EXPECT_TRUE(api_.Blockchain().Stop(b::Type::Litecoin_testnet4));
    EXPECT_TRUE(api_.Blockchain().Stop(b::Type::Litecoin));
    EXPECT_TRUE(api_.Blockchain().Stop(b::Type::Ethereum_ropsten));
    EXPECT_TRUE(api_.Blockchain().Stop(b::Type::Ethereum_frontier));
    EXPECT_TRUE(api_.Blockchain().Stop(b::Type::BitcoinCash_testnet3));
    EXPECT_TRUE(api_.Blockchain().Stop(b::Type::BitcoinCash));
    EXPECT_TRUE(api_.Blockchain().Stop(b::Type::Bitcoin_testnet3));
    EXPECT_TRUE(api_.Blockchain().Stop(b::Type::Bitcoin));
}
}  // namespace
