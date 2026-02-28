#include "ml_vimba_camera.h"
#include "PrjCommon/service/config.h"
#include "ml_frame_observer.h"
#include "ml_exposure_auto.h"

#include <direct.h>
#define NUM_FRAMES 3

string GetInstanceFolderPath(void)
{
	string exePath = "";
	CHAR tcFullPath[MAX_PATH];
	GetModuleFileNameA(NULL, tcFullPath, MAX_PATH);
	exePath = string(tcFullPath);
	string dirPath = "";
	size_t iPos = exePath.rfind("\\");
	dirPath = exePath.substr(0, iPos);
	return dirPath;
}
MLVimbaCamera::MLVimbaCamera()
{
	width_ = 0;
	height_ = 0;
	frame_rate_ = 0.0;
	format_type_ = MLPixelFormat::kMLRGB24;
	camera_state_ = MLCameraState::kConnected;
	state_callback_ = false;
	grabbing_callback_ = false;
	graylevel_callback_ = false;
	is_grabbing_ = false;
	camera_callback_ = NULL;
	is_open_ = false;
	disconnect_ = false;
	exposure_time_ = 0;
	m_bThread = false;
	flip_rotate_ = 11;
	index_ = 0;
	pixel_fix_ = false;

	InitializeConfigVariables();
}

MLVimbaCamera::MLVimbaCamera(int index, bool fix_pixel)
{
	width_ = 0;
	height_ = 0;
	frame_rate_ = 0.0;
	format_type_ = MLPixelFormat::kMLRGB24;
	camera_state_ = MLCameraState::kConnected;
	state_callback_ = false;
	grabbing_callback_ = false;
	graylevel_callback_ = false;
	is_grabbing_ = false;
	camera_callback_ = NULL;
	is_open_ = false;
	disconnect_ = false;
	exposure_time_ = 0;
	m_bThread = false;
	flip_rotate_ = 11;
	index_ = index;
	pixel_fix_ = fix_pixel;

	InitializeConfigVariables();
}

MLVimbaCamera::~MLVimbaCamera()
{
	Close();
	MLCameraSystem::GetInstance().ShutDown();
}

void MLVimbaCamera::InitializeConfigVariables()
{
	// setting autoExposure related variables
	string dark_level;
	string black_time_str;
	string dynamic_range_str;
	string max_exposure_time_str;
	char* buffer;

	if ((buffer = getcwd(NULL, 0)) == NULL)
	{
		perror("getcwd error");
	}
	else
	{
		printf("%s\n", buffer);
		string dir(buffer);
		string dlldir = GetInstanceFolderPath();
		free(buffer);
	}

	CORE::CConfigFile* config = new CORE::CConfigFile("config\\device\\cameraExposure.ini");
	config->GetValue("MLExposure", "dark_level", dark_level);
	config->GetValue("MLExposure", "dark_time", black_time_str);
	config->GetValue("MLExposure", "dynamic_range", dynamic_range_str);
	config->GetValue("MLExposure", "max_time", max_exposure_time_str);
	config->GetValue("MLExposure", "dark_level", dark_level);
	black_level_ = atof(dark_level.c_str());
	black_time_ = atoi(black_time_str.c_str());
	dynamic_range_ = atof(dynamic_range_str.c_str());
	max_exposure_time_ = atoi(max_exposure_time_str.c_str());

	if (dynamic_range_ <= 0.2)
	{
		dynamic_range_ = 0.3;
	}
	else if (dynamic_range_ >= 1)
	{
		dynamic_range_ = 0.8;
	}
	delete config;
}

MLResult MLVimbaCamera::Open()
{
	std::string message;
	MLResult result = {};
	if (SP_ISNULL(camera_ptr_))
		return result;
	VmbErrorType err = camera_ptr_->Open(VmbAccessModeFull);

	switch (err)
	{
	case VmbErrorSuccess:
		result.code = 1;
		result.msg = "success";
		is_open_ = true;
		message = "Camera ID: " + GetCameraInfo().id + ", open successfully.";
		LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
		break;
	case VmbErrorApiNotStarted:
		result.code = -1;
		result.msg = "VmbStartup() was not called before the current command";
		is_open_ = false;
		message = "Camera ID: " + GetCameraInfo().id +
			", VmbStartup() was not called before the current command when open the camera.";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());
		break;
	case VmbErrorNotFound:
		result.code = -2;
		result.msg = "The designated camera cannot be found";
		is_open_ = false;
		message = "Camera ID: " + GetCameraInfo().id + ", The designated camera cannot be found when open the camera.";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());
		break;
	case VmbErrorInvalidAccess:
		result.code = -3;
		result.msg = "Operation is invalid with the current access mode";
		is_open_ = false;
		message = "Camera ID: " + GetCameraInfo().id +
			", Operation is invalid with the current access mode when open the camera.";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());
		break;
	default:
		result.code = 0;
		result.msg = "Other error";
		is_open_ = false;
		message = "Camera ID: " + GetCameraInfo().id + ", Other error when open the camera.";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());
		break;
	}

	if (state_callback_ == true)
	{
		if (VmbErrorSuccess == err)
		{
			camera_callback_->NotifyCameraStateChanged(MLCameraState::kConnected, MLCameraState::kIsOpen);

			camera_state_ = MLCameraState::kIsOpen;
		}
		else
		{
			if (disconnect_ == true)
			{
				camera_callback_->NotifyCameraStateChanged(MLCameraState::kConnected, MLCameraState::kDisconnected);

				camera_state_ = MLCameraState::kDisconnected;
			}
			else
			{
				camera_callback_->NotifyCameraStateChanged(MLCameraState::kConnected, MLCameraState::kError);

				camera_state_ = MLCameraState::kError;
			}
		}
	}

	return result;
}

