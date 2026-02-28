#pragma once

#include <string>
#include <vector>
#include "prjcommon_global.h"

namespace MLUtils {
	enum Size
	{
		Large = 0,
		Small,
		Size_UnKnown
	};

	enum EyeType
	{
		Left = 0,
		Right,
		EyeType_UnKnown
	};

	enum EyeRelief
	{
		Far = 0,     // 远，对应 21.3mm
		Medium,      // 中，对应 13.3mm
		Near,        // 近，对应 5.3mm      
		EyeRelief_UnKnown
	};

	struct TestState
	{
		bool IsDut = true;
		bool IsUpdateSLB = true;
		Size size;
		EyeType eyeType;
		EyeRelief relief;
		std::string cust_type;

		TestState() : size(Size_UnKnown), eyeType(EyeType_UnKnown), relief(EyeRelief_UnKnown) {}
	};

	class PRJCOMMON_EXPORT MLUtilCommon {
	public:
		static MLUtilCommon* instance();

		~MLUtilCommon();

		Size TransStrToSize(std::string str);
		Size TransIntToSize(int size);
		std::string TransSizeToStr(Size size);
		EyeType TransStrToEyeType(std::string str);
		EyeRelief TransStrToEyeRelief(std::string str);
		EyeRelief TransIntToEyeRelief(int index);

		std::vector<std::string> split(const std::string& s, char delimiter);

	private:
		MLUtilCommon();
	private:
		static MLUtilCommon* m_instance;
	};
}  // namespace MLUtils


