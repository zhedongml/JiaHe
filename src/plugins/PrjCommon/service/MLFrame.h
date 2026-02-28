#ifndef CORE_MLFRAME_ML_H_
#define CORE_MLFRAME_ML_H_
#include "ml.h"
#include <opencv2/highgui/highgui.hpp>
#include "../prjcommon_global.h"

namespace CORE
{
	class PRJCOMMON_EXPORT MLFrame
	{
	public:
		MLFrame();
		MLFrame(cv::Mat image, CORE::MLPixelFormat depth);
		MLFrame(const MLFrame& frame);

		MLFrame& operator=(const MLFrame& frame);

		~MLFrame();
	public:
		cv::Mat m_Image;
		CORE::MLPixelFormat m_Depth;

		MLPixelFormat format;

        int width;

        int height;

        void *buffer_ptr;
	};
}

#endif // !CORE_MLFRAME_ML_H_