#ifndef FEEDSMODEL_H
#define FEEDSMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QList>
#include <QMetaType>
#include <QSqlDatabase>
#include <QVariantMap>

class FeedsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        FeedUrlRole,
        TitleRole,
        FaviconLinkRole,
        AddedRole,
        FolderIdRole,
        UnreadCountRole,
        OrderingRole,
        LinkRole,
        PinnedRole,
        UpdateErrorCountRole,
        LastUpdateErrorRole,
    };

    explicit FeedsModel(const QSqlDatabase &db, QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    void parseFeeds(const QByteArray &json);
    QList<int> feedIds() const;

private:
    struct Feed {
        int id;
        QString feedUrl;
        QString title;
        QString faviconLink;
        QDateTime added;
        int folderId;
        int unreadCount;
        int ordering;
        QString link;
        bool pinned;
        int updateErrorCount;
        QString lastUpdateError;

        static Feed fromJson(const QJsonObject &object);
        static Feed fromQuery(const QSqlQuery &query);
    };

    QList<Feed> m_feeds;

    QSqlDatabase m_db;
    QString m_databaseName;
    void addFeed(const Feed &feed);
    void loadData();
    void checkFeeds(QList<int> feeds);
};

Q_DECLARE_METATYPE(FeedsModel *);

#endif // FEEDSMODEL_H
