// Copyright (c) 2010-2020 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "0_stdafx.hpp"                            // IWYU pragma: associated
#include "1_Internal.hpp"                          // IWYU pragma: associated
#include "ui/accountactivity/AccountActivity.hpp"  // IWYU pragma: associated

#include <utility>

#include "opentxs/Pimpl.hpp"

// #define OT_METHOD "opentxs::ui::implementation::AccountActivity::"

namespace opentxs::factory
{
#if OT_QT
auto AccountActivityQtModel(
    ui::implementation::AccountActivity& parent) noexcept
    -> std::unique_ptr<ui::AccountActivityQt>
{
    using ReturnType = ui::AccountActivityQt;

    return std::make_unique<ReturnType>(parent);
}
#endif  // OT_QT
}  // namespace opentxs::factory

#if OT_QT
namespace opentxs::ui
{
QT_PROXY_MODEL_WRAPPER_EXTRA(AccountActivityQt, implementation::AccountActivity)

auto AccountActivityQt::accountID() const noexcept -> QString
{
    return parent_.AccountID().c_str();
}
auto AccountActivityQt::balancePolarity() const noexcept -> int
{
    return parent_.BalancePolarity();
}
#if OT_BLOCKCHAIN
auto AccountActivityQt::depositChains() const noexcept -> QList<int>
{
    const auto input = parent_.DepositChains();
    auto output = QList<int>{};
    std::transform(
        std::begin(input), std::end(input), std::back_inserter(output), [
        ](const auto& in) -> auto { return static_cast<int>(in); });

    return output;
}
#endif  // OT_BLOCKCHAIN
auto AccountActivityQt::displayBalance() const noexcept -> QString
{
    return parent_.DisplayBalance().c_str();
}
#if OT_BLOCKCHAIN
auto AccountActivityQt::getDepositAddress(const int chain) const noexcept
    -> QString
{
    return parent_.DepositAddress(static_cast<blockchain::Type>(chain)).c_str();
}
#endif  // OT_BLOCKCHAIN
auto AccountActivityQt::init() noexcept -> void
{
#if OT_BLOCKCHAIN
    parent_.SetSyncCallback([this](int current, int max, double percent) {
        emit syncPercentageUpdated(percent);
        emit syncProgressUpdated(current, max);
    });
#endif  // OT_BLOCKCHAIN
}
#if OT_BLOCKCHAIN
auto AccountActivityQt::sendToAddress(
    const QString& address,
    const QString& amount,
    const QString& memo) const noexcept -> bool
{
    return parent_.Send(
        address.toStdString(), amount.toStdString(), memo.toStdString());
}
#endif  // OT_BLOCKCHAIN
auto AccountActivityQt::sendToContact(
    const QString& contactID,
    const QString& amount,
    const QString& memo) const noexcept -> bool
{
    return parent_.Send(
        parent_.api_.Factory().Identifier(contactID.toStdString()),
        amount.toStdString(),
        memo.toStdString());
}
#if OT_BLOCKCHAIN
auto AccountActivityQt::syncPercentage() const noexcept -> double
{
    return parent_.SyncPercentage();
}
auto AccountActivityQt::validateAddress(const QString& address) const noexcept
    -> bool
{
    return parent_.ValidateAddress(address.toStdString());
}
#endif  // OT_BLOCKCHAIN
auto AccountActivityQt::validateAmount(const QString& amount) const noexcept
    -> QString
{
    return parent_.ValidateAmount(amount.toStdString()).c_str();
}
}  // namespace opentxs::ui
#endif

namespace opentxs::ui::implementation
{
AccountActivity::AccountActivity(
    const api::client::internal::Manager& api,
    const identifier::Nym& nymID,
    const Identifier& accountID,
    const AccountType type,
    const SimpleCallback& cb,
    ListenerDefinitions&& listeners) noexcept
    : AccountActivityList(
          api,
          nymID,
          cb,
          true
#if OT_QT
          ,
          Roles{
              {AccountActivityQt::PolarityRole, "polarity"},
              {AccountActivityQt::ContactsRole, "contacts"},
              {AccountActivityQt::WorkflowRole, "workflow"},
              {AccountActivityQt::TypeRole, "type"},
          },
          5
#endif  // OT_QT
          )
    , listeners_(std::move(listeners))
    , balance_(0)
    , account_id_(accountID)
    , type_(type)
{
    init();
    setup_listeners(listeners_);
}

auto AccountActivity::construct_row(
    const AccountActivityRowID& id,
    const AccountActivitySortKey& index,
    CustomData& custom) const noexcept -> RowPointer
{
    return factory::BalanceItem(
        *this, api_, id, index, custom, primary_id_, account_id_);
}

AccountActivity::~AccountActivity()
{
    for (auto& it : listeners_) { delete it.second; }
}
}  // namespace opentxs::ui::implementation