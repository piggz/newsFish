#include "feedsmodel.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <qsqldatabase.h>
#include <qsqlquery.h>

FeedsModel::Feed FeedsModel::Feed::fromJson(const QJsonObject &object)
{
    return Feed{
        object[QStringLiteral("id")].toInt(),
        object[QStringLiteral("url")].toString(),
        object[QStringLiteral("title")].toString(),
        object[QStringLiteral("faviconLink")].toString(),
        QDateTime::fromSecsSinceEpoch(object[QStringLiteral("added")].toInt()),
        object[QStringLiteral("folderId")].toInt(),
        object[QStringLiteral("unreadCount")].toInt(),
        object[QStringLiteral("ordering")].toInt(),
        object[QStringLiteral("link")].toString(),
        object[QStringLiteral("pinned")].toBool(),
        object[QStringLiteral("updateErrorCount")].toInt(),
        object[QStringLiteral("lastUpdateError")].toString(),
    };
}

FeedsModel::Feed FeedsModel::Feed::fromQuery(const QSqlQuery &query)
{
    return Feed{
        query.value(0).toInt(),
        query.value(1).toString(),
        query.value(2).toString(),
        query.value(3).toString(),
        QDateTime::fromSecsSinceEpoch(query.value(4).toInt()),
        query.value(5).toInt(),
        query.value(6).toInt(),
        query.value(7).toInt(),
        query.value(8).toString(),
        query.value(9).toBool(),
        query.value(10).toInt(),
        query.value(11).toString(),
    };
}

FeedsModel::FeedsModel(const QSqlDatabase &db, QObject *parent)
    : QAbstractListModel(parent)
    , m_db(db)
{
    if (!m_db.open()) {
        qWarning() << Q_FUNC_INFO << "DB is not open:" << m_db.lastError();
        return;
    }

    QSqlQuery qry;

    qry.prepare(QStringLiteral(R"RAW(
        CREATE TABLE IF NOT EXISTS feeds (
            id INTEGER UNIQUE PRIMARY KEY,
            feed_url VARCHAR(2048),
            title VARCHAR(1024),
            favicon_link VARCHAR(2048),
            added INTEGER,
            folder_id INTEGER,
            unread_count INTEGER,
            ordering INTEGER,
            link VARCHAR(2048),
            pinned INTEGER,
            update_error_count INTEGER,
            last_update_error VARCHAR(1024)
        ))RAW"));

    if (!qry.exec()) {
        qWarning() << Q_FUNC_INFO << "Error trying to create feeds table" << qry.lastError();
        return;
    }

    loadData();
}

QHash<int, QByteArray> FeedsModel::roleNames() const
{
    return {
        {IdRole, "feedId"},
        {FeedUrlRole, "feedUrl"},
        {TitleRole, "title"},
        {FaviconLinkRole, "faviconLink"},
        {AddedRole, "added"},
        {FolderIdRole, "folderId"},
        {UnreadCountRole, "unreadCount"},
        {OrderingRole, "ordering"},
        {LinkRole, "link"},
        {PinnedRole, "pinned"},
        {UpdateErrorCountRole, "updateErrorCount"},
        {LastUpdateErrorRole, "lastUpdateError"},
    };
}

QList<int> FeedsModel::feedIds() const
{
    QList<int> ids;

    for (const auto &feed : std::as_const(m_feeds)) {
        ids << feed.id;
    }

    return ids;
}

void FeedsModel::loadData()
{
    if (!m_db.isOpen()) {
        qWarning() << Q_FUNC_INFO << "DB is not open:" << m_db.lastError();
        return;
    }

    qDebug() << "Loading feed data from cache";

    QSqlQuery query(m_db);

    if (!query.exec(QStringLiteral("SELECT id, feed_url, title, favicon_link, added, folder_id, unread_count, ordering, link, pinned, update_error_count, "
                                   "last_update_error FROM feeds"))) {
        qWarning() << Q_FUNC_INFO << "Error trying to fetch data from cache" << query.lastError() << query.lastQuery();
        return;
    }

    beginResetModel();
    while (query.next()) {
        m_feeds << Feed::fromQuery(query);
    }
    endResetModel();
}

void FeedsModel::checkFeeds(QList<int> feeds)
{
    if (!m_db.isOpen()) {
        qWarning() << Q_FUNC_INFO << "DB is not open:" << m_db.lastError();
        return;
    }

    QList<int> feedsToDelete;

    QSqlQuery qry(m_db);
    qry.prepare(QStringLiteral("SELECT id FROM feeds"));

    bool ret = qry.exec();
    if (!ret) {
        qDebug() << qry.lastError();
    } else {
        while (qry.next()) {
            // Loop over feeds in the database and check they are in the current list
            if (!feeds.contains(qry.value(0).toInt())) {
                feedsToDelete << qry.value(0).toInt();
            }
        }
        for (int feed : std::as_const(feedsToDelete)) {
            QSqlQuery delQuery(m_db);
            qry.prepare(QStringLiteral("DELETE FROM items where feedid = :id"));
            qry.bindValue(QStringLiteral(":id"), feed);
            qry.exec();
            qry.prepare(QStringLiteral("DELETE FROM feeds where id = :id"));
            qry.bindValue(QStringLiteral(":id"), feed);
            qry.exec();
        }
    }
}

QVariant FeedsModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &item = m_feeds.at(index.row());

    switch (role) {
    case IdRole:
        return item.id;
    case FeedUrlRole:
        return item.feedUrl;
    case TitleRole:
        return item.title;
    case FaviconLinkRole:
        return item.faviconLink;
    case AddedRole:
        return item.added;
    case FolderIdRole:
        return item.folderId;
    case UnreadCountRole:
        return item.unreadCount;
    case OrderingRole:
        return item.ordering;
    case LinkRole:
        return item.link;
    case PinnedRole:
        return item.pinned;
    case UpdateErrorCountRole:
        return item.updateErrorCount;
    case LastUpdateErrorRole:
        return item.lastUpdateError;
    }

    return {};
}

int FeedsModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_feeds.count();
}

void FeedsModel::parseFeeds(const QByteArray &json)
{
    const auto data = QJsonDocument::fromJson(json);

    const auto feeds = data.object()[QStringLiteral("feeds")].toArray();
    QList<int> feedIds;

    qDebug() << "Feed Count" << feeds.size();

    beginResetModel();

    m_feeds.clear();
    m_db.transaction();

    for (const auto &feed : feeds) {
        const auto object = feed.toObject();
        const auto feedObject = Feed::fromJson(object);
        addFeed(feedObject);
        feedIds << feedObject.id;
    }

    checkFeeds(feedIds);
    endResetModel();
    m_db.commit();
}

void FeedsModel::addFeed(const Feed &feed)
{
    m_feeds << feed;

    if (!m_db.isOpen()) {
        qWarning() << Q_FUNC_INFO << "DB is not open:" << m_db.lastError();
        return;
    }

    QSqlQuery qry(m_db);
    qry.prepare(QStringLiteral(R"RAW(
        INSERT OR REPLACE INTO feeds(id, feed_url, title, favicon_link, added, folder_id, unread_count, ordering, link, pinned, update_error_count, last_update_error)
        VALUES(:id, :feed_url, :title, :favicon_link, :added, :folder_id, :unread_count, :ordering, :link, :pinned, :update_error_count, :last_update_error))RAW"));
    qry.bindValue(QStringLiteral(":id"), feed.id);
    qry.bindValue(QStringLiteral(":feed_url"), feed.feedUrl);
    qry.bindValue(QStringLiteral(":title"), feed.title);
    qry.bindValue(QStringLiteral(":favicon_link"), feed.faviconLink);
    qry.bindValue(QStringLiteral(":added"), feed.added.toSecsSinceEpoch());
    qry.bindValue(QStringLiteral(":folder_id"), feed.folderId);
    qry.bindValue(QStringLiteral(":unread_count"), feed.unreadCount);
    qry.bindValue(QStringLiteral(":ordering"), feed.ordering);
    qry.bindValue(QStringLiteral(":link"), feed.link);
    qry.bindValue(QStringLiteral(":pinned"), feed.pinned);
    qry.bindValue(QStringLiteral(":update_error_count"), feed.updateErrorCount);
    qry.bindValue(QStringLiteral(":last_update_error"), feed.lastUpdateError);
}
