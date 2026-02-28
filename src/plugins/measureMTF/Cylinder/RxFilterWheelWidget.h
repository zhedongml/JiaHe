#pragma once
#include <QWidget>
#include "ui_RxFilterWheelWidget.h"
#include "Core/itoolbox.h"
#include "Result.h"

enum class RefreshType
{
	Setting,
	Status
};

class RxFilterWheelWidget : public Core::IToolBox
{
	Q_OBJECT
public:
	RxFilterWheelWidget(QString toolBoxName = "", QWidget* parent = nullptr);
	~RxFilterWheelWidget();

private:
	void initUI();
	struct Mycmp {
		bool operator()(const std::pair<std::string, int>& p1,
			const std::pair<std::string, int>& p2) {
			return p1.second < p2.second;
		}
	};
public slots:
	void setCylinder(QString);
	void setAxis(QString);
	void updateConnectStatus(bool connect);
	void refresh(RefreshType type);
	void on_setrx_btn_clicked();
	void on_refresh_btn_clicked();

private:
	Ui::RxWidget ui;

};

