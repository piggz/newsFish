#include "newsinterface.h"

#include <QAuthenticator>
#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QUrlQuery>

// const QString NewsInterface::rootPath = "/ocs/v1.php/apps/news/";
const QString NewsInterface::rootPath = QStringLiteral("/index.php/apps/news/api/v1-2/");
const QString NewsInterface::format = QStringLiteral("json");

NewsInterface::NewsInterface(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_db(QSqlDatabase::addDatabase(QStringLiteral("QSQLITE")))
    , m_busy(false)
{
    m_networkManager->setAutoDeleteReplies(true);

    feedsPath = rootPath + QStringLiteral("feeds");
    itemsPath = rootPath + QStringLiteral("items");

    connect(m_networkManager,
            SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)),
            this,
            SLOT(slotAuthenticationRequired(QNetworkReply *, QAuthenticator *)));

    if (QDir().mkpath(QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)))) {
        qCritical() << "Couldn't create directory for cache database";
    }
    const auto path = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QLatin1Char('/') + qGuiApp->applicationName()
                                      + QStringLiteral(".sqlite"));
    qDebug() << "Storing cache in" << path;
    m_db.setDatabaseName(path);

    m_db.open();

    m_feedsModel = new FeedsModel(m_db, this);
    m_itemsModel = new ItemsModel(m_db, this);

    connect(m_itemsModel, SIGNAL(feedParseComplete()), this, SLOT(slotItemProcessFinished()));
}

void NewsInterface::sync()
{
    getFeeds();
}

void NewsInterface::slotAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator)
{
    qDebug() << "Asked to authenticate";
    authenticator->setUser(m_username);
    authenticator->setPassword(m_password);
}

void NewsInterface::slotItemProcessFinished()
{
    syncNextFeed();
}

void NewsInterface::getFeeds()
{
    if (!m_busy) {
        m_busy = true;
        Q_EMIT busyChanged(m_busy);

        QUrl url(m_serverPath + feedsPath);
        url.setUserName(m_username);
        url.setPassword(m_password);

        qDebug() << url << this;

        QNetworkRequest r(url);
        addAuthHeader(&r);

        auto reply = m_networkManager->get(r);
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            qDebug() << "Reply from feeds";
            m_feedsModel->parseFeeds(reply->readAll());
            m_feedsToSync = m_feedsModel->feedIds();
            syncNextFeed();
        });

        connect(reply, &QNetworkReply::sslErrors, this, [reply](const QList<QSslError> &) {
            reply->ignoreSslErrors();
        });
    }
}

void NewsInterface::getItems(int feedId)
{
    if (!m_busy) {
        m_busy = true;
        Q_EMIT busyChanged(m_busy);
    }
    qDebug() << "Getting items for feed " << feedId;

    QUrl url(m_serverPath + itemsPath);
    url.setUserName(m_username);
    url.setPassword(m_password);

    QUrlQuery q;
    q.addQueryItem(QStringLiteral("id"), QString::number(feedId));
    q.addQueryItem(QStringLiteral("batchSize"), QString::number(m_numItemsToSync));
    q.addQueryItem(QStringLiteral("offset"), QStringLiteral("0"));
    q.addQueryItem(QStringLiteral("type"), QStringLiteral("0"));
    q.addQueryItem(QStringLiteral("format"), format);
    q.addQueryItem(QStringLiteral("getRead"), QStringLiteral("true"));
    url.setQuery(q);

    QNetworkRequest r(url);
    addAuthHeader(&r);

    auto reply = m_networkManager->get(r);
    connect(reply, &QNetworkReply::finished, this, [this, reply, feedId]() {
        m_itemsModel->parseItems(QString::number(feedId), reply->readAll());

        syncNextFeed();
    });

    connect(reply, &QNetworkReply::sslErrors, this, [reply](const QList<QSslError> &) {
        reply->ignoreSslErrors();
    });
}

void NewsInterface::syncNextFeed()
{
    qDebug() << "Syncing next feed" << m_feedsToSync;

    if (!m_feedsToSync.isEmpty()) {
        int id = m_feedsToSync.takeFirst();
        getItems(id);
        return;
    }

    m_busy = false;
    Q_EMIT busyChanged(m_busy);
}

FeedsModel *NewsInterface::feedsModel() const
{
    return m_feedsModel;
}

ItemsModel *NewsInterface::itemsModel() const
{
    return m_itemsModel;
}

bool NewsInterface::isBusy() const
{
    qDebug() << "Busy: " << m_busy;
    return m_busy;
}

void NewsInterface::recreateDatabase()
{
    m_itemsModel->recreateTable();
}

void NewsInterface::setItemRead(long itemId, bool read)
{
    qDebug() << "Setting item read " << itemId;

    QUrl url(m_serverPath + itemsPath + QLatin1Char('/') + QString::number(itemId) + (read ? QStringLiteral("/read") : QStringLiteral("/unread")));
    url.setUserName(m_username);
    url.setPassword(m_password);

    QNetworkRequest r(url);
    addAuthHeader(&r);

    auto reply = m_networkManager->put(r, QByteArray());
    connect(reply, &QNetworkReply::finished, this, [this, reply, itemId, read]() {
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << reply->errorString() << reply->error();
        }
        m_busy = false;
        m_itemsModel->setItemRead(itemId, read);
        Q_EMIT busyChanged(m_busy);
    });
}

void NewsInterface::setItemStarred(int feedId, const QString &itemGUIDHash, bool starred)
{
    qDebug() << "Setting item starred " << itemGUIDHash;

    QUrl url(m_serverPath + itemsPath + QLatin1Char('/') + QString::number(feedId) + QLatin1Char('/') + itemGUIDHash
             + (starred ? QStringLiteral("/star") : QStringLiteral("/unstar")));
    url.setUserName(m_username);
    url.setPassword(m_password);

    QNetworkRequest r(url);
    addAuthHeader(&r);

    auto reply = m_networkManager->put(r, "");
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        m_busy = false;
        Q_EMIT busyChanged(m_busy);
    });
}

void NewsInterface::addAuthHeader(QNetworkRequest *r)
{
    if (r) {
        QString concatenated = m_username + QLatin1Char(':') + m_password;
        QByteArray data = concatenated.toLocal8Bit().toBase64();
        QString headerData = QStringLiteral("Basic ") + QString::fromUtf8(data);
        r->setRawHeader("Authorization", headerData.toLocal8Bit());
    }
}
