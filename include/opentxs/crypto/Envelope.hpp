// Copyright (c) 2010-2021 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CRYPTO_ENVELOPE_HPP
#define OPENTXS_CRYPTO_ENVELOPE_HPP

// IWYU pragma: no_include "opentxs/Proto.hpp"

#include "opentxs/Forward.hpp"  // IWYU pragma: associated

#include <set>

#include "opentxs/Bytes.hpp"
#include "opentxs/Pimpl.hpp"
#include "opentxs/Proto.hpp"

namespace opentxs
{
namespace crypto
{
class Envelope;
}  // namespace crypto

namespace proto
{
class Envelope;
}  // namespace proto

using OTEnvelope = Pimpl<crypto::Envelope>;
}  // namespace opentxs

namespace opentxs
{
namespace crypto
{
class Envelope
{
public:
    using Recipients = std::set<Nym_p>;
    using SerializedType = proto::Envelope;

    OPENTXS_EXPORT virtual bool Armored(
        opentxs::Armored& ciphertext) const noexcept = 0;
    OPENTXS_EXPORT virtual bool Open(
        const identity::Nym& recipient,
        const AllocateOutput plaintext,
        const PasswordPrompt& reason) const noexcept = 0;
    OPENTXS_EXPORT virtual SerializedType Serialize() const noexcept = 0;

    OPENTXS_EXPORT virtual bool Seal(
        const Recipients& recipients,
        const ReadView plaintext,
        const PasswordPrompt& reason) noexcept = 0;
    OPENTXS_EXPORT virtual bool Seal(
        const identity::Nym& theRecipient,
        const ReadView plaintext,
        const PasswordPrompt& reason) noexcept = 0;

    virtual ~Envelope() = default;

protected:
    Envelope() = default;

private:
    virtual Envelope* clone() const noexcept = 0;

    Envelope(const Envelope&) = delete;
    Envelope(Envelope&&) = delete;
    Envelope& operator=(const Envelope&) = delete;
    Envelope& operator=(Envelope&&) = delete;
};
}  // namespace crypto
}  // namespace opentxs
#endif
