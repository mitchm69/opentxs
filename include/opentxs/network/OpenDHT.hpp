// Copyright (c) 2010-2021 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_NETWORK_OPENDHT_HPP
#define OPENTXS_NETWORK_OPENDHT_HPP

#include "opentxs/Forward.hpp"  // IWYU pragma: associated

#include <string>

#include "opentxs/Types.hpp"

namespace opentxs
{
namespace network
{
class OpenDHT
{
public:
    OPENTXS_EXPORT virtual void Insert(
        const std::string& key,
        const std::string& value,
        DhtDoneCallback cb = {}) const noexcept = 0;
    OPENTXS_EXPORT virtual void Retrieve(
        const std::string& key,
        DhtResultsCallback vcb,
        DhtDoneCallback dcb = {}) const noexcept = 0;

    virtual ~OpenDHT() = default;

protected:
    OpenDHT() = default;

private:
    OpenDHT(const OpenDHT&) = delete;
    OpenDHT(OpenDHT&&) = delete;
    OpenDHT& operator=(const OpenDHT&) = delete;
    OpenDHT& operator=(OpenDHT&&) = delete;
};
}  // namespace network
}  // namespace opentxs
#endif
