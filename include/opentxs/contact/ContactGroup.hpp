// Copyright (c) 2010-2021 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CONTACT_CONTACTGROUP_HPP
#define OPENTXS_CONTACT_CONTACTGROUP_HPP

// IWYU pragma: no_include "opentxs/Proto.hpp"

#include "opentxs/Forward.hpp"  // IWYU pragma: associated

#include <iosfwd>
#include <map>
#include <memory>
#include <string>

#include "opentxs/Proto.hpp"
#include "opentxs/core/Identifier.hpp"
#include "opentxs/protobuf/ContactEnums.pb.h"

namespace opentxs
{
namespace proto
{
class ContactSection;
}  // namespace proto

class ContactItem;
}  // namespace opentxs

namespace opentxs
{
class ContactGroup
{
public:
    using ItemMap = std::map<OTIdentifier, std::shared_ptr<ContactItem>>;

    OPENTXS_EXPORT ContactGroup(
        const std::string& nym,
        const proto::ContactSectionName section,
        const proto::ContactItemType type,
        const ItemMap& items);
    OPENTXS_EXPORT ContactGroup(
        const std::string& nym,
        const proto::ContactSectionName section,
        const std::shared_ptr<ContactItem>& item);
    OPENTXS_EXPORT ContactGroup(const ContactGroup&) = default;
    OPENTXS_EXPORT ContactGroup(ContactGroup&&) = default;

    OPENTXS_EXPORT ContactGroup operator+(const ContactGroup& rhs) const;

    OPENTXS_EXPORT ItemMap::const_iterator begin() const;
    OPENTXS_EXPORT std::shared_ptr<ContactItem> Best() const;
    OPENTXS_EXPORT std::shared_ptr<ContactItem> Claim(
        const Identifier& item) const;
    OPENTXS_EXPORT bool HaveClaim(const Identifier& item) const;
    OPENTXS_EXPORT ContactGroup
    AddItem(const std::shared_ptr<ContactItem>& item) const;
    OPENTXS_EXPORT ContactGroup
    AddPrimary(const std::shared_ptr<ContactItem>& item) const;
    OPENTXS_EXPORT ContactGroup Delete(const Identifier& id) const;
    OPENTXS_EXPORT ItemMap::const_iterator end() const;
    OPENTXS_EXPORT const Identifier& Primary() const;
    OPENTXS_EXPORT std::shared_ptr<ContactItem> PrimaryClaim() const;
    OPENTXS_EXPORT bool SerializeTo(
        proto::ContactSection& section,
        const bool withIDs = false) const;
    OPENTXS_EXPORT std::size_t Size() const;
    OPENTXS_EXPORT const proto::ContactItemType& Type() const;

    OPENTXS_EXPORT ~ContactGroup() = default;

private:
    const std::string nym_{};
    const proto::ContactSectionName section_{proto::CONTACTSECTION_ERROR};
    const proto::ContactItemType type_{proto::CITEMTYPE_ERROR};
    const OTIdentifier primary_;
    const ItemMap items_{};

    static ItemMap create_item(const std::shared_ptr<ContactItem>& item);
    static OTIdentifier get_primary_item(const ItemMap& items);
    static ItemMap normalize_items(const ItemMap& items);

    ContactGroup() = delete;
    ContactGroup& operator=(const ContactGroup&) = delete;
    ContactGroup& operator=(ContactGroup&&) = delete;
};
}  // namespace opentxs
#endif