MLResult MLVimbaCamera::OpenBySN(const char* SN)
{
	std::string message;
	MLResult result = {};
	camera_ptr_ = MLCameraSystem::GetInstance().GetCameraPtrBySN(SN);
	if (SP_ISNULL(camera_ptr_))
	{
		result.code = 0;
		result.msg = "the Vimba camera [" + std::string(SN) + "] open failed!Vimba Camera is not recognized";
		message = "the Vimba camera [" + std::string(SN) + "] open failed!Vimba Camera is not recognized";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());
		return result;
	}

	VmbErrorType err = camera_ptr_->Open(VmbAccessModeFull);

	int flip = flip_rotate_ / 10;

	switch (err)
	{
	case VmbErrorSuccess:
		result.code = 1;
		result.msg = "success";
		is_open_ = true;
		GetFormatType();
		message = "Camera ID: " + GetCameraInfo().id + ", open successfully.";
		LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
		switch (flip)
		{
		case 1:
			FlipX(false);
			FlipY(false);
			break;
		case 2:
			FlipX(true);
			FlipY(false);
			break;
		case 3:
			FlipX(false);
			FlipY(true);
			break;
		case 4:
			FlipX(true);
			FlipY(true);
			break;
		default:
			break;
		}
		break;
	case VmbErrorApiNotStarted:
		result.code = -1;
		result.msg = "VmbStartup() was not called before the current command";
		message = "Camera ID: " + GetCameraInfo().id +
			", VmbStartup() was not called before the current command when open the camera.";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());
		break;
	case VmbErrorNotFound:
		result.code = -2;
		result.msg = "The designated camera cannot be found";
		message = "Camera ID: " + GetCameraInfo().id + ", The designated camera cannot be found when open the camera.";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());
		break;
	case VmbErrorInvalidAccess:
		result.code = -3;
		result.msg = "Operation is invalid with the current access mode";
		message = "Camera ID: " + GetCameraInfo().id +
			", Operation is invalid with the current access mode when open the camera.";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());
		break;
	default:
		result.code = 0;
		result.msg = "Other error";
		message = "Camera ID: " + GetCameraInfo().id + ", Other error when open the camera.";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());
		break;
	}

	if (state_callback_ == true)
	{
		if (VmbErrorSuccess == err)
		{
			camera_callback_->NotifyCameraStateChanged(MLCameraState::kConnected, MLCameraState::kIsOpen);

			camera_state_ = MLCameraState::kIsOpen;
		}
		else
		{
			camera_callback_->NotifyCameraStateChanged(MLCameraState::kConnected, MLCameraState::kError);

			camera_state_ = MLCameraState::kError;
		}
	}
	return result;
}

MLCameraInfo MLVimbaCamera::GetCameraInfo()
{
	if (SP_ISNULL(camera_ptr_))
		return camera_info_;
	if (camera_info_.id.empty() && camera_info_.serial_number.empty())
	{
		VmbErrorType err;
		err = camera_ptr_->GetID(camera_info_.id);
		if (VmbErrorSuccess != err)
		{
			LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Could not get camera ID. Error code:" << err);
		}

		err = camera_ptr_->GetName(camera_info_.friendly_name);
		if (VmbErrorSuccess != err)
		{
			LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Could not get camera name. Error code:" << err);
		}

		err = camera_ptr_->GetModel(camera_info_.fullname);
		if (VmbErrorSuccess != err)
		{
			LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Could not get camera mode name. Error code:" << err);
		}

		err = camera_ptr_->GetSerialNumber(camera_info_.serial_number);
		if (VmbErrorSuccess != err)
		{
			LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Could not get camera serial number. Error code:" << err);
		}

		err = camera_ptr_->GetInterfaceID(camera_info_.ip);
		if (VmbErrorSuccess != err)
		{
			LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Could not get interface ID. Error code:" << err);
		}
	}
	return camera_info_;
}

bool MLVimbaCamera::IsOpened()
{
	return is_open_;
}

