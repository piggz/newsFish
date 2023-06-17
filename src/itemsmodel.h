#pragma once

#include <QAbstractListModel>
#include <QDateTime>
#include <QList>
#include <QSqlDatabase>

class ItemsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        ItemId = Qt::UserRole + 1,
        ItemFeedId,
        ItemTitle,
        ItemGUID,
        ItemGUIDHash,
        ItemBody,
        ItemBodyHTML,
        ItemLink,
        ItemAuthor,
        ItemPubDate,
        ItemUnread,
        ItemStarred
    };

    explicit ItemsModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    QHash<int, QByteArray> roleNames() const override;

    void parseItems(const QByteArray &json);
    void setDatabase(const QString &dbname);
    void setFeed(int feedId);
    void recreateTable();
    void deleteOldData(int days);

Q_SIGNALS:
    void feedParseComplete();

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
        QString bodyHtml;
        QString link;
        QString author;
        QDateTime publishDate;
        bool unread;
        bool starred;
    };

    QList<Item> m_items;

    QSqlDatabase m_db;
    QString m_databaseName;
};

Q_DECLARE_METATYPE(ItemsModel *);
