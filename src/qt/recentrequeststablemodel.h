// Copyright (c) 2011-2015 The Bitcoin Core developers 
// Copyright (c) 2017 IoP Ventures LLC

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef IoP_QT_RECENTREQUESTSTABLEMODEL_H
#define IoP_QT_RECENTREQUESTSTABLEMODEL_H

#include "walletmodel.h"

#include <QAbstractTableModel>
#include <QStringList>
#include <QDateTime>

class CWallet;

class RecentRequestEntry
{
public:
    RecentRequestEntry() : nVersion(RecentRequestEntry::CURRENT_VERSION), id(0) { }

    static const int CURRENT_VERSION = 1;
    int nVersion;
    int64_t id;
    QDateTime date;
    SendCoinsRecipient recipient;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        unsigned int nDate = date.toTime_t();

        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(id);
        READWRITE(nDate);
        READWRITE(recipient);

        if (ser_action.ForRead())
            date = QDateTime::fromTime_t(nDate);
    }
};

class RecentRequestEntryLessThan
{
public:
    RecentRequestEntryLessThan(int nColumn, Qt::SortOrder fOrder):
        column(nColumn), order(fOrder) {}
    bool operator()(RecentRequestEntry &left, RecentRequestEntry &right) const;

private:
    int column;
    Qt::SortOrder order;
};

/** Model for list of recently generated payment requests / IoP: URIs.
 * Part of wallet model.
 */
class RecentRequestsTableModel: public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit RecentRequestsTableModel(CWallet *wallet, WalletModel *parent);
    ~RecentRequestsTableModel();

    enum ColumnIndex {
        Date = 0,
        Label = 1,
        Message = 2,
        Amount = 3,
        NUMBER_OF_COLUMNS
    };

    /** @name Methods overridden from QAbstractTableModel
        @{*/
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    Qt::ItemFlags flags(const QModelIndex &index) const;
    /*@}*/

    const RecentRequestEntry &entry(int row) const { return list[row]; }
    void addNewRequest(const SendCoinsRecipient &recipient);
    void addNewRequest(const std::string &recipient);
    void addNewRequest(RecentRequestEntry &recipient);

public Q_SLOTS:
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    void updateDisplayUnit();

private:
    WalletModel *walletModel;
    QStringList columns;
    QList<RecentRequestEntry> list;
    int64_t nReceiveRequestsMaxId;

    /** Updates the column title to "Amount (DisplayUnit)" and emits headerDataChanged() signal for table headers to react. */
    void updateAmountColumnTitle();
    /** Gets title for amount column including current display unit if optionsModel reference available. */
    QString getAmountTitle();
};

#endif // IoP_QT_RECENTREQUESTSTABLEMODEL_H