MLResult MLVimbaCamera::GrabOne()
{
	if (is_grabbing_ == false && !SP_ISNULL(camera_ptr_))
	{
		std::string message;
		FramePtr frame_ptr;
		VmbErrorType err;
		VmbFrameStatusType receive_status;
		err = camera_ptr_->AcquireSingleImage(frame_ptr, 5000);

		if (VmbErrorSuccess == err)
		{
			err = frame_ptr->GetReceiveStatus(receive_status);
		}
		if (VmbErrorSuccess == err && VmbFrameStatusComplete == receive_status)
		{
			SourceImage2CV(frame_ptr);
			message = "Camera ID: " + GetCameraInfo().id + ", grab one successfully.";
			LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
			return MLResult{ 1, "success" };
		}
		else
		{
			switch (err)
			{
			case VmbErrorBadParameter:
				return MLResult{ -1, "frame is NULL" };
				message = "Camera ID: " + GetCameraInfo().id + ", frame is NULL when grab one.";
				LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());
				break;
			case VmbErrorTimeout:
				return MLResult{ -2, "Call timed out" };
				message = "Camera ID: " + GetCameraInfo().id + ", Call timed out when grab one.";
				LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());
				break;
			default:
				return MLResult{ 0, "Other error" };
				message = "Camera ID: " + GetCameraInfo().id + ", Other error when grab one.";
				LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());
				break;
			}
			if (disconnect_ == true && camera_callback_ != NULL)
			{
				camera_callback_->NotifyCameraStateChanged(MLCameraState::kConnected, MLCameraState::kDisconnected);

				camera_state_ = MLCameraState::kDisconnected;
			}
		}
	}
	return MLResult{ 1, "is grabbing!" };
}

MLResult MLVimbaCamera::StartGrabbingAsync()
{
	std::string message;
	if (SP_ISNULL(camera_ptr_))
	{
		message = "Camera ID: " + GetCameraInfo().id + ",start grabbing error,camera ptr is null!";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());
		return MLResult{ 0,message };
	}
	if (is_grabbing_ == true)
	{
		message = "Camera ID: " + GetCameraInfo().id + ", camera is already in grabbing mode.";
		LOG4CPLUS_WARN(LogPlus::getInstance()->logger, message.c_str());
		return MLResult{ 1,message };
	}

	format_type_ = GetFormatType();

	SP_SET(frame_observer_ptr_, new MLFrameObserver(camera_ptr_, this));

	VmbErrorType err = camera_ptr_->StartContinuousImageAcquisition(NUM_FRAMES, frame_observer_ptr_);
	if (VmbErrorSuccess == err)
	{
		is_grabbing_ = true;
		if (state_callback_ == true)
		{
			camera_callback_->NotifyCameraStateChanged(MLCameraState::kIsOpen, MLCameraState::kIsGrabbing);

			camera_state_ = MLCameraState::kIsGrabbing;
		}
		message = "Camera ID: " + GetCameraInfo().id + ", the camera is grabbing ...";
		LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
		return MLResult{ 1, message };
	}
	else
	{
		message = "Camera ID: " + GetCameraInfo().id +
			", Could not start image acquisition. Error code:" + std::to_string(err);
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());

		if (disconnect_ == true && camera_callback_ != NULL)
		{
			camera_callback_->NotifyCameraStateChanged(MLCameraState::kConnected, MLCameraState::kDisconnected);

			camera_state_ = MLCameraState::kDisconnected;
		}
		return MLResult{ 0, message };
	}
}

MLResult MLVimbaCamera::StopGrabbing()
{
	std::string message;
	if (SP_ISNULL(camera_ptr_))
	{
		message = "Camera ID: " + GetCameraInfo().id + ",stop grabbing error,camera ptr is null!";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());
		return MLResult{ 0,message };
	}
	if (is_grabbing_ == false)
	{
		message = "Camera ID: " + GetCameraInfo().id + ", camera has stopped grabbing mode.";
		LOG4CPLUS_WARN(LogPlus::getInstance()->logger, message.c_str());
		return MLResult{ 1,message };
	}

	VmbErrorType err = camera_ptr_->StopContinuousImageAcquisition();

	if (VmbErrorSuccess == err)
	{
		is_grabbing_ = false;
		if (state_callback_ == true)
		{
			camera_callback_->NotifyCameraStateChanged(MLCameraState::kIsGrabbing, MLCameraState::kIsOpen);

			camera_state_ = MLCameraState::kIsOpen;
		}

		message = "Camera ID: " + GetCameraInfo().id + ", stop grabbing. ";
		LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
		return MLResult{ 1,message };
	}
	else
	{
		message = "Camera ID: " + GetCameraInfo().id +
			", Could not stop image acquisition. Error code:" + std::to_string(err);
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());

		if (disconnect_ == true && camera_callback_ != NULL)
		{
			camera_callback_->NotifyCameraStateChanged(MLCameraState::kConnected, MLCameraState::kDisconnected);

			camera_state_ = MLCameraState::kDisconnected;
		}
		return MLResult{ 0,message };
	}
}

