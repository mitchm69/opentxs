// Copyright (c) 2010-2021 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "0_stdafx.hpp"         // IWYU pragma: associated
#include "1_Internal.hpp"       // IWYU pragma: associated
#include "identity/Source.hpp"  // IWYU pragma: associated

#include <memory>
#include <stdexcept>
#include <string>

#include "2_Factory.hpp"
#include "internal/api/Api.hpp"
#include "opentxs/Pimpl.hpp"
#include "opentxs/Types.hpp"
#include "opentxs/api/Factory.hpp"
#include "opentxs/core/Armored.hpp"
#include "opentxs/core/Data.hpp"
#include "opentxs/core/Log.hpp"
#include "opentxs/core/LogSource.hpp"
#include "opentxs/core/String.hpp"
#include "opentxs/core/crypto/NymParameters.hpp"
#include "opentxs/core/crypto/PaymentCode.hpp"
#include "opentxs/core/identifier/Nym.hpp"
#include "opentxs/crypto/key/Asymmetric.hpp"
#include "opentxs/crypto/key/Keypair.hpp"
#include "opentxs/crypto/library/AsymmetricProvider.hpp"
#include "opentxs/identity/credential/Primary.hpp"
#include "opentxs/protobuf/AsymmetricKey.pb.h"
#include "opentxs/protobuf/Credential.pb.h"
#include "opentxs/protobuf/Enums.pb.h"
#include "opentxs/protobuf/KeyCredential.pb.h"
#include "opentxs/protobuf/MasterCredentialParameters.pb.h"
#include "opentxs/protobuf/NymIDSource.pb.h"
#include "opentxs/protobuf/PaymentCode.pb.h"
#include "opentxs/protobuf/SourceProof.pb.h"

#define OT_METHOD "opentxs::identity::Source::"

namespace opentxs
{
auto Factory::NymIDSource(
    const api::internal::Core& api,
    NymParameters& params,
    const opentxs::PasswordPrompt& reason) -> identity::Source*
{
    using ReturnType = identity::implementation::Source;

    switch (params.SourceType()) {
        case proto::SOURCETYPE_BIP47: {
#if OT_CRYPTO_SUPPORTED_KEY_SECP256K1 && OT_CRYPTO_WITH_BIP32
            const auto paymentCode = api.Factory().PaymentCode(
                params.Seed(),
                params.Nym(),
                params.PaymentCodeVersion(),
                reason);

            return new ReturnType{api.Factory(), paymentCode};
#else
            LogOutput("opentxs::Factory::")(__FUNCTION__)(
                ": opentxs was build without bip47 support")
                .Flush();

            return nullptr;
#endif  // OT_CRYPTO_SUPPORTED_KEY_SECP256K1 && OT_CRYPTO_WITH_BIP32
        }
        case proto::SOURCETYPE_PUBKEY:
            switch (params.credentialType()) {
                case proto::CREDTYPE_LEGACY: {
                    params.Keypair() = api.Factory().Keypair(
                        params,
                        crypto::key::Asymmetric::DefaultVersion,
                        proto::KEYROLE_SIGN,
                        reason);
                } break;
                case proto::CREDTYPE_HD:
#if OT_CRYPTO_WITH_BIP32
                {
                    const auto curve =
                        crypto::AsymmetricProvider::KeyTypeToCurve(
                            params.AsymmetricKeyType());

                    if (EcdsaCurve::invalid == curve) {
                        throw std::runtime_error("Invalid curve type");
                    }

                    params.Keypair() = api.Factory().Keypair(
                        params.Seed(),
                        params.Nym(),
                        params.Credset(),
                        params.CredIndex(),
                        curve,
                        proto::KEYROLE_SIGN,
                        reason);
                } break;
#endif  // OT_CRYPTO_WITH_BIP32
                case proto::CREDTYPE_ERROR:
                default: {
                    throw std::runtime_error("Unsupported credential type");
                }
            }

            if (false == bool(params.Keypair().get())) {
                LogOutput("opentxs::Factory::")(__FUNCTION__)(
                    ": Failed to generate signing keypair")
                    .Flush();

                return nullptr;
            }

            return new ReturnType{api.Factory(), params};
        case proto::SOURCETYPE_ERROR:
        default: {
            LogOutput("opentxs::Factory::")(__FUNCTION__)(
                ": Unsupported source type.")
                .Flush();

            return nullptr;
        }
    }
}

auto Factory::NymIDSource(
    const api::internal::Core& api,
    const proto::NymIDSource& serialized) -> identity::Source*
{
    using ReturnType = identity::implementation::Source;

    return new ReturnType{api.Factory(), serialized};
}
}  // namespace opentxs

