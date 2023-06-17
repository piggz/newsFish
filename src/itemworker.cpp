#include "itemworker.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

ItemWorker::ItemWorker(const QByteArray &json, QObject *parent)
    : QObject(parent)
{
    m_json = json;
}

void ItemWorker::process()
{
    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("worker_connection"));
    m_db.setDatabaseName(QStringLiteral("ownnews.sqlite"));

    if (m_db.open()) {
        parseItems();
        Q_EMIT finished();
    } else {
        qDebug() << Q_FUNC_INFO << "Unable to open database" << m_db.lastError();
    }
}

void ItemWorker::parseItems()
{
    auto data = QJsonDocument::fromJson(m_json);
    if (data.isNull() || !data.isObject()) {
        qWarning() << "Got invalid json";
        qDebug() << m_json;
        qDebug() << data;
        return;
    }

    const auto object = data.object();

    // OLD API QList<QVariant> items = data.toMap()["ocs"].toMap()["data"].toMap()["items"].toList();
    const auto items = object[QStringLiteral("items")].toArray();

    qDebug() << "Item Count" << items.size();

    for (const auto &item : items) {
        const auto map = item.toObject();
        addItem(map[QStringLiteral("id")].toInt(),
                map[QStringLiteral("feedId")].toInt(),
                map[QStringLiteral("title")].toString(),
                map[QStringLiteral("body")].toString(),
                map[QStringLiteral("url")].toString(),
                map[QStringLiteral("author")].toString(),
                map[QStringLiteral("pubDate")].toInt(),
                map[QStringLiteral("unread")].toBool(),
                map[QStringLiteral("starred")].toBool(),
                map[QStringLiteral("guid")].toString(),
                map[QStringLiteral("guidHash")].toString());
    }
}

void ItemWorker::addItem(int id,
                         int feedid,
                         const QString &title,
                         const QString &body,
                         const QString &link,
                         const QString &author,
                         unsigned int pubdate,
                         bool unread,
                         bool starred,
                         const QString &guid,
                         const QString &guidhash)
{

    QSqlQuery qry(m_db);
    qry.prepare(
                QStringLiteral("INSERT OR REPLACE INTO items(id, feedid, title, body, link, author, pubdate, unread, starred, guid, guidhash) VALUES(:id, :feedid, "
                               ":title, :body, :link, :author, :pubdate, :unread, :starred, :guid, :guidhash)"));
    qry.bindValue(QStringLiteral(":id"), id);
    qry.bindValue(QStringLiteral(":feedid"), feedid);
    qry.bindValue(QStringLiteral(":title"), title);
    qry.bindValue(QStringLiteral(":body"), body);
    qry.bindValue(QStringLiteral(":link"), link);
    qry.bindValue(QStringLiteral(":author"), author);
    qry.bindValue(QStringLiteral(":pubdate"), pubdate);
    qry.bindValue(QStringLiteral(":unread"), unread);
    qry.bindValue(QStringLiteral(":starred"), starred);
    qry.bindValue(QStringLiteral(":guid"), guid);
    qry.bindValue(QStringLiteral(":guidhash"), guidhash);

    //        qDebug() << "Adding item " << feedid << title << pubdate;

    bool ret = qry.exec();
    if (!ret)
        qDebug() << qry.lastError();
    //        else {
    //            qDebug() << "item inserted!";
    // TODO
#if 0
    QVariantMap item;
    item["id"] = id;
    item["feedid"] = feedid;
    item["title"] = title;
    item["body"] = body;
    item["link"] = link;
    item["author"] = author;
    item["pubdate"] = QDateTime::fromTime_t(pubdate);
    item["unread"] = unread;
    item["starred"] = starred;

    m_items << item;
#endif
    //        }
}
