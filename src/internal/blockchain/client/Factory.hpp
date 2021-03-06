// Copyright (c) 2010-2021 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <memory>
#include <string>

#include "opentxs/Types.hpp"

namespace opentxs
{
namespace api
{
namespace client
{
namespace blockchain
{
namespace database
{
namespace implementation
{
class Database;
}  // namespace implementation
}  // namespace database
}  // namespace blockchain

namespace internal
{
struct Blockchain;
}  // namespace internal
}  // namespace client

class Core;
}  // namespace api

namespace blockchain
{
namespace client
{
namespace internal
{
struct BlockDatabase;
struct BlockOracle;
struct Config;
struct FilterDatabase;
struct FilterOracle;
struct HeaderDatabase;
struct HeaderOracle;
struct IO;
struct Network;
struct PeerDatabase;
struct PeerManager;
struct Wallet;
struct WalletDatabase;
}  // namespace internal
}  // namespace client

namespace internal
{
struct Database;
}  // namespace internal
}  // namespace blockchain
}  // namespace opentxs

namespace opentxs::factory
{
auto BlockchainDatabase(
    const api::Core& api,
    const api::client::internal::Blockchain& blockchain,
    const blockchain::client::internal::Network& network,
    const api::client::blockchain::database::implementation::Database& db,
    const blockchain::Type type) noexcept
    -> std::unique_ptr<blockchain::internal::Database>;
auto BlockchainFilterOracle(
    const api::Core& api,
    const api::client::internal::Blockchain& blockchain,
    const blockchain::client::internal::Config& config,
    const blockchain::client::internal::Network& network,
    const blockchain::client::internal::HeaderOracle& header,
    const blockchain::client::internal::BlockOracle& block,
    const blockchain::client::internal::FilterDatabase& database,
    const blockchain::Type type,
    const std::string& shutdown) noexcept
    -> std::unique_ptr<blockchain::client::internal::FilterOracle>;
OPENTXS_EXPORT auto BlockchainNetworkBitcoin(
    const api::Core& api,
    const api::client::internal::Blockchain& blockchain,
    const blockchain::Type type,
    const blockchain::client::internal::Config& config,
    const std::string& seednode,
    const std::string& syncEndpoint) noexcept
    -> std::unique_ptr<blockchain::client::internal::Network>;
auto BlockchainPeerManager(
    const api::Core& api,
    const blockchain::client::internal::Config& config,
    const blockchain::client::internal::Network& network,
    const blockchain::client::internal::HeaderOracle& headers,
    const blockchain::client::internal::FilterOracle& filter,
    const blockchain::client::internal::BlockOracle& block,
    const blockchain::client::internal::PeerDatabase& database,
    const blockchain::client::internal::IO& io,
    const blockchain::Type type,
    const api::client::blockchain::BlockStorage policy,
    const std::string& seednode,
    const std::string& shutdown) noexcept
    -> std::unique_ptr<blockchain::client::internal::PeerManager>;
OPENTXS_EXPORT auto BlockchainWallet(
    const api::Core& api,
    const api::client::internal::Blockchain& blockchain,
    const blockchain::client::internal::Network& parent,
    const blockchain::client::internal::WalletDatabase& db,
    const blockchain::Type chain,
    const std::string& shutdown)
    -> std::unique_ptr<blockchain::client::internal::Wallet>;
auto BlockOracle(
    const api::Core& api,
    const blockchain::client::internal::Network& network,
    const blockchain::client::internal::HeaderOracle& header,
    const blockchain::client::internal::BlockDatabase& db,
    const blockchain::Type chain,
    const std::string& shutdown) noexcept
    -> std::unique_ptr<blockchain::client::internal::BlockOracle>;
auto HeaderOracle(
    const api::Core& api,
    const blockchain::client::internal::HeaderDatabase& database,
    const blockchain::Type type) noexcept
    -> std::unique_ptr<blockchain::client::internal::HeaderOracle>;
}  // namespace opentxs::factory