bool MLVimbaCamera::IsGrabbing()
{
	return is_grabbing_;
}

int MLVimbaCamera::GetWidth()
{
	if (!SP_ISNULL(camera_ptr_) && width_ == 0)
	{
		VmbErrorType err;
		FeaturePtr feature;
		VmbInt64_t vimba_width;
		err = camera_ptr_->GetFeatureByName("Width", feature);
		if (VmbErrorSuccess == err)
		{
			feature->GetValue(vimba_width);
			width_ = (int)vimba_width;
		}
		else
		{
			std::string message =
				"Camera ID: " + GetCameraInfo().id + ", Could not get Width! Error code : " + std::to_string(err);
			LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());

			if (disconnect_ == true && camera_callback_ != NULL)
			{
				camera_callback_->NotifyCameraStateChanged(MLCameraState::kConnected, MLCameraState::kDisconnected);

				camera_state_ = MLCameraState::kDisconnected;
			}
		}
	}
	return width_;
}

int MLVimbaCamera::GetHeight()
{
	if (!SP_ISNULL(camera_ptr_) && height_ == 0)
	{
		VmbErrorType err;
		FeaturePtr feature;
		VmbInt64_t vimba_height;
		err = camera_ptr_->GetFeatureByName("Height", feature);
		if (VmbErrorSuccess == err)
		{
			feature->GetValue(vimba_height);
			height_ = (int)vimba_height;
		}
		else
		{
			std::string message =
				"Camera ID: " + GetCameraInfo().id + ", Could not get Height! Error code : " + std::to_string(err);
			LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());

			if (disconnect_ == true && camera_callback_ != NULL)
			{
				camera_callback_->NotifyCameraStateChanged(MLCameraState::kConnected, MLCameraState::kDisconnected);

				camera_state_ = MLCameraState::kDisconnected;
			}
		}
	}
	return height_;
}

void MLVimbaCamera::SetFrameRate(double rate)
{
	if (SP_ISNULL(camera_ptr_))
		return;
	std::string message;
	VmbErrorType err;
	FeaturePtr feature;
	err = camera_ptr_->GetFeatureByName("AcquisitionFrameRateEnable", feature);
	if (VmbErrorSuccess == err)
	{
		bool enable;
		err = feature->GetValue(enable);
		if (VmbErrorSuccess == err && !enable)
		{
			err = feature->SetValue(true);
		}
		if (VmbErrorSuccess == err)
		{
			err = camera_ptr_->GetFeatureByName("AcquisitionFrameRateAbs", feature);
			if (VmbErrorSuccess != err)
			{
				err = camera_ptr_->GetFeatureByName("AcquisitionFrameRate", feature);
			}
			if (VmbErrorSuccess == err)
			{
				err = feature->SetValue(rate);
			}
			if (VmbErrorSuccess == err)
			{
				frame_rate_ = rate;

				message = "Camera ID: " + GetCameraInfo().id + ", set frame rate : " + std::to_string(rate);
				LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
			}
			else
			{
				message = "Camera ID: " + GetCameraInfo().id + ", Could not set frame rate. ";
				LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());

				if (disconnect_ == true && camera_callback_ != NULL)
				{
					camera_callback_->NotifyCameraStateChanged(MLCameraState::kConnected, MLCameraState::kDisconnected);

					camera_state_ = MLCameraState::kDisconnected;
				}
			}
		}
		else
		{
			message = "Camera ID: " + GetCameraInfo().id + ", Could not set frame rate. ";
			LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());

			if (disconnect_ == true && camera_callback_ != NULL)
			{
				camera_callback_->NotifyCameraStateChanged(MLCameraState::kConnected, MLCameraState::kDisconnected);

				camera_state_ = MLCameraState::kDisconnected;
			}
		}
	}
}

double MLVimbaCamera::GetFrameRate()
{
	std::string message;
	if (!SP_ISNULL(camera_ptr_) && frame_rate_ == 0.0)
	{
		VmbErrorType err;
		FeaturePtr feature;
		err = camera_ptr_->GetFeatureByName("AcquisitionFrameRateAbs", feature);
		if (VmbErrorSuccess != err)
		{
			err = camera_ptr_->GetFeatureByName("AcquisitionFrameRate", feature);
		}
		if (VmbErrorSuccess == err)
		{
			err = feature->GetValue(frame_rate_);
		}
		if (VmbErrorSuccess != err)
		{
			message =
				"Camera ID: " + GetCameraInfo().id + ", Could not get frame rate! Error code :  " + std::to_string(err);
			LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());

			if (disconnect_ == true && camera_callback_ != NULL)
			{
				camera_callback_->NotifyCameraStateChanged(MLCameraState::kConnected, MLCameraState::kDisconnected);

				camera_state_ = MLCameraState::kDisconnected;
			}
		}
	}
	message = "Camera ID: " + GetCameraInfo().id + ", get frame rate : " + std::to_string(frame_rate_);
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());

	return frame_rate_;
}

