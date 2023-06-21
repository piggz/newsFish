#pragma once

#include <QAbstractListModel>
#include <QDateTime>
#include <QList>
#include <QSqlDatabase>
#include <qsqldatabase.h>

class ItemsModel : public QAbstractListModel
{
    Q_OBJECT

    /// The if of feed that this ItemsModel will display
    Q_PROPERTY(int feedId READ feedId WRITE setFeedId NOTIFY feedIdChanged)

public:
    enum Roles {
        ItemId = Qt::UserRole + 1,
        ItemFeedId,
        ItemTitle,
        ItemGUID,
        ItemGUIDHash,
        ItemBody,
        ItemLink,
        ItemAuthor,
        ItemPubDate,
        ItemUnread,
        ItemStarred
    };

    explicit ItemsModel(const QSqlDatabase &db, QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    QHash<int, QByteArray> roleNames() const override;

    int feedId() const;
    void setFeedId(int feedId);

    void parseItems(const QString &feedId, const QByteArray &json);
    void setDatabase(const QString &dbname);
    void recreateTable();
    void setItemRead(int itemId, bool read);

Q_SIGNALS:
    void feedParseComplete();
    void feedIdChanged();

private Q_SLOTS:
    void slotWorkerFinished();

private:
    struct Item {
        int id;
        int feedId;
        QString title;
        QString guid;
        QString guidHash;
        QString body;
        QString link;
        QString author;
        QDateTime publishDate;
        bool unread;
        bool starred;

        static Item fromJson(const QJsonObject &object);
        void cacheInDatabase(QSqlDatabase database) const;
    };

    QList<Item> m_items;
    int m_feedId;

    QSqlDatabase m_db;
};

Q_DECLARE_METATYPE(ItemsModel *);
