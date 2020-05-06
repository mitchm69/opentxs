// Copyright (c) 2010-2020 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "0_stdafx.hpp"               // IWYU pragma: associated
#include "1_Internal.hpp"             // IWYU pragma: associated
#include "api/crypto/Asymmetric.hpp"  // IWYU pragma: associated

#include <type_traits>
#include <utility>

#include "Factory.hpp"
#include "crypto/key/Null.hpp"
#include "internal/api/Api.hpp"
#include "opentxs/api/crypto/Asymmetric.hpp"
#include "opentxs/api/crypto/Crypto.hpp"
#include "opentxs/core/Log.hpp"
#include "opentxs/core/LogSource.hpp"
#include "opentxs/core/crypto/NymParameters.hpp"
#include "opentxs/crypto/Bip32.hpp"
#include "opentxs/crypto/key/Asymmetric.hpp"
#if OT_CRYPTO_SUPPORTED_KEY_ED25519
#include "opentxs/crypto/key/Ed25519.hpp"
#endif  // OT_CRYPTO_SUPPORTED_KEY_ED25519
#include "opentxs/crypto/key/HD.hpp"
#if OT_CRYPTO_SUPPORTED_KEY_RSA
#include "opentxs/crypto/key/RSA.hpp"
#endif  // OT_CRYPTO_SUPPORTED_KEY_RSA
#if OT_CRYPTO_SUPPORTED_KEY_SECP256K1
#include "opentxs/crypto/key/Secp256k1.hpp"
#endif  // OT_CRYPTO_SUPPORTED_KEY_SECP256K1
#include "opentxs/protobuf/Enums.pb.h"

#define OT_METHOD "opentxs::api::crypto::implementation::Asymmetric::"

namespace opentxs
{
api::crypto::internal::Asymmetric* Factory::AsymmetricAPI(
    const api::internal::Core& api)
{
    return new api::crypto::implementation::Asymmetric(api);
}
}  // namespace opentxs

