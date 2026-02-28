#pragma once

#include <QObject>
#include <QAbstractListModel>

struct Module
{
	Module(){}
	Module(const QString& name):name(name){}
	QString name;
	QString result="NT";
	QString error;
	QString color="black";
	QString status="start";
};


class RecipeDataModules : public QAbstractListModel
{
	Q_OBJECT

public:
	Q_INVOKABLE void clear();
	Q_INVOKABLE void addModule(const QString& value);
	Q_INVOKABLE bool setProperty(int index, const QString &property, const QString& value);
	Q_INVOKABLE QVariant get(const QString& name, int index);
	Q_INVOKABLE void clearResults();

	enum Roles
	{
		NameRole = Qt::UserRole,
		ResultRole,
		StatusRole,
		ColorRole,
		ErrorRole
	};

	explicit RecipeDataModules(QObject *parent=0);
	int rowCount(const QModelIndex& parent) const override;
	QHash<int, QByteArray> roleNames() const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;
	const QVector<Module>& getData();
	~RecipeDataModules();

private:
	QVector<Module> m_data;
	static QHash<int, QByteArray> rolesToNames;
	static QHash<QString, int> namesToRoles;
};


