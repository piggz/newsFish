#include "itemsmodel.h"
#include "itemworker.h"

#include <QDateTime>
#include <QDebug>
#include <QRegExp>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextDocument>
#include <QThread>
#include <qstringliteral.h>

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
    case ItemBodyHTML:
        return item.bodyHtml;
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

void ItemsModel::parseItems(const QByteArray &json)
{
    qDebug() << Q_FUNC_INFO;

    m_items.clear();

    auto thread = new QThread;
    auto worker = new ItemWorker(m_db, json);
    worker->moveToThread(thread);
    // connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(thread, &QThread::started, worker, &ItemWorker::process);
    connect(worker, &ItemWorker::finished, thread, &QThread::quit);
    connect(worker, &ItemWorker::finished, worker, &QObject::deleteLater);
    connect(worker, &ItemWorker::finished, this, &ItemsModel::slotWorkerFinished);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

void ItemsModel::slotWorkerFinished()
{
    Q_EMIT feedParseComplete();
}

void ItemsModel::setDatabase(const QString &dbname)
{
}

void ItemsModel::setFeed(int feedId)
{
    qDebug() << Q_FUNC_INFO << m_db.isOpen();

    if (m_db.isOpen() || m_db.open()) {
        QSqlQuery qry;
        qry.prepare(QStringLiteral(
            "SELECT id, feedid, title, guid, guidhash, body, link, author, pubdate, unread, starred FROM items WHERE feedid = :fid ORDER BY pubdate DESC"));
        qry.bindValue(QStringLiteral(":fid"), feedId);

        qDebug() << feedId;
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

                txt.setHtml(qry.value(5).toString());
                item.body = txt.toPlainText().trimmed();

                item.bodyHtml = qry.value(5).toString();
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

void ItemsModel::deleteOldData(int days)
{
    qDebug() << "Deleting data older than " << days << " days";

    if (m_db.isOpen()) {
        QSqlQuery qry;

        QDateTime now = QDateTime::currentDateTime();
        now = now.addDays(-days);

        qry.prepare(QStringLiteral("DELETE FROM items WHERE pubdate < :pubdate"));
        qry.bindValue(QStringLiteral(":pubdate"), now.toTime_t());
        qDebug() << now << qry.lastQuery();
        qDebug() << qry.boundValues();

        bool ret = qry.exec();

        if (!ret) {
            qDebug() << qry.lastError();
        } else {
            qDebug() << "Items table cleared of old items!";
        }
    } else {
        qDebug() << "Unable to delete old data:" << m_db.lastError();
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
        {ItemBodyHTML, "itembodyhtml"},
        {ItemLink, "itemlink"},
        {ItemAuthor, "itemauthor"},
        {ItemPubDate, "itempubdate"},
        {ItemUnread, "itemunread"},
        {ItemStarred, "itemstarred"},
    };
}
