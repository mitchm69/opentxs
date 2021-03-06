// Copyright (c) 2010-2021 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// IWYU pragma: private
// IWYU pragma: friend ".*src/api/client/Manager.cpp"

#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "api/Core.hpp"
#include "internal/api/client/Client.hpp"
#include "opentxs/Types.hpp"
#include "opentxs/Version.hpp"
#include "opentxs/api/client/Activity.hpp"
#include "opentxs/api/client/Blockchain.hpp"
#include "opentxs/api/client/Contacts.hpp"
#include "opentxs/api/client/OTX.hpp"
#include "opentxs/api/client/Pair.hpp"
#include "opentxs/api/client/ServerAction.hpp"
#include "opentxs/api/client/UI.hpp"
#include "opentxs/api/client/Workflow.hpp"
#include "opentxs/api/network/ZMQ.hpp"

namespace opentxs
{
namespace api
{
namespace internal
{
struct Context;
}  // namespace internal

class Crypto;
class Legacy;
class Settings;
}  // namespace api

namespace identifier
{
class Nym;
class Server;
}  // namespace identifier

namespace network
{
namespace zeromq
{
class Context;
}  // namespace zeromq
}  // namespace network

class Flag;
class Identifier;
class OTAPI_Exec;
class OT_API;
}  // namespace opentxs

namespace opentxs::api::client::implementation
{
class Manager final : public opentxs::api::client::internal::Manager,
                      public api::implementation::Core
{
public:
    auto Activity() const -> const api::client::Activity& final;
    auto Blockchain() const -> const api::client::Blockchain& final;
    auto Contacts() const -> const api::client::Contacts& final;
    auto Exec(const std::string& wallet = "") const -> const OTAPI_Exec& final;
    auto InternalUI() const noexcept -> const internal::UI& final
    {
        return *ui_;
    }
    using Core::Lock;
    auto Lock(const identifier::Nym& nymID, const identifier::Server& serverID)
        const -> std::recursive_mutex& final;
    auto OTAPI(const std::string& wallet = "") const -> const OT_API& final;
    auto OTX() const -> const client::OTX& final;
    auto Pair() const -> const api::client::Pair& final;
    auto ServerAction() const -> const client::ServerAction& final;
    auto UI() const -> const api::client::UI& final;
    auto Workflow() const -> const client::Workflow& final;
    auto ZMQ() const -> const api::network::ZMQ& final;

    void StartActivity() final;
#if OT_BLOCKCHAIN
    auto StartBlockchain() noexcept -> void;
#endif  // OT_BLOCKCHAIN
    void StartContacts() final;

    Manager(
        const api::internal::Context& parent,
        Flag& running,
        const ArgList& args,
        const api::Settings& config,
        const api::Crypto& crypto,
        const opentxs::network::zeromq::Context& context,
        const std::string& dataFolder,
        const int instance);

    ~Manager() final;

private:
    std::unique_ptr<network::ZMQ> zeromq_;
    std::unique_ptr<internal::Contacts> contacts_;
    std::unique_ptr<internal::Activity> activity_;
    std::shared_ptr<internal::Blockchain> blockchain_;
    std::unique_ptr<client::Workflow> workflow_;
    std::unique_ptr<OT_API> ot_api_;
    std::unique_ptr<OTAPI_Exec> otapi_exec_;
    std::unique_ptr<client::ServerAction> server_action_;
    std::unique_ptr<client::OTX> otx_;
    std::unique_ptr<internal::Pair> pair_;
    std::unique_ptr<internal::UI> ui_;
    mutable std::mutex map_lock_;
    mutable std::map<ContextID, std::recursive_mutex> context_locks_;

    static auto init(
        const api::Legacy& legacy,
        const internal::Manager& parent,
        const std::string& dataFolder,
        const ArgList& args,
        std::shared_ptr<internal::Blockchain>& blockchain,
        std::unique_ptr<internal::Contacts>& contacts) noexcept
        -> std::unique_ptr<internal::Activity>;

    auto get_lock(const ContextID context) const -> std::recursive_mutex&;

    void Cleanup();
    void Init();

    Manager() = delete;
    Manager(const Manager&) = delete;
    Manager(Manager&&) = delete;
    auto operator=(const Manager&) -> Manager& = delete;
    auto operator=(Manager&&) -> Manager& = delete;
};
}  // namespace opentxs::api::client::implementation
