#include "MLFrame.h"

namespace CORE
{
	MLFrame::MLFrame()
	{
		this->m_Depth = CORE::MLPixelFormat::kMLMono8;
	}

	MLFrame::MLFrame(cv::Mat image, CORE::MLPixelFormat depth)
	{
		this->m_Depth = depth;
		this->m_Image = image.clone();
	}

	MLFrame::MLFrame(const MLFrame& frame)
	{
		this->m_Depth = frame.m_Depth;
        this->m_Image = frame.m_Image.clone();
	}

	MLFrame& MLFrame::operator=(const MLFrame& frame)
	{
		this->m_Depth = frame.m_Depth;
        this->m_Image = frame.m_Image.clone();
		return *this;
	}

	MLFrame::~MLFrame()
	{
		m_Image.release();
	}
}