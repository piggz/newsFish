#include "itemsmodel.h"

#include <QDateTime>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegExp>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextDocument>

ItemsModel::Item ItemsModel::Item::fromJson(const QJsonObject &object)
{
    QTextDocument textDocument;
    textDocument.setHtml(object[QStringLiteral("body")].toString());

    return {
        object[QStringLiteral("id")].toInt(),
        object[QStringLiteral("feedId")].toInt(),
        object[QStringLiteral("title")].toString(),
        object[QStringLiteral("guid")].toString(),
        object[QStringLiteral("guidHash")].toString(),
        object[QStringLiteral("body")].toString(),
        object[QStringLiteral("url")].toString(),
        object[QStringLiteral("author")].toString(),
        QDateTime::fromSecsSinceEpoch(object[QStringLiteral("pubDate")].toInt()),
        object[QStringLiteral("unread")].toBool(),
        object[QStringLiteral("starred")].toBool(),
    };
}

void ItemsModel::Item::cacheInDatabase(QSqlDatabase database) const
{
    QSqlQuery qry(database);
    qry.prepare(
        QStringLiteral("INSERT OR REPLACE INTO items(id, feedid, title, body, link, author, pubdate, unread, starred, guid, guidhash) VALUES(:id, :feedid, "
                       ":title, :body, :link, :author, :pubdate, :unread, :starred, :guid, :guidhash)"));
    qry.bindValue(QStringLiteral(":id"), id);
    qry.bindValue(QStringLiteral(":feedid"), feedId);
    qry.bindValue(QStringLiteral(":title"), title);
    qry.bindValue(QStringLiteral(":guid"), guid);
    qry.bindValue(QStringLiteral(":guidhash"), guidHash);
    qry.bindValue(QStringLiteral(":body"), body);
    qry.bindValue(QStringLiteral(":link"), link);
    qry.bindValue(QStringLiteral(":author"), author);
    qry.bindValue(QStringLiteral(":pubdate"), publishDate.toSecsSinceEpoch());
    qry.bindValue(QStringLiteral(":unread"), unread ? 1 : 0);
    qry.bindValue(QStringLiteral(":starred"), starred ? 1 : 0);

    if (!qry.exec()) {
        qWarning() << Q_FUNC_INFO << "Error trying to cache a feed item" << qry.lastError();
    }
}

ItemsModel::ItemsModel(const QSqlDatabase &db, QObject *parent)
    : QAbstractListModel(parent)
{
    m_db = db;
    QSqlQuery qry(db);

    qry.prepare(
        QStringLiteral("CREATE TABLE IF NOT EXISTS items (id INTEGER UNIQUE PRIMARY KEY, \
                 feedid INTEGER, \
                 title VARCHAR(1024), \
                 guid VARCHAR(1024), \
                 guidhash VARCHAR(1024), \
                 body VARCHAR(2048), \
                 link VARCHAR(2048), \
                 author VARCHAR(1024), \
                 pubdate INTEGER, \
                 unread INTEGER, \
                 starred INTEGER)"));

    bool ret = qry.exec();
    if (!ret) {
        qDebug() << qry.lastError();
    } else {
        qDebug() << "Items table created!";
    }
}

QVariant ItemsModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &item = m_items.at(index.row());

    switch (role) {
    case ItemId:
        return item.id;
    case ItemFeedId:
        return item.feedId;
    case ItemTitle:
        return item.title;
    case ItemBody:
        return item.body;
    case ItemLink:
        return item.link;
    case ItemAuthor:
        return item.author;
    case ItemPubDate:
        return item.publishDate;
    case ItemUnread:
        return item.unread;
    case ItemStarred:
        return item.starred;
    case ItemGUID:
        return item.guid;
    case ItemGUIDHash:
        return item.guidHash;
    default:
        return {};
    }
}

int ItemsModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_items.count();
}

void ItemsModel::parseItems(const QString &feedId, const QByteArray &json)
{
    auto data = QJsonDocument::fromJson(json);
    if (data.isNull() || !data.isObject()) {
        qWarning() << "Got invalid json" << json;
        return;
    }

    QList<Item> feedItems;

    const auto object = data.object();
    const auto items = object[QStringLiteral("items")].toArray();

    if (items.isEmpty()) {
        return;
    }

    m_db.transaction();
    for (const auto &value : items) {
        const auto object = value.toObject();
        const auto item = Item::fromJson(object);
        feedItems << item;
        item.cacheInDatabase(m_db);
    }
    m_db.commit();

    if (feedItems.length() > 15) {
        QSqlQuery query(m_db);
        query.prepare(QStringLiteral("DELETE FROM items WHERE pubdate < :pubdate AND feedid = :feedid"));
        query.bindValue(QStringLiteral(":pubdate"), feedItems[14].publishDate.toTime_t());
        query.bindValue(QStringLiteral(":feedid"), feedId);

        if (!query.exec()) {
            qWarning() << Q_FUNC_INFO << "Error trying to delete old items of the feed item" << query.lastError();
        }
    }

    if (m_feedId == feedId) {
        beginResetModel();
        m_items = feedItems;
        endResetModel();
    }
}