void MLVimbaCamera::SetFormatType(MLPixelFormat format)
{
	if (SP_ISNULL(camera_ptr_))
		return;
	VmbErrorType err;
	FeaturePtr feature;
	VmbInt64_t pixel_format;
	std::string pixel_str;
	err = camera_ptr_->GetFeatureByName("PixelFormat", feature);
	if (VmbErrorSuccess == err)
	{
		switch (format)
		{
		case MLPixelFormat::kMLMono8:
			pixel_format = VmbPixelFormatMono8;
			pixel_str = "kMLMono8";
			break;
		case MLPixelFormat::kMLMono12:
			pixel_format = VmbPixelFormatMono12;
			pixel_str = "kMLMono12";
			break;
		case MLPixelFormat::kMLMono16:
			pixel_format = VmbPixelFormatMono16;
			pixel_str = "kMLMono16";
			break;
		case MLPixelFormat::kMLRGB24:
			pixel_format = VmbPixelFormatRgb8;
			pixel_str = "kMLRGB24";
			break;

		default:
			break;
		}
		err = feature->SetValue(pixel_format);
	}
	std::string message;
	if (VmbErrorSuccess == err)
	{
		format_type_ = format;

		message = "Camera ID: " + GetCameraInfo().id + ", set pixel format : " + pixel_str;
		LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
	}
	else
	{
		message =
			"Camera ID: " + GetCameraInfo().id + ", Could not set pixel format! Error code :  " + std::to_string(err);
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());

		if (disconnect_ == true && camera_callback_ != NULL)
		{
			camera_callback_->NotifyCameraStateChanged(MLCameraState::kConnected, MLCameraState::kDisconnected);

			camera_state_ = MLCameraState::kDisconnected;
		}
	}
}

MLPixelFormat MLVimbaCamera::GetFormatType()
{
	if (!SP_ISNULL(camera_ptr_) && format_type_ == MLPixelFormat::kMLRGB24)
	{
		VmbErrorType err;
		FeaturePtr feature;
		VmbInt64_t pixel_format;
		err = camera_ptr_->GetFeatureByName("PixelFormat", feature);
		if (VmbErrorSuccess == err)
		{
			err = feature->GetValue(pixel_format);
		}
		std::string message;
		std::string pixel_str;
		if (VmbErrorSuccess == err)
		{
			switch (pixel_format)
			{
			case VmbPixelFormatMono8:
				format_type_ = MLPixelFormat::kMLMono8;
				pixel_str = "kMLMono8";
				break;
			case VmbPixelFormatMono12:
				format_type_ = MLPixelFormat::kMLMono12;
				pixel_str = "kMLMono12";
				break;
			case VmbPixelFormatMono16:
				format_type_ = MLPixelFormat::kMLMono16;
				pixel_str = "kMLMono16";
				break;
			case VmbPixelFormatRgb8:
				format_type_ = MLPixelFormat::kMLRGB24;
				pixel_str = "kMLRGB24";
				break;

			default:
				break;
			}

			message = "Camera ID: " + GetCameraInfo().id + ", get pixel format : " + pixel_str;
			LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
		}
		else
		{
			message =
				"Camera ID: " + GetCameraInfo().id + ", Could not get pixel format! Error code :" + std::to_string(err);
			LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());

			if (disconnect_ == true && camera_callback_ != NULL)
			{
				camera_callback_->NotifyCameraStateChanged(MLCameraState::kConnected, MLCameraState::kDisconnected);

				camera_state_ = MLCameraState::kDisconnected;
			}
		}
	}
	return format_type_;
}

void MLVimbaCamera::SetExposureMode(MLExposureMode mode)
{
	if (SP_ISNULL(camera_ptr_))
		return;
	std::string message;
	std::string exposure_mode;
	VmbErrorType err;
	FeaturePtr feature;
	err = camera_ptr_->GetFeatureByName("ExposureAuto", feature);
	if (VmbErrorSuccess == err)
	{
		switch (mode)
		{
		case MLExposureMode::kContinuous:
			err = feature->SetValue("Continuous");
			exposure_mode = "Continuous";
			break;
		case MLExposureMode::kOff:
			err = feature->SetValue("Off");
			exposure_mode = "Off";
			break;
		case MLExposureMode::kOnce:
			err = feature->SetValue("Once");
			exposure_mode = "Once";
			break;
		case MLExposureMode::kMLAuto:
			// feature_name->SetValue("Continuous");
			break;

		default:
			break;
		}
	}

	if (VmbErrorSuccess != err)
	{
		message =
			"Camera ID: " + GetCameraInfo().id + ", Could not set exposure mode! Error code : " + std::to_string(err);
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());

		if (disconnect_ == true && camera_callback_ != NULL)
		{
			camera_callback_->NotifyCameraStateChanged(MLCameraState::kConnected, MLCameraState::kDisconnected);

			camera_state_ = MLCameraState::kDisconnected;
		}
	}
	else
	{
		message = "Camera ID: " + GetCameraInfo().id + ", set exposure mode : " + exposure_mode;
		LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
	}
}

