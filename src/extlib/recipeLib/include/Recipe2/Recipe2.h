#pragma once

#include "Recipe2Plugin.h"
#include "recipe2_global.h"

class RecipeWidget;
class RECIPE2_EXPORT Recipe2 : public QObject, public Recipe2Plugin
{
	Q_OBJECT
		/*
		*Use Q_INTERFACES() macro tells Qt's meta-object system about the interface and tells the meta-object system "I want to use this interface
		*/
		Q_INTERFACES(Recipe2Plugin)
		/*
		* Use Q_ PLUGIN_ METADATA() macro export plug-in
		*/
		Q_PLUGIN_METADATA(IID AbstractInterface_iid FILE "Recipe2.json")
public:
	Recipe2();
	~Recipe2();

	QWidget* GetWidget();

	/*virtual void NodeBinding(const std::string& ID,
		const SimpleActionNode::TickFunctor& tick_functor,
		PortsList ports = {});*/

	virtual void LoadCustomNode(std::string nodePath);

signals:
	void updateTreeSystemIsRunDone(bool isTreeSystemDone);
	void SendLogSignal(std::string logMsg);
	void UpdateCaptureProgressBar(int progress, bool isSucc);
	void UpdateAlignProgressBar(int progress, bool isSucc);
	void UpdateEntireProgressBar(int progress, bool isSucc);


public slots:
	virtual void IsSwitchOperatorMode(bool isSwitchOperatorMode);
	virtual void ZoomAndCenterView();
	virtual void HorizontalandVerticalLayout();
	virtual void Reorder();
	virtual void SwitchTreeOrXmlUI(int Tree_XmlUIMode);

public:
	bool LoadXmlByPureOperator(std::string path);
	void RunXmlByPureOperator();
	void ExitXmlByPureOperator();
	void ClearProgressBar();

private:
	RecipeWidget* widget = nullptr;
};