void ItemsModel::slotWorkerFinished()
{
    Q_EMIT feedParseComplete();
}

int ItemsModel::feedId() const
{
    return m_feedId;
}

void ItemsModel::setFeedId(int feedId)
{
    if (m_feedId == feedId) {
        return;
    }

    m_feedId = feedId;

    qDebug() << Q_FUNC_INFO << m_db.isOpen();

    if (m_db.isOpen() || m_db.open()) {
        QSqlQuery qry;
        qry.prepare(QStringLiteral(
            "SELECT id, feedid, title, guid, guidhash, body, link, author, pubdate, unread, starred FROM items WHERE feedid = :fid ORDER BY pubdate DESC"));
        qry.bindValue(QStringLiteral(":fid"), feedId);

        bool ret = qry.exec();
        if (!ret) {
            qDebug() << qry.lastError();
        } else {
            beginResetModel();
            m_items.clear();

            QTextDocument txt;

            while (qry.next()) {
                Item item;
                item.id = qry.value(0).toInt();
                item.feedId = qry.value(1).toInt();

                txt.setHtml(qry.value(2).toString());
                item.title = txt.toPlainText().trimmed();

                item.guid = qry.value(3).toString();
                item.guidHash = qry.value(4).toString();

                item.body = qry.value(5).toString();
                item.link = qry.value(6).toString();
                item.author = qry.value(7).toString();
                item.publishDate = QDateTime::fromTime_t(qry.value(8).toUInt());
                item.unread = qry.value(9).toBool();
                item.starred = qry.value(10).toBool();

                m_items << item;
            }
            endResetModel();
        }

    } else {
        qDebug() << "Unable to open database:" << m_db.lastError();
    }

    Q_EMIT feedIdChanged();
    // qDebug() << m_items;
}

void ItemsModel::recreateTable()
{
    if (!m_db.isOpen()) {
        return;
    }
    QSqlQuery qry;

    qry.prepare(QStringLiteral("DROP TABLE items"));
    bool ret = qry.exec();

    qry.prepare(QStringLiteral(R"RAW(
        CREATE TABLE IF NOT EXISTS items (
            id INTEGER UNIQUE PRIMARY KEY,
            feedid INTEGER,
            title VARCHAR(1024),
            guid VARCHAR(1024),
            guidhash VARCHAR(1024),
            body TEXT,
            link VARCHAR(2048),
            author VARCHAR(1024),
            pubdate INTEGER,
            unread INTEGER,
            starred INTEGER
        )
    )RAW"));
    ret = qry.exec();

    if (!ret) {
        qDebug() << qry.lastError();
    } else {
        qDebug() << "Items table created!";
    }
}

void ItemsModel::setItemRead(int itemId, bool read)
{
    QSqlQuery query(m_db);
    query.prepare(QStringLiteral("UPDATE items SET unread = :unread WHERE id = :itemid"));
    query.bindValue(QStringLiteral(":unread"), read ? 0 : 1);
    query.bindValue(QStringLiteral(":itemid"), itemId);
    if (!query.exec()) {
        qWarning() << query.lastError();
    }

    for (int i = 0, count = m_items.count(); i < count; i++) {
        if (m_items[i].id == itemId) {
            m_items[i].unread = !read;
            Q_EMIT dataChanged(index(i, 0), index(i, 0), {ItemUnread});

            return;
        }
    }
}

QHash<int, QByteArray> ItemsModel::roleNames() const
{
    return {
        {ItemId, "itemid"},
        {ItemFeedId, "itemfeedid"},
        {ItemTitle, "itemtitle"},
        {ItemGUID, "itemguid"},
        {ItemGUIDHash, "itemguidhash"},
        {ItemBody, "itembody"},
        {ItemLink, "itemlink"},
        {ItemAuthor, "itemauthor"},
        {ItemPubDate, "itempubdate"},
        {ItemUnread, "itemunread"},
        {ItemStarred, "itemstarred"},
    };
}
