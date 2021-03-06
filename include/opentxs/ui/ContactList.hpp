// Copyright (c) 2010-2021 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_UI_CONTACTLIST_HPP
#define OPENTXS_UI_CONTACTLIST_HPP

#include "opentxs/Forward.hpp"  // IWYU pragma: associated

#include "opentxs/SharedPimpl.hpp"
#include "opentxs/ui/List.hpp"

#ifdef SWIG
// clang-format off
%rename(UIContactList) opentxs::ui::ContactList;
// clang-format on
#endif  // SWIG

namespace opentxs
{
namespace ui
{
class ContactList;
class ContactListItem;
}  // namespace ui
}  // namespace opentxs

namespace opentxs
{
namespace ui
{
class ContactList : virtual public List
{
public:
    OPENTXS_EXPORT virtual std::string AddContact(
        const std::string& label,
        const std::string& paymentCode = "",
        const std::string& nymID = "") const noexcept = 0;
    OPENTXS_EXPORT virtual opentxs::SharedPimpl<opentxs::ui::ContactListItem>
    First() const noexcept = 0;
    OPENTXS_EXPORT virtual opentxs::SharedPimpl<opentxs::ui::ContactListItem>
    Next() const noexcept = 0;

    OPENTXS_EXPORT ~ContactList() override = default;

protected:
    ContactList() noexcept = default;

private:
    ContactList(const ContactList&) = delete;
    ContactList(ContactList&&) = delete;
    ContactList& operator=(const ContactList&) = delete;
    ContactList& operator=(ContactList&&) = delete;
};
}  // namespace ui
}  // namespace opentxs
#endif
