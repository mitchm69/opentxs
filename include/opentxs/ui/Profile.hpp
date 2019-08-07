// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_UI_PROFILE_HPP
#define OPENTXS_UI_PROFILE_HPP

#include "opentxs/Forward.hpp"

#include "opentxs/ui/List.hpp"
#include "opentxs/Proto.hpp"

#include <string>
#include <tuple>
#include <vector>

#ifdef SWIG
#include <algorithm>
#include <tuple>
#include <vector>

// clang-format off
%extend opentxs::ui::Profile {
    bool AddClaim(
        const int section,
        const int type,
        const std::string& value,
        const bool primary,
        const bool active) const
    {
        return $self->AddClaim(
            static_cast<opentxs::proto::ContactSectionName>(section),
            static_cast<opentxs::proto::ContactItemType>(type),
            value,
            primary,
            active);
    }
    std::vector<std::pair<int, std::string>> AllowedItems(
        const int section,
        const std::string& lang)
    {
        const auto types = opentxs::ui::ProfileSection::AllowedItems(
            static_cast<opentxs::proto::ContactSectionName>(section),
            lang);
        std::vector<std::pair<int, std::string>> output;
        std::transform(
            types.begin(),
            types.end(),
            std::inserter(output, output.end()),
            [](std::pair<opentxs::proto::ContactItemType, std::string> type) ->
                std::pair<int, std::string> {
                    return {static_cast<int>(type.first), type.second};} );

        return output;
    }
    std::vector<std::pair<int, std::string>> AllowedSections(
        const std::string& lang)
    {
        const auto sections = $self->AllowedSections(lang);
        std::vector<std::pair<int, std::string>> output;
        std::transform(
            sections.begin(),
            sections.end(),
            std::inserter(output, output.end()),
            [](std::pair<opentxs::proto::ContactSectionName, std::string> type) ->
                std::pair<int, std::string> {
                    return {static_cast<int>(type.first), type.second};} );

        return output;
    }
}
%ignore opentxs::ui::Profile::AddClaim;
%ignore opentxs::ui::Profile::AllowedItems;
%ignore opentxs::ui::Profile::AllowedSections;
%rename(UIProfile) opentxs::ui::Profile;
// clang-format on
#endif  // SWIG

namespace opentxs
{
namespace ui
{
namespace implementation
{
class Profile;
}  // namespace implementation

class Profile : virtual public List
{
public:
    using ItemType = std::pair<proto::ContactItemType, std::string>;
    using ItemTypeList = std::vector<ItemType>;
    using SectionType = std::pair<proto::ContactSectionName, std::string>;
    using SectionTypeList = std::vector<SectionType>;

    EXPORT virtual bool AddClaim(
        const proto::ContactSectionName section,
        const proto::ContactItemType type,
        const std::string& value,
        const bool primary,
        const bool active) const = 0;
    EXPORT virtual ItemTypeList AllowedItems(
        const proto::ContactSectionName section,
        const std::string& lang) const = 0;
    EXPORT virtual SectionTypeList AllowedSections(
        const std::string& lang) const = 0;
    EXPORT virtual bool Delete(
        const int section,
        const int type,
        const std::string& claimID) const = 0;
    EXPORT virtual std::string DisplayName() const = 0;
    EXPORT virtual opentxs::SharedPimpl<opentxs::ui::ProfileSection> First()
        const = 0;
    EXPORT virtual std::string ID() const = 0;
    EXPORT virtual opentxs::SharedPimpl<opentxs::ui::ProfileSection> Next()
        const = 0;
    EXPORT virtual std::string PaymentCode() const = 0;
    EXPORT virtual bool SetActive(
        const int section,
        const int type,
        const std::string& claimID,
        const bool active) const = 0;
    EXPORT virtual bool SetPrimary(
        const int section,
        const int type,
        const std::string& claimID,
        const bool primary) const = 0;
    EXPORT virtual bool SetValue(
        const int section,
        const int type,
        const std::string& claimID,
        const std::string& value) const = 0;

    EXPORT virtual ~Profile() = default;

protected:
    Profile() = default;

private:
    Profile(const Profile&) = delete;
    Profile(Profile&&) = delete;
    Profile& operator=(const Profile&) = delete;
    Profile& operator=(Profile&&) = delete;
};

#if OT_QT
class ProfileQt : public QAbstractItemModel
{
public:
    using ConstructorCallback = std::function<
        implementation::Profile*(RowCallbacks insert, RowCallbacks remove)>;

    enum Roles {};

    QString displayName() const;
    QString nymID() const;
    QString paymentCode() const;

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole)
        const override;
    QModelIndex index(
        int row,
        int column,
        const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    const Profile& operator*() const;

    ProfileQt(ConstructorCallback cb);
    ~ProfileQt() override;

signals:
    void updated() const;

private:
    Q_OBJECT
    Q_PROPERTY(QString displayName READ displayName NOTIFY updated)
    Q_PROPERTY(QString nymID READ nymID NOTIFY updated)
    Q_PROPERTY(QString paymentCode READ paymentCode NOTIFY updated)

    std::unique_ptr<implementation::Profile> parent_;

    void notify() const;
    void finish_row_add();
    void finish_row_delete();
    void start_row_add(const QModelIndex& parent, int first, int last);
    void start_row_delete(const QModelIndex& parent, int first, int last);

    ProfileQt() = delete;
    ProfileQt(const ProfileQt&) = delete;
    ProfileQt(ProfileQt&&) = delete;
    ProfileQt& operator=(const ProfileQt&) = delete;
    ProfileQt& operator=(ProfileQt&&) = delete;
};
#endif
}  // namespace ui
}  // namespace opentxs
#endif