namespace opentxs::identity::implementation
{
const VersionConversionMap Source::key_to_source_version_{
    {1, 1},
    {2, 2},
    {3, 2},
};

Source::Source(
    const api::Factory& factory,
    const proto::NymIDSource& serialized) noexcept
    : factory_(factory)
    , type_(serialized.type())
    , pubkey_(deserialize_pubkey(factory, type_, serialized))
    , payment_code_(deserialize_paymentcode(factory, type_, serialized))
    , version_(serialized.version())
{
}

Source::Source(
    const api::Factory& factory,
    const NymParameters& nymParameters) noexcept(false)
    : factory_{factory}
    , type_(nymParameters.SourceType())
    , pubkey_(nymParameters.Keypair().GetPublicKey())
    , payment_code_(factory_.PaymentCode(""))
    , version_(key_to_source_version_.at(pubkey_->Version()))

{
    if (false == bool(pubkey_.get())) {
        throw std::runtime_error("Invalid pubkey");
    }
}

Source::Source(const api::Factory& factory, const PaymentCode& source) noexcept
    : factory_{factory}
    , type_(proto::SOURCETYPE_BIP47)
    , pubkey_(crypto::key::Asymmetric::Factory())
    , payment_code_{source}
    , version_(key_to_source_version_.at(payment_code_->Version()))
{
}

Source::Source(const Source& rhs) noexcept
    : Source(rhs.factory_, *rhs.Serialize())
{
}

auto Source::asData() const -> OTData
{
    std::shared_ptr<proto::NymIDSource> serialized = Serialize();

    return factory_.Data(*serialized);
}

auto Source::deserialize_paymentcode(
    const api::Factory& factory,
    const proto::SourceType type,
    const proto::NymIDSource& serialized) -> OTPaymentCode
{
    if (proto::SOURCETYPE_BIP47 == type) {

        return factory.PaymentCode(serialized.paymentcode());
    } else {

        return factory.PaymentCode("");
    }
}

auto Source::deserialize_pubkey(
    const api::Factory& factory,
    const proto::SourceType type,
    const proto::NymIDSource& serialized) -> OTAsymmetricKey
{
    if (proto::SOURCETYPE_PUBKEY == type) {

        return factory.AsymmetricKey(serialized.key());
    } else {

        return crypto::key::Asymmetric::Factory();
    }
}

auto Source::extract_key(
    const proto::Credential& credential,
    const proto::KeyRole role) -> std::unique_ptr<proto::AsymmetricKey>
{
    std::unique_ptr<proto::AsymmetricKey> output;

    const bool master = (proto::CREDROLE_MASTERKEY == credential.role());
    const bool child = (proto::CREDROLE_CHILDKEY == credential.role());
    const bool keyCredential = master || child;

    if (!keyCredential) { return output; }

    const auto& publicCred = credential.publiccredential();

    for (auto& key : publicCred.key()) {
        if (role == key.role()) {
            output.reset(new proto::AsymmetricKey(key));

            break;
        }
    }

    return output;
}

auto Source::NymID() const noexcept -> OTNymID
{
    auto nymID = factory_.NymID();

    switch (type_) {
        case proto::SOURCETYPE_PUBKEY: {
            nymID->CalculateDigest(asData()->Bytes());

        } break;
        case proto::SOURCETYPE_BIP47: {
            nymID = payment_code_->ID();
        } break;
        default: {
        }
    }

    return nymID;
}

auto Source::Serialize() const noexcept -> std::shared_ptr<proto::NymIDSource>
{
    auto source = std::make_shared<proto::NymIDSource>();
    source->set_version(version_);
    source->set_type(type_);

    switch (type_) {
        case proto::SOURCETYPE_PUBKEY: {
            OT_ASSERT(pubkey_.get())

            auto key = pubkey_->Serialize();
            key->set_role(proto::KEYROLE_SIGN);
            *(source->mutable_key()) = *key;

        } break;
        case proto::SOURCETYPE_BIP47: {
            *(source->mutable_paymentcode()) = payment_code_->Serialize();

        } break;
        default: {
        }
    }

    return source;
}

// This function assumes that all internal verification checks are complete
// except for the source proof
auto Source::Verify(
    const proto::Credential& master,
    [[maybe_unused]] const proto::Signature& sourceSignature) const noexcept
    -> bool
{
    bool isSelfSigned, sameSource;
    std::unique_ptr<proto::AsymmetricKey> signingKey;
    std::shared_ptr<proto::AsymmetricKey> sourceKey;

    switch (type_) {
        case proto::SOURCETYPE_PUBKEY: {
            if (!pubkey_.get()) { return false; }

            isSelfSigned =
                (proto::SOURCEPROOFTYPE_SELF_SIGNATURE ==
                 master.masterdata().sourceproof().type());

            if (!isSelfSigned) {
                OT_ASSERT_MSG(false, "Not yet implemented");

                return false;
            }

            signingKey = extract_key(master, proto::KEYROLE_SIGN);

            if (!signingKey) {
                LogOutput(OT_METHOD)(__FUNCTION__)(
                    ": Failed to extract signing key.")
                    .Flush();

                return false;
            }

            sourceKey = pubkey_->Serialize();
            sameSource = (sourceKey->key() == signingKey->key());

            if (!sameSource) {
                LogOutput(OT_METHOD)(__FUNCTION__)(": Master credential was not"
                                                   " derived from this source.")
                    .Flush();

                return false;
            }
        } break;
        case proto::SOURCETYPE_BIP47:
#if OT_CRYPTO_SUPPORTED_KEY_SECP256K1
        {
            if (!payment_code_->Verify(master, sourceSignature)) {
                LogOutput(OT_METHOD)(__FUNCTION__)(
                    ": Invalid source signature.")
                    .Flush();

                return false;
            }
        } break;
#else
        {
            LogOutput(OT_METHOD)(__FUNCTION__)(
                ": Missing support for secp256k1")
                .Flush();

            return false;
        }
#endif  // OT_CRYPTO_SUPPORTED_KEY_SECP256K1
        default: {
            return false;
        }
    }

    return true;
}  // namespace opentxs::identity::implementation

auto Source::Sign(
    [[maybe_unused]] const identity::credential::Primary& credential,
    [[maybe_unused]] proto::Signature& sig,
    [[maybe_unused]] const PasswordPrompt& reason) const noexcept -> bool
{
    bool goodsig = false;

    switch (type_) {
        case (proto::SOURCETYPE_PUBKEY): {
            OT_ASSERT_MSG(false, "This is not implemented yet.");

        } break;
        case (proto::SOURCETYPE_BIP47): {
#if OT_CRYPTO_SUPPORTED_KEY_SECP256K1
            goodsig = payment_code_->Sign(credential, sig, reason);
#else
            LogOutput(OT_METHOD)(__FUNCTION__)(
                ": Missing support for secp256k1")
                .Flush();
#endif
        } break;
        default: {
        }
    }

    return goodsig;
}

auto Source::asString() const noexcept -> OTString
{
    return OTString(factory_.Armored(asData()));
}

auto Source::Description() const noexcept -> OTString
{
    auto description = String::Factory();
    auto keyID = factory_.Identifier();

    switch (type_) {
        case (proto::SOURCETYPE_PUBKEY): {
            if (pubkey_.get()) {
                pubkey_->CalculateID(keyID);
                description = String::Factory(keyID);
            }
        } break;
        case (proto::SOURCETYPE_BIP47): {
            description = String::Factory(payment_code_->asBase58());

        } break;
        default: {
        }
    }

    return description;
}
}  // namespace opentxs::identity::implementation