MLResult MLVimbaCamera::SetMLExposureAuto()
{
	std::string message;
	if (SP_ISNULL(camera_ptr_))
	{
		message =
			"Camera ID: " + GetCameraInfo().id + ", set auto exposure time error,camera ptr is null!";
		return MLResult{ 0,message };
	}

	MLExposureAuto ML_Exposure(dynamic_range_, black_level_, black_time_, 24, max_exposure_time_,
		MLPixelFormat::kMLMono8 /*m_format*/);
	int et = ML_Exposure.MLAutoExposure(this, CaptureImage);
	MLResult result = SetAdjustExposureTime(et);
	if (result.code != 1)
	{
		message =
			"Camera ID: " + GetCameraInfo().id + ", set ML exposure auto. exposure time:" + std::to_string(et) + " error!";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());
		return result;
	}
	message =
		"Camera ID: " + GetCameraInfo().id + ", set ML exposure auto. exposure time:" + std::to_string(et);
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
	return MLResult{ 1,message };
}

MLResult MLVimbaCamera::SetExposureTime(double time)
{
	std::string message;
	VmbErrorType err;
	FeaturePtr feature;
	err = camera_ptr_->GetFeatureByName("ExposureTimeAbs", feature);
	if (VmbErrorSuccess != err)
	{
		err = camera_ptr_->GetFeatureByName("ExposureTime", feature);
	}
	if (VmbErrorSuccess == err)
	{
		err = feature->SetValue(time);
	}
	if (VmbErrorSuccess != err)
	{
		message =
			"Camera ID: " + GetCameraInfo().id + ", Could not set exposure time! Error code : " + std::to_string(err);
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());

		if (disconnect_ == true && camera_callback_ != NULL)
		{
			camera_callback_->NotifyCameraStateChanged(MLCameraState::kConnected, MLCameraState::kDisconnected);

			camera_state_ = MLCameraState::kDisconnected;
		}
		return MLResult{ 0,message };
	}
	else
	{
		message = "Camera ID: " + GetCameraInfo().id + ", set exposure time : " + std::to_string(time);
		LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
		return MLResult{ 1,message };
	}
}

double MLVimbaCamera::GetExposureTime()
{
	if (SP_ISNULL(camera_ptr_))
		return 0;
	VmbErrorType err;
	FeaturePtr feature;
	double exposure_time;
	err = camera_ptr_->GetFeatureByName("ExposureTimeAbs", feature);
	if (VmbErrorSuccess != err)
	{
		err = camera_ptr_->GetFeatureByName("ExposureTime", feature);
	}
	if (VmbErrorSuccess == err)
	{
		err = feature->GetValue(exposure_time);
	}
	if (VmbErrorSuccess == err)
	{

		return exposure_time;
	}
	else
	{
		std::string message =
			"Camera ID: " + GetCameraInfo().id + ", Could not get exposure time! Error code : " + std::to_string(err);
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());

		if (disconnect_ == true && camera_callback_ != NULL)
		{
			camera_callback_->NotifyCameraStateChanged(MLCameraState::kConnected, MLCameraState::kDisconnected);

			camera_state_ = MLCameraState::kDisconnected;
		}

		return 0.0;
	}
}

void MLVimbaCamera::GetExposureTimeRange(double& min, double& max)
{
	if (SP_ISNULL(camera_ptr_))
		return;
	std::string message;
	VmbErrorType err;
	FeaturePtr feature;
	double exposure_time;
	err = camera_ptr_->GetFeatureByName("ExposureTimeAbs", feature);
	if (VmbErrorSuccess != err)
	{
		err = camera_ptr_->GetFeatureByName("ExposureTime", feature);
	}
	if (VmbErrorSuccess == err)
	{
		err = feature->GetRange(min, max);
	}
	if (VmbErrorSuccess != err)
	{
		min = 0.0;
		max = 0.0;

		message = "Camera ID: " + GetCameraInfo().id + ", Could not get exposure time range! Error code " +
			std::to_string(err);
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());

		if (disconnect_ == true && camera_callback_ != NULL)
		{
			camera_callback_->NotifyCameraStateChanged(MLCameraState::kConnected, MLCameraState::kDisconnected);

			camera_state_ = MLCameraState::kDisconnected;
		}
	}
	else
	{
		message = "Camera ID: " + GetCameraInfo().id + ", get exposure time range:" + std::to_string(min) + "--" +
			std::to_string(max);
		LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
	}
}

