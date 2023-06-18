#ifndef NEWSINTERFACE_H
#define NEWSINTERFACE_H

#include <QObject>
#include <QSqlDatabase>

#include "feedsmodel.h"
#include "itemsmodel.h"

class QNetworkAccessManager;
class QNetworkReply;
class QAuthenticator;
class QNetworkRequest;

class NewsInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(FeedsModel *feedsModel READ feedsModel CONSTANT)
    Q_PROPERTY(ItemsModel *itemsModel READ itemsModel CONSTANT)
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY(QString serverPath MEMBER m_serverPath NOTIFY serverPathChanged)
    Q_PROPERTY(QString username MEMBER m_username NOTIFY usernameChanged)
    Q_PROPERTY(QString password MEMBER m_password NOTIFY passwordChanged)
    Q_PROPERTY(int daysToRetain MEMBER m_daysToRetain NOTIFY daysToRetainChanged)
    Q_PROPERTY(int numItemsToSync MEMBER m_numItemsToSync NOTIFY numItemsToSyncChanged)

public:
    explicit NewsInterface(QObject *parent = nullptr);

    bool isBusy() const;
    FeedsModel *feedsModel() const;
    ItemsModel *itemsModel() const;

    Q_INVOKABLE void sync();
    Q_INVOKABLE void viewItems(int feedId);
    Q_INVOKABLE void recreateDatabase();
    Q_INVOKABLE void setItemRead(long itemId, bool read);
    Q_INVOKABLE void setItemStarred(int feedId, const QString &itemGUIDHash, bool starred);

Q_SIGNALS:
    void busyChanged(bool busy);
    void serverPathChanged();
    void usernameChanged();
    void passwordChanged();
    void daysToRetainChanged();
    void numItemsToSyncChanged();

private:
    QNetworkAccessManager *m_networkManager;
    QSqlDatabase m_db;

    FeedsModel *m_feedsModel;
    ItemsModel *m_itemsModel;

    static const QString rootPath;
    static const QString format;
    QString m_serverPath;
    QString feedsPath;
    QString itemsPath;
    QString m_username;
    QString m_password;
    int m_daysToRetain = 10;
    int m_numItemsToSync = 20;

    bool m_busy;

    QList<int> m_feedsToSync;

    void getFeeds();
    void getItems(int feedId);
    void syncNextFeed();
    void addAuthHeader(QNetworkRequest *r);

private Q_SLOTS:
    void slotAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);
    void slotItemProcessFinished();
};

#endif // NEWSINTERFACE_H
