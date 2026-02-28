#include <windows.h>
#include "CollimatorAngleCal.h"
#include "ml_camera.h"
#include "CrossCenterPro.h"
#include "Core/icore.h"
#include <synchapi.h>


CollimatorAngleCal::CollimatorAngleCal(double baseX, double baseY, double pixelSize, double focalLength, CORE::MLCamera* cameraPtr)
{
	m_isEndThread = true;
	m_angleCallBack = nullptr;
	m_camera = cameraPtr;
	m_CCPro = new CrossCenterPro();
	m_CCPro->SetFramePixel(pixelSize);
	m_CCPro->SetFocalLength(focalLength);
	m_CCPro->SetBaseCenter(baseX, baseY);
	m_offsetX = -99999;
	m_offsetY = -99999;

	connect(Core::ICore::instance(), &Core::ICore::coreAboutToClose,this, [=]() {
		m_isEndThread = true;
		});
}

CollimatorAngleCal::~CollimatorAngleCal()
{
	m_isEndThread = true;
	if (m_thread.joinable())
	{
		m_thread.join();
	}

	delete m_CCPro;
}

bool CollimatorAngleCal::AngleCalculate()
{
	/* if (!m_camera->IsOpened())
		 return false;*/

	if (m_isEndThread)
		m_isEndThread = false;
	else
		return true;

	m_thread = std::thread(&CollimatorAngleCal::RunThread, this);
}

void CollimatorAngleCal::SubscribeAngleCallBack(CORE::MLAngleXYCallback angleCallBack)
{
	m_angleCallBack = angleCallBack;
}

double CollimatorAngleCal::GetOffsetX()
{
	return m_offsetX;
}

double CollimatorAngleCal::GetOffsetY()
{
	return m_offsetY;
}

void CollimatorAngleCal::RunThread()
{
	while (true)
	{
		if (m_isEndThread)
			break;

		if (!m_camera->IsOpened())
		{
			Sleep(50);
			continue;
		}

		m_camera->GrabOne();
		cv::Mat img = m_camera->GetImage();
		if (!img.empty())
		{
			/* cv::Mat img1;
			 cv::flip(img, img1, 0);*/
			m_CCPro->Execute(img, 0);
			cv::Point2f c0 = m_CCPro->GetBasePiont();
			cv::Point2f c1 = m_CCPro->GetCrossPiont();
			if ((c1.x >= c0.x + 500) || (c1.x <= c0.x - 500) || (c1.y >= c0.y + 400) || (c1.y <= c0.y - 400))
			{
				if (m_angleCallBack != nullptr)
					m_angleCallBack(-99999, -99999);
				m_offsetX = -99999;
				m_offsetY = -99999;
			}
			else
			{
				m_offsetX = (m_CCPro->GetAngleX() * 3600 / 2);
				m_offsetY = (m_CCPro->GetAngleY() * 3600 / 2);
				if (m_angleCallBack != nullptr)
					m_angleCallBack(m_offsetX, m_offsetY);
			}
		}
		Sleep(10);
	}
}