MLResult MLVimbaCamera::SetAdjustExposureTime(double time)
{
	std::string message;
	if (SP_ISNULL(camera_ptr_))
	{
		message =
			"Camera ID: " + GetCameraInfo().id + ", set adjust exposure time " + std::to_string(time)+" error,camera ptr is null!";
		return MLResult{ 0,message };
	}
	int exposure_time = AdjustTime(time, 60);

	MLResult result = SetExposureTime(exposure_time);
	if (result.code != 1)
	{
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, result.msg.c_str());
		return result;
	}

	message =
		"Camera ID: " + GetCameraInfo().id + ", set adjust exposure time:" + std::to_string(exposure_time);
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
	return MLResult{ 1,"" };
}

MLCameraState MLVimbaCamera::GetState()
{
	return camera_state_;
}

void MLVimbaCamera::FlipX(bool flip)
{
	if (SP_ISNULL(camera_ptr_))
		return;
	VmbErrorType err;
	FeaturePtr feature;
	bool reverse;
	err = camera_ptr_->GetFeatureByName("ReverseX", feature);
	if (VmbErrorSuccess == err)
	{
		err = feature->GetValue(reverse);
	}
	if (VmbErrorSuccess == err && flip != reverse)
	{
		bool grabbing = false;
		if (is_grabbing_)
		{
			StopGrabbing();
			grabbing = true;
		}
		err = feature->SetValue(flip);

		if (grabbing)
		{
			StartGrabbingAsync();
		}

		if (VmbErrorSuccess == err)
		{
			std::string message = "Camera ID: " + GetCameraInfo().id + ", Flip it along the X-axis.";
			LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
		}
		else
		{
			std::string message = "Camera ID: " + GetCameraInfo().id + ", Flip it along the X-axis failed.";
			LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
		}
	}
}

void MLVimbaCamera::FlipY(bool flip)
{
	if (SP_ISNULL(camera_ptr_))
		return;
	VmbErrorType err;
	FeaturePtr feature;
	bool reverse;
	err = camera_ptr_->GetFeatureByName("ReverseY", feature);
	if (VmbErrorSuccess == err)
	{
		err = feature->GetValue(reverse);
	}
	if (VmbErrorSuccess == err && flip != reverse)
	{
		bool grabbing = false;
		if (is_grabbing_)
		{
			StopGrabbing();
			grabbing = true;
		}
		err = feature->SetValue(flip);

		if (grabbing)
		{
			StartGrabbingAsync();
		}

		if (VmbErrorSuccess == err)
		{
			std::string message = "Camera ID: " + GetCameraInfo().id + ", Flip it along the Y-axis.";
			LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
		}
		else
		{
			std::string message = "Camera ID: " + GetCameraInfo().id + ", Flip it along the Y-axis failed.";
			LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
		}
	}
}

void MLVimbaCamera::Subscribe(MLCameraEvent event, MLCameraCallback* callback)
{
	if (camera_callback_ == nullptr || callback == camera_callback_)
	{
		if (camera_callback_ == nullptr)
		{
			camera_callback_ = callback;
		}
		std::string camera_event;
		switch (event)
		{
		case CORE::MLCameraEvent::kStateChanged:
			state_callback_ = true;
			camera_event = "kStateChanged";
			break;
		case CORE::MLCameraEvent::kFrameReceived:
			StartGrabbingAsync();
			grabbing_callback_ = true;
			camera_event = "kFrameReceived";
			break;
		case CORE::MLCameraEvent::kGrayLevel:
			StartCalcGrayLevel();
			graylevel_callback_ = true;
			camera_event = "GrayLevelReceived";
			break;
		default:
			break;
		}
		/*std::string message = "Camera subscribe the camera callback: " + camera_event;
		LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());*/
	}
}

void MLVimbaCamera::Unsubscribe(MLCameraEvent event, MLCameraCallback* callback)
{
	if (camera_callback_ != nullptr)
	{
		std::string camera_event;
		switch (event)
		{
		case CORE::MLCameraEvent::kStateChanged:
			state_callback_ = false;
			camera_event = "kStateChanged";
			break;
		case CORE::MLCameraEvent::kFrameReceived:
			StopGrabbing();
			grabbing_callback_ = false;
			camera_event = "kFrameReceived";
			break;
		case CORE::MLCameraEvent::kGrayLevel:
			StopCalcGrayLevel();
			graylevel_callback_ = false;
			camera_event = "GrayLevelReceived";
			break;
		default:
			break;
		}

		if (state_callback_ == false && grabbing_callback_ == false && graylevel_callback_ == false)
		{
			// delete callback;

			// callback = NULL;

			camera_callback_ = NULL;
		}

		/* std::string message = "Camera ID: " + GetCameraInfo().id + ", unsubscribe the camera callback: " +
		 camera_event; LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());*/
	}
}

