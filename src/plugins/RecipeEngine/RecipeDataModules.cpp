#include "RecipeDataModules.h"


QHash<int, QByteArray>  RecipeDataModules::rolesToNames
{
	{ NameRole, "name"},
	{ StatusRole,"status" },
	{ ColorRole,"color" },
	{ ErrorRole,"error" },
	{ ResultRole,"result" }
};

QHash<QString, int>  RecipeDataModules::namesToRoles
{
	{"name",NameRole},
	{"status",StatusRole},
	{"color",ColorRole},
	{"error",ErrorRole},
	{"result",ResultRole}
};

RecipeDataModules::RecipeDataModules(QObject *parent)
	: QAbstractListModel(parent)
{
}

int RecipeDataModules::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return m_data.count();
}

QHash<int, QByteArray> RecipeDataModules::roleNames() const
{

	return rolesToNames;
}

QVariant RecipeDataModules::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	const Module& Module = m_data.at(index.row());
	if (role == NameRole)
		return Module.name;
	else if (role == StatusRole)
		return Module.status;
	else if (role == ColorRole)
		return Module.color;
	else if (role == ErrorRole)
		return Module.error;
	else if (role == ResultRole)
		return Module.result;
	else
		return QVariant();
}


bool RecipeDataModules::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (!hasIndex(index.row(), index.column(), index.parent()) || !value.isValid())
		return false;

	Module *item = &m_data[index.row()];
	switch (role)
	{
	case NameRole:
		item->name = value.toString();
		break;
	case ResultRole:
		item->result = value.toString();
		break;
	case StatusRole:
		item->status = value.toString();
	case ColorRole:
		item->color = value.toString();
		break;
	case ErrorRole:
		item->error = value.toString();
		break;
	}
	emit dataChanged(index, index, { role });
	return true;
}

const QVector<Module>& RecipeDataModules::getData()
{
	return m_data;
}

void RecipeDataModules::clear()
{
	beginResetModel();
	m_data.clear();
	endResetModel();
}

void RecipeDataModules::addModule(const QString& value)
{
	beginInsertRows(QModelIndex(), m_data.count(), m_data.count());
	m_data.append(Module(value));
	endInsertRows();
}

bool RecipeDataModules::setProperty(int index, const QString& property, const QString& value)
{
	if (index<0 || index > m_data.count())
		return false;

	QModelIndex  ind = this->index(index, 0);
	return setData(this->index(index,0), value, namesToRoles[property]);
}


QVariant RecipeDataModules::get(const QString& name, int index)
{
	if (index<0 || index > m_data.count() || !namesToRoles.contains(name))
		return QVariant();

	return data(this->index(index, 0), namesToRoles[name]);
}

void RecipeDataModules::clearResults()
{
	QModelIndex start = this->index(0, 0);
	QModelIndex end = this->index(m_data.size()-1, 0);

	for (int i = 0; i < m_data.size(); i++)
	{
		m_data[i].color = "black";
		m_data[i].result = "NT";
		m_data[i].error = "";
	}
	emit dataChanged(start, end, { ColorRole,ResultRole,ErrorRole });
}


RecipeDataModules::~RecipeDataModules()
{
}
