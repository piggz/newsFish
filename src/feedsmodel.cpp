#include "feedsmodel.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

FeedsModel::FeedsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QHash<int, QByteArray> FeedsModel::roleNames() const
{
    return {
        {FeedId, "feedid"},
        {FeedTitle, "feedtitle"},
        {FeedURL, "feedurl"},
        {FeedIcon, "feedicon"},
    };
}

QList<int> FeedsModel::feedIds() const
{
    QList<int> ids;

    for (const QVariantMap &feed : std::as_const(m_feeds)) {
        ids << feed[QStringLiteral("id")].toInt();
    }

    return ids;
}

void FeedsModel::addFeed(int id, const QString &title, const QString &url, const QString &icon)
{
    if (m_db.isOpen()) {
        QSqlQuery qry(m_db);
        qry.prepare(QStringLiteral("INSERT OR REPLACE INTO feeds(id, title, url, icon) VALUES(:id, :title, :url, :icon)"));
        qry.bindValue(QStringLiteral(":id"), id);
        qry.bindValue(QStringLiteral(":title"), title);
        qry.bindValue(QStringLiteral(":url"), url);
        qry.bindValue(QStringLiteral(":icon"), icon);

        bool ret = qry.exec();
        if (!ret)
            qDebug() << qry.lastError();
        else {
            qDebug() << "feed inserted!";
            QVariantMap feed;
            feed[QStringLiteral("id")] = id;
            feed[QStringLiteral("title")] = title;
            feed[QStringLiteral("url")] = url;
            feed[QStringLiteral("icon")] = icon;
            m_feeds << feed;
        }
    } else {
        qDebug() << "Unable to add feed:" << m_db.lastError();
    }
}

void FeedsModel::loadData()
{
    if (m_db.isOpen()) {
        qDebug() << "Loading feed data";
        QSqlQuery qry(m_db);
        qry.prepare(QStringLiteral("SELECT id, title, url, icon FROM feeds"));

        bool ret = qry.exec();
        if (!ret) {
            qDebug() << qry.lastError();
        } else {
            beginResetModel();
            while (qry.next()) {
                QVariantMap feed;
                feed[QStringLiteral("id")] = qry.value(0).toInt();
                feed[QStringLiteral("title")] = qry.value(1).toString();
                feed[QStringLiteral("url")] = qry.value(2).toString();
                feed[QStringLiteral("icon")] = qry.value(3).toString();
                m_feeds << feed;
            }
            endResetModel();
        }
    }
}

void FeedsModel::checkFeeds(QList<int> feeds)
{
    QList<int> feedsToDelete;
    if (m_db.isOpen()) {
        qDebug() << "Loading feed data";
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
}

QVariant FeedsModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &item = m_feeds.at(index.row());

    switch (role) {
    case FeedId:
        return item[QStringLiteral("id")];
    case FeedTitle:
        return item[QStringLiteral("title")];
    case FeedURL:
        return item[QStringLiteral("url")];
    case FeedIcon:
        return item[QStringLiteral("icon")];
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
    for (const auto &feed : feeds) {
        const auto map = feed.toObject();
        addFeed(map[QStringLiteral("id")].toInt(),
                map[QStringLiteral("title")].toString(),
                map[QStringLiteral("url")].toString(),
                map[QStringLiteral("faviconLink")].toString());
        feedIds << map[QStringLiteral("id")].toInt();
    }

    checkFeeds(feedIds);
    loadData();
    endResetModel();
}

void FeedsModel::setDatabase(const QString &database)
{
    m_databaseName = database;
    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("feed_connection"));
    m_db.setDatabaseName(m_databaseName);

    if (m_db.open()) {
        QSqlQuery qry;

        qry.prepare(
            QStringLiteral("CREATE TABLE IF NOT EXISTS feeds (id INTEGER UNIQUE PRIMARY KEY, title VARCHAR(1024), url VARCHAR(2048), icon VARCHAR(2048))"));
        bool ret = qry.exec();
        if (!ret) {
            qDebug() << qry.lastError();
        } else {
            qDebug() << "Feed table created!";
        }

        loadData();
    }
}