void MLVimbaCamera::Close()
{
	if (SP_ISNULL(camera_ptr_) || !is_open_)
		return;
	m_mutex.lock();
	graylevel_callback_ = false;
	m_mutex.unlock();
	StopCalcGrayLevel();
	if (is_grabbing_)
		StopGrabbing();
	VmbErrorType err = camera_ptr_->Close();
	is_open_ = false;
	if (state_callback_ == true)
	{
		if (VmbErrorSuccess == err)
		{
			camera_callback_->NotifyCameraStateChanged(MLCameraState::kIsOpen, MLCameraState::kConnected);

			camera_state_ = MLCameraState::kConnected;

			std::string message = "Camera ID: " + GetCameraInfo().id + ", close successfully.";
			LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
		}
		else
		{
			camera_callback_->NotifyCameraStateChanged(MLCameraState::kIsOpen, MLCameraState::kError);

			camera_state_ = MLCameraState::kError;

			std::string message =
				"Camera ID: " + GetCameraInfo().id + ", close failed, Error code: " + std::to_string(err);
			LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());

			if (disconnect_ == true && camera_callback_ != NULL)
			{
				camera_callback_->NotifyCameraStateChanged(MLCameraState::kConnected, MLCameraState::kDisconnected);

				camera_state_ = MLCameraState::kDisconnected;
			}
		}
	}
}

Mat MLVimbaCamera::GetImage()
{
	if (is_grabbing_)
	{
		Mat tmp;
		m_mutex.lock();
		tmp = img_.clone();
		m_mutex.unlock();
		return tmp;
		// if (pixel_fix_)
		//{
		//    return EliminateHotPixel::getInstance()->deHotAndDeadPixel(tmp,index_);
		//}
		// else
		//{
		//    return tmp;
		//}
	}
	else
	{
		GrabOne();
		return one_img_.clone();
		// if (pixel_fix_)
		//{
		//    return EliminateHotPixel::getInstance()->deHotAndDeadPixel(one_img_.clone(), index_);
		//}
		// else
		//{
		//    return one_img_.clone();
		//}
	}
}

void MLVimbaCamera::StartCalcGrayLevel()
{
	if (!m_bThread)
	{
		m_bThread = true;
		m_Thread_ = std::thread(&MLVimbaCamera::ThreadFun, this);
	}
}

void MLVimbaCamera::StopCalcGrayLevel()
{
	// if (m_hThread)
	//{
	//    CloseHandle(m_hThread);
	//}
	if (m_bThread)
	{
		m_bThread = false;
		m_Thread_.join();
	}
}

void MLVimbaCamera::Rotate(bool rotate)
{
	if (is_grabbing_)
	{
		m_mutex.lock();
		if (rotate)
		{
			flip_rotate_ += 1;
			if ((flip_rotate_ % 10) > 4)
			{
				flip_rotate_ = flip_rotate_ / 10 * 10 + 1;
			}
		}
		else
		{
			flip_rotate_ -= 1;
			if ((flip_rotate_ % 10) < 1)
			{
				flip_rotate_ = flip_rotate_ / 10 * 10 + 4;
			}
		}
		m_mutex.unlock();
	}
}

void MLVimbaCamera::SetBinning(Binning binning, BinningMode binningMode)
{
}

Binning MLVimbaCamera::getBinning()
{
	return Binning();
}

int MLVimbaCamera::AdjustTime(double init_time, int rate)
{
	if (rate > 0)
	{
		int times = init_time * rate / 1e6;
		if (times < 1)
		{
			return init_time;
		}
		double time_ad = double(times) / rate * 1e6;
		return round(time_ad);
	}
	else
		return init_time;
}

Mat CaptureImage(void* instance, int& exposure_time, bool is_auto)
{

	MLVimbaCamera* camera = ((MLVimbaCamera*)instance);
	if (is_auto)
	{
		exposure_time = camera->GetExposureTime();
		camera->exposure_time_ = exposure_time;
	}
	else
	{
		int temp = camera->exposure_time_ - exposure_time;
		camera->SetExposureTime(exposure_time);
		camera->exposure_time_ = exposure_time;

		if (temp >= 120000)
		{
			Sleep(temp / 300 + 800);
		}
		else
		{
			Sleep(exposure_time / 200 + 300);
		}
	}

	return camera->GetImage();
}

void MLVimbaCamera::ThreadFun()
{
	while (m_bThread)
	{
		if (!img_.empty() && is_grabbing_)
		{
			int sum = 0;
			m_mutex.lock();
			Mat img = img_.clone();
			m_mutex.unlock();
			if (img.empty())
			{
				continue;
			}

			int w = img.cols;
			int h = img.rows;
			unsigned char* p = img.ptr<unsigned char>(0);
			std::sort(p, p + 5);
			std::reverse(p, p + 5);

			for (int i = 5; i < w * h; i++)
			{
				int j = 0;
				for (j = 0; j < 5; j++)
				{
					if (p[i] > p[j])
					{
						p[j] = p[i];
						break;
					}
				}
			}

			for (int i = 0; i < 5; i++)
			{
				sum += p[i];
			}
			int maxV = sum / 5;

			if (camera_callback_ != NULL && graylevel_callback_)
			{
				camera_callback_->NotifyCameraGrayLevel(maxV);
			}
		}
		Sleep(200);
	}
}