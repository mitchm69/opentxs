// Copyright (c) 2010-2021 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <atomic>
#include <iosfwd>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "api/client/blockchain/BalanceNode.hpp"
#include "internal/api/client/blockchain/Blockchain.hpp"
#include "opentxs/Proto.hpp"
#include "opentxs/Types.hpp"
#include "opentxs/Version.hpp"
#include "opentxs/api/client/blockchain/Types.hpp"
#include "opentxs/core/Identifier.hpp"
#include "opentxs/crypto/Types.hpp"
#include "opentxs/crypto/key/HD.hpp"
#include "opentxs/protobuf/BlockchainDeterministicAccountData.pb.h"
#include "opentxs/protobuf/HDPath.pb.h"

namespace opentxs
{
namespace api
{
namespace internal
{
struct Core;
}  // namespace internal
}  // namespace api

namespace proto
{
class BlockchainDeterministicAccountData;
}  // namespace proto

class PasswordPrompt;
}  // namespace opentxs

namespace opentxs::api::client::blockchain::implementation
{
class Deterministic : virtual public internal::Deterministic, public BalanceNode
{
public:
    auto Floor(const Subchain type) const noexcept
        -> std::optional<Bip32Index> final;
    auto BalanceElement(const Subchain type, const Bip32Index index) const
        noexcept(false) -> const Element& final;
    auto GenerateNext(const Subchain type, const PasswordPrompt& reason)
        const noexcept -> std::optional<Bip32Index> final;
    auto Key(const Subchain type, const Bip32Index index) const noexcept
        -> ECKey final;
    auto LastGenerated(const Subchain type) const noexcept
        -> std::optional<Bip32Index> final;
    auto Lookahead() const noexcept -> std::size_t final { return window_; }
    auto Path() const noexcept -> proto::HDPath final { return path_; }
    auto Reserve(
        const Subchain type,
        const PasswordPrompt& reason,
        const Identifier& contact,
        const std::string& label,
        const Time time) const noexcept -> std::optional<Bip32Index> override;
    auto RootNode(const PasswordPrompt& reason) const noexcept
        -> HDKey override;

    ~Deterministic() override = default;

protected:
    using IndexMap = std::map<Subchain, Bip32Index>;
    using SerializedType = proto::BlockchainDeterministicAccountData;

    struct ChainData {
        AddressData internal_{};
        AddressData external_{};
    };

    static constexpr Bip32Index window_{20};
    static constexpr Bip32Index max_index_{2147483648};

    const proto::HDPath path_;
#if OT_CRYPTO_WITH_BIP32
    HDKey key_;
#endif  // OT_CRYPTO_WITH_BIP32
    mutable ChainData data_;
    mutable IndexMap generated_;
    mutable IndexMap used_;

#if OT_CRYPTO_WITH_BIP32
    static auto instantiate_key(
        const api::internal::Core& api,
        proto::HDPath& path) -> HDKey;
#endif  // OT_CRYPTO_WITH_BIP32

    auto check_lookahead(const rLock& lock, const PasswordPrompt& reason) const
        noexcept(false) -> void;
#if OT_CRYPTO_WITH_BIP32
    auto check_lookahead(
        const rLock& lock,
        const Subchain type,
        const PasswordPrompt& reason) const noexcept(false) -> void;
#endif  // OT_CRYPTO_WITH_BIP32
    auto element(const rLock& lock, const Subchain type, const Bip32Index index)
        const noexcept(false) -> const Element&
    {
        return const_cast<Deterministic*>(this)->element(lock, type, index);
    }
    auto is_generated(const rLock&, const Subchain type, Bip32Index index)
        const noexcept
    {
        return index < generated_.at(type);
    }
#if OT_CRYPTO_WITH_BIP32
    auto need_lookahead(const rLock& lock, const Subchain type) const noexcept
        -> bool;
#endif  // OT_CRYPTO_WITH_BIP32
    auto serialize_deterministic(const rLock& lock, SerializedType& out)
        const noexcept -> void;
#if OT_CRYPTO_WITH_BIP32
    auto use_next(
        const rLock& lock,
        const Subchain type,
        const PasswordPrompt& reason,
        const Identifier& contact,
        const std::string& label,
        const Time time) const noexcept -> std::optional<Bip32Index>;
#endif  // OT_CRYPTO_WITH_BIP32

    auto element(
        const rLock& lock,
        const Subchain type,
        const Bip32Index index) noexcept(false) -> Element&;
    using BalanceNode::init;
    auto init(const PasswordPrompt& reason) noexcept(false) -> void;

    Deterministic(
        const api::internal::Core& api,
        const internal::BalanceTree& parent,
        const BalanceNodeType type,
        OTIdentifier&& id,
        const proto::HDPath path,
        ChainData&& data,
        Identifier& out) noexcept;
    Deterministic(
        const api::internal::Core& api,
        const internal::BalanceTree& parent,
        const BalanceNodeType type,
        const SerializedType& serialized,
        const Bip32Index generated,
        const Bip32Index used,
        ChainData&& data,
        Identifier& out) noexcept(false);

private:
    static constexpr auto BlockchainDeterministicAccountDataVersion =
        VersionNumber{1};

    static auto extract_contacts(
        const Bip32Index index,
        const AddressMap& map,
        std::set<OTIdentifier>& contacts) noexcept -> void;

    auto check_activity(
        const rLock& lock,
        const std::vector<Activity>& unspent,
        std::set<OTIdentifier>& contacts,
        const PasswordPrompt& reason) const noexcept -> bool final;
    auto confirm(
        const rLock& lock,
        const Subchain type,
        const Bip32Index index) noexcept -> void final;
#if OT_CRYPTO_WITH_BIP32
    auto generate(
        const rLock& lock,
        const Subchain type,
        const Bip32Index index,
        const PasswordPrompt& reason) const noexcept(false) -> Bip32Index;
    auto generate_next(
        const rLock& lock,
        const Subchain type,
        const PasswordPrompt& reason) const noexcept(false) -> Bip32Index;
#endif  // OT_CRYPTO_WITH_BIP32
    auto mutable_element(
        const rLock& lock,
        const Subchain type,
        const Bip32Index index) noexcept(false)
        -> internal::BalanceElement& final;
    virtual auto set_deterministic_contact(Element&) const noexcept -> void {}
    virtual auto set_deterministic_contact(
        std::set<OTIdentifier>&) const noexcept -> void
    {
    }
    auto set_metadata(
        const rLock& lock,
        const Subchain subchain,
        const Bip32Index index,
        const Identifier& contact,
        const std::string& label) const noexcept -> void;
    auto unconfirm(
        const rLock& lock,
        const Subchain type,
        const Bip32Index index) noexcept -> void final;

    Deterministic() = delete;
    Deterministic(const Deterministic&) = delete;
    Deterministic(Deterministic&&) = delete;
    auto operator=(const Deterministic&) -> Deterministic& = delete;
    auto operator=(Deterministic&&) -> Deterministic& = delete;
};
}  // namespace opentxs::api::client::blockchain::implementation
