#ifndef FEEDSMODEL_H
#define FEEDSMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QMetaType>
#include <QSqlDatabase>
#include <QVariantMap>
class FeedsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles { FeedId = Qt::UserRole + 1, FeedTitle, FeedURL, FeedIcon };

    explicit FeedsModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    void parseFeeds(const QByteArray &json);
    void setDatabase(const QString &dbname);
    QList<int> feedIds() const;

private:
    QList<QVariantMap> m_feeds;

    QSqlDatabase m_db;
    QString m_databaseName;
    void addFeed(int id, const QString &title, const QString &url, const QString &icon);
    void loadData();
    void checkFeeds(QList<int> feeds);
};

Q_DECLARE_METATYPE(FeedsModel *);

#endif // FEEDSMODEL_H
