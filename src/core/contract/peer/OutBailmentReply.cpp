// Copyright (c) 2010-2020 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "0_stdafx.hpp"                             // IWYU pragma: associated
#include "1_Internal.hpp"                           // IWYU pragma: associated
#include "core/contract/peer/OutbailmentReply.hpp"  // IWYU pragma: associated

#include <memory>
#include <stdexcept>
#include <utility>

#include "Factory.hpp"
#include "core/contract/peer/PeerReply.hpp"
#include "internal/api/Api.hpp"
#include "opentxs/Proto.hpp"
#include "opentxs/api/Factory.hpp"
#include "opentxs/core/Log.hpp"
#include "opentxs/core/LogSource.hpp"
#include "opentxs/protobuf/Check.hpp"
#include "opentxs/protobuf/PeerEnums.pb.h"
#include "opentxs/protobuf/verify/PeerReply.hpp"

#define CURRENT_VERSION 4

namespace opentxs
{
using ParentType = contract::peer::implementation::Reply;
using ReturnType = contract::peer::reply::implementation::Outbailment;

auto Factory::OutBailmentReply(
    const api::internal::Core& api,
    const Nym_p& nym,
    const identifier::Nym& initiator,
    const Identifier& request,
    const identifier::Server& server,
    const std::string& terms,
    const opentxs::PasswordPrompt& reason) noexcept
    -> std::shared_ptr<contract::peer::reply::Outbailment>
{
    auto pRequest = ParentType::LoadRequest(api, nym, request);

    if (false == bool(pRequest)) { return {}; }

    const auto peerRequest = *pRequest;

    try {
        auto output = std::make_shared<ReturnType>(
            api,
            nym,
            api.Factory().NymID(peerRequest.initiator()),
            request,
            server,
            terms);

        OT_ASSERT(output);

        auto& reply = *output;

        if (false == ParentType::Finish(reply, reason)) { return {}; }

        return std::move(output);
    } catch (const std::exception& e) {
        LogOutput("opentxs::Factory::")(__FUNCTION__)(": ")(e.what()).Flush();

        return {};
    }
}

auto Factory::OutBailmentReply(
    const api::internal::Core& api,
    const Nym_p& nym,
    const proto::PeerReply& serialized) noexcept
    -> std::shared_ptr<contract::peer::reply::Outbailment>
{
    if (false == proto::Validate(serialized, VERBOSE)) {
        LogOutput("opentxs::Factory::")(__FUNCTION__)(
            ": Invalid serialized reply.")
            .Flush();

        return {};
    }

    try {
        auto output = std::make_shared<ReturnType>(api, nym, serialized);

        OT_ASSERT(output);

        auto& contract = *output;
        Lock lock(contract.lock_);

        if (false == contract.validate(lock)) {
            LogOutput("opentxs::Factory::")(__FUNCTION__)(": Invalid reply.")
                .Flush();

            return {};
        }

        return std::move(output);
    } catch (const std::exception& e) {
        LogOutput("opentxs::Factory::")(__FUNCTION__)(": ")(e.what()).Flush();

        return {};
    }
}
}  // namespace opentxs

namespace opentxs::contract::peer::reply::implementation
{
Outbailment::Outbailment(
    const api::internal::Core& api,
    const Nym_p& nym,
    const identifier::Nym& initiator,
    const Identifier& request,
    const identifier::Server& server,
    const std::string& terms)
    : Reply(
          api,
          nym,
          CURRENT_VERSION,
          initiator,
          server,
          proto::PEERREQUEST_OUTBAILMENT,
          request,
          terms)
{
    Lock lock(lock_);
    first_time_init(lock);
}

Outbailment::Outbailment(
    const api::internal::Core& api,
    const Nym_p& nym,
    const SerializedType& serialized)
    : Reply(api, nym, serialized, serialized.outbailment().instructions())
{
    Lock lock(lock_);
    init_serialized(lock);
}

Outbailment::Outbailment(const Outbailment& rhs)
    : Reply(rhs)
{
}

auto Outbailment::IDVersion(const Lock& lock) const -> SerializedType
{
    auto contract = Reply::IDVersion(lock);
    auto& bailment = *contract.mutable_outbailment();
    bailment.set_version(version_);
    bailment.set_instructions(conditions_);

    return contract;
}
}  // namespace opentxs::contract::peer::reply::implementation