namespace opentxs::api::crypto::implementation
{
const VersionNumber Asymmetric::serialized_path_version_{1};

const Asymmetric::TypeMap Asymmetric::curve_to_key_type_{
    {EcdsaCurve::invalid, proto::AKEYTYPE_ERROR},
    {EcdsaCurve::secp256k1, proto::AKEYTYPE_SECP256K1},
    {EcdsaCurve::ed25519, proto::AKEYTYPE_ED25519},
};

Asymmetric::Asymmetric(const api::internal::Core& api)
    : api_(api)
{
}

#if OT_CRYPTO_WITH_BIP32
template <typename ReturnType, typename NullType>
auto Asymmetric::instantiate_hd_key(
    const proto::AsymmetricKeyType type,
    const std::string& seedID,
    const opentxs::crypto::Bip32::Key& serialized,
    const PasswordPrompt& reason,
    const proto::KeyRole role,
    const VersionNumber version) const noexcept -> std::unique_ptr<ReturnType>
{
    using Pointer = std::unique_ptr<ReturnType>;

    const auto& [privkey, ccode, pubkey, path, parent] = serialized;

    switch (type) {
        case proto::AKEYTYPE_ED25519:
#if OT_CRYPTO_SUPPORTED_KEY_ED25519
        {
            return Pointer{opentxs::Factory::Ed25519Key(
                api_,
                api_.Crypto().ED25519(),
                privkey,
                ccode,
                pubkey,
                serialize_path(seedID, path),
                parent,
                role,
                version,
                reason)};
        }
#else
            break;
#endif  // OT_CRYPTO_SUPPORTED_KEY_ED25519
        case proto::AKEYTYPE_SECP256K1:
#if OT_CRYPTO_SUPPORTED_KEY_SECP256K1
        {
            return Pointer{opentxs::Factory::Secp256k1Key(
                api_,
                api_.Crypto().SECP256K1(),
                privkey,
                ccode,
                pubkey,
                serialize_path(seedID, path),
                parent,
                role,
                version,
                reason)};
        }
#else
            break;
#endif  // OT_CRYPTO_SUPPORTED_KEY_SECP256K1
        default: {
        }
    }

    LogOutput(OT_METHOD)(__FUNCTION__)(": Invalid key type.").Flush();

    return std::make_unique<NullType>();
}
#endif  // OT_CRYPTO_WITH_BIP32

template <typename ReturnType, typename NullType>
auto Asymmetric::instantiate_serialized_key(
    const proto::AsymmetricKey& serialized) const noexcept
    -> std::unique_ptr<ReturnType>

{
#if OT_CRYPTO_SUPPORTED_KEY_ED25519 || OT_CRYPTO_SUPPORTED_KEY_SECP256K1
    using Pointer = std::unique_ptr<ReturnType>;
#endif  // OT_CRYPTO_SUPPORTED_KEY_ED25519 || OT_CRYPTO_SUPPORTED_KEY_SECP256K1

    switch (serialized.type()) {
        case proto::AKEYTYPE_ED25519:
#if OT_CRYPTO_SUPPORTED_KEY_ED25519
        {
            return Pointer{opentxs::Factory::Ed25519Key(
                api_, api_.Crypto().ED25519(), serialized)};
        }
#else
            break;
#endif  // OT_CRYPTO_SUPPORTED_KEY_ED25519
        case proto::AKEYTYPE_SECP256K1:
#if OT_CRYPTO_SUPPORTED_KEY_SECP256K1
        {
            return Pointer{opentxs::Factory::Secp256k1Key(
                api_, api_.Crypto().SECP256K1(), serialized)};
        }
#else
            break;
#endif  // OT_CRYPTO_SUPPORTED_KEY_SECP256K1
        default: {
        }
    }

    LogOutput(OT_METHOD)(__FUNCTION__)(
        ": Open-Transactions isn't built with support for this key type.")
        .Flush();

    return std::make_unique<NullType>();
}

Asymmetric::ECKey Asymmetric::InstantiateECKey(
    const proto::AsymmetricKey& serialized) const
{
    using ReturnType = opentxs::crypto::key::EllipticCurve;
    using NullType = opentxs::crypto::key::implementation::NullEC;

    switch (serialized.type()) {
        case proto::AKEYTYPE_ED25519:
        case proto::AKEYTYPE_SECP256K1: {
            return instantiate_serialized_key<ReturnType, NullType>(serialized);
        }
        case (proto::AKEYTYPE_LEGACY): {
            LogOutput(OT_METHOD)(__FUNCTION__)(": Wrong key type (RSA)")
                .Flush();
        } break;
        default: {
        }
    }

    return std::make_unique<NullType>();
}

Asymmetric::HDKey Asymmetric::InstantiateHDKey(
    const proto::AsymmetricKey& serialized) const
{
    using ReturnType = opentxs::crypto::key::HD;
    using NullType = opentxs::crypto::key::implementation::NullHD;

    switch (serialized.type()) {
        case proto::AKEYTYPE_ED25519:
        case proto::AKEYTYPE_SECP256K1: {
            return instantiate_serialized_key<ReturnType, NullType>(serialized);
        }
        case (proto::AKEYTYPE_LEGACY): {
            LogOutput(OT_METHOD)(__FUNCTION__)(": Wrong key type (RSA)")
                .Flush();
        } break;
        default: {
        }
    }

    return std::make_unique<NullType>();
}

#if OT_CRYPTO_WITH_BIP32
Asymmetric::HDKey Asymmetric::InstantiateKey(
    const proto::AsymmetricKeyType type,
    const std::string& seedID,
    const opentxs::crypto::Bip32::Key& serialized,
    const PasswordPrompt& reason,
    const proto::KeyRole role,
    const VersionNumber version) const
{
    using ReturnType = opentxs::crypto::key::HD;
    using BlankType = opentxs::crypto::key::implementation::NullHD;

    return instantiate_hd_key<ReturnType, BlankType>(
        type, seedID, serialized, reason, role, version);
}
#endif  // OT_CRYPTO_WITH_BIP32

Asymmetric::Key Asymmetric::InstantiateKey(
    const proto::AsymmetricKey& serialized) const
{
    using ReturnType = opentxs::crypto::key::Asymmetric;
    using NullType = opentxs::crypto::key::implementation::Null;

    switch (serialized.type()) {
        case proto::AKEYTYPE_ED25519:
        case proto::AKEYTYPE_SECP256K1: {
            return instantiate_serialized_key<ReturnType, NullType>(serialized);
        }
#if OT_CRYPTO_SUPPORTED_KEY_RSA
        case (proto::AKEYTYPE_LEGACY): {
            return std::unique_ptr<ReturnType>{opentxs::Factory::RSAKey(
                api_, api_.Crypto().RSA(), serialized)};
        }
#endif  // OT_CRYPTO_SUPPORTED_KEY_RSA
        default: {
        }
    }

    LogOutput(OT_METHOD)(__FUNCTION__)(
        ": Open-Transactions isn't built with support for this key type.")
        .Flush();

    return std::make_unique<NullType>();
}

#if OT_CRYPTO_WITH_BIP32
Asymmetric::HDKey Asymmetric::NewHDKey(
    const std::string& seedID,
    const OTPassword& seed,
    const EcdsaCurve& curve,
    const opentxs::crypto::Bip32::Path& path,
    const PasswordPrompt& reason,
    const proto::KeyRole role,
    const VersionNumber version) const
{
    return InstantiateKey(
        curve_to_key_type_.at(curve),
        seedID,
        api_.Crypto().BIP32().DeriveKey(curve, seed, path),
        reason,
        role,
        version);
}

#if OT_CRYPTO_SUPPORTED_KEY_SECP256K1
auto Asymmetric::NewSecp256k1Key(
    const std::string& seedID,
    const OTPassword& seed,
    const opentxs::crypto::Bip32::Path& derive,
    const PasswordPrompt& reason,
    const proto::KeyRole role,
    const VersionNumber version) const -> Secp256k1Key
{
    const auto serialized =
        api_.Crypto().BIP32().DeriveKey(EcdsaCurve::secp256k1, seed, derive);
    const auto& [privkey, ccode, pubkey, path, parent] = serialized;

    return Secp256k1Key{opentxs::Factory::Secp256k1Key(
        api_,
        api_.Crypto().SECP256K1(),
        privkey,
        ccode,
        pubkey,
        serialize_path(seedID, path),
        parent,
        role,
        version,
        reason)};
}
#endif  // OT_CRYPTO_SUPPORTED_KEY_SECP256K1
#endif  // OT_CRYPTO_WITH_BIP32

Asymmetric::Key Asymmetric::NewKey(
    const NymParameters& params,
    const PasswordPrompt& reason,
    const proto::KeyRole role,
    const VersionNumber version) const
{
    switch (params.AsymmetricKeyType()) {
#if OT_CRYPTO_SUPPORTED_KEY_ED25519
        case (proto::AKEYTYPE_ED25519): {
            return Key{opentxs::Factory::Ed25519Key(
                api_, api_.Crypto().ED25519(), role, version, reason)};
        }
#endif  // OT_CRYPTO_SUPPORTED_KEY_ED25519
#if OT_CRYPTO_SUPPORTED_KEY_SECP256K1
        case (proto::AKEYTYPE_SECP256K1): {
            return Key{opentxs::Factory::Secp256k1Key(
                api_, api_.Crypto().SECP256K1(), role, version, reason)};
        }
#endif  // OT_CRYPTO_SUPPORTED_KEY_SECP256K1
#if OT_CRYPTO_SUPPORTED_KEY_RSA
        case (proto::AKEYTYPE_LEGACY): {
            return opentxs::Factory::RSAKey(
                api_, api_.Crypto().RSA(), role, version, params, reason);
        }
#endif  // OT_CRYPTO_SUPPORTED_KEY_RSA
        default: {
            LogOutput(OT_METHOD)(__FUNCTION__)(
                ": Open-Transactions isn't built with support for this key "
                "type.")
                .Flush();
        }
    }

    return {};
}

#if OT_CRYPTO_WITH_BIP32
proto::HDPath Asymmetric::serialize_path(
    const std::string& seedID,
    const opentxs::crypto::Bip32::Path& children)
{
    proto::HDPath output;
    output.set_version(serialized_path_version_);
    output.set_root(seedID);

    for (const auto& index : children) { output.add_child(index); }

    return output;
}
#endif  // OT_CRYPTO_WITH_BIP32
}  // namespace opentxs::api::crypto::implementation
