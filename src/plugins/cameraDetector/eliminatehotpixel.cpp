#include "eliminatehotpixel.h"

EliminateHotPixel* EliminateHotPixel::self = 0;
EliminateHotPixel* EliminateHotPixel::getInstance() {
	if (!self) {
		self = new EliminateHotPixel;
	}
	return self;
}
void EliminateHotPixel::loadHotAndDeadPixelCfg(string fileName,int cameraName) {
	//QString filePath = QCoreApplication::applicationDirPath() + "/config/";
	ifstream file("config/"+fileName, ios::in);
	if (file.fail()) 
	{ 
	    cout << "File not found:"<< fileName << endl;
	    return;
	}
	string line;
	vector<string> record;
	while (getline(file, line))
	{
		record.clear();
		int linepos = 0;
		char c;
		int linemax = line.length();
		string curstring;
		while (linepos < linemax)
		{
			c = line[linepos];
			if (isdigit(c)) {
				curstring += c;
			}
			else if (c == ',' && curstring.size()) {
				record.push_back(curstring);
				curstring = "";
			}
			++linepos;
		}
		if (curstring.size())
			record.push_back(curstring);
		int n = record.size();
		if (n >=2) {
			int x = atoi(record[0].c_str());
			int y = atoi(record[1].c_str());
			PixPostion pos;
			pos.x = x;
			pos.y = y;
			if (n == 3) {
				int pixRaduis = atoi(record[2].c_str());
				pos.pixR = pixRaduis;
			}
			else if (n == 4) {
				int pixRaduis = atoi(record[2].c_str());
				pos.pixR = pixRaduis;
				int thickness = atoi(record[3].c_str());
				pos.neighThick = thickness;
			}
			map<int, vector<PixPostion>>::iterator it = hotpixelInfo.find(cameraName);
			if ( it!= hotpixelInfo.end()) {
				vector<PixPostion>* pixlocVec =&(it->second);
				pixlocVec->push_back(pos);
			}
			else {
				vector<PixPostion> pixlocVec;
				pixlocVec.push_back(pos);
				hotpixelInfo.insert(map<int, vector<PixPostion>>::value_type (cameraName, pixlocVec));
			}
		}
	}
	file.close();
	
}
void EliminateHotPixel::setDeHotPixMode(int type)
{
	mode = type;
}
EliminateHotPixel::EliminateHotPixel() {

}
EliminateHotPixel::~EliminateHotPixel() {

}
int EliminateHotPixel::getNeighbourVal(cv::Mat& rawImg, int x, int y, int pixR, int neighThickness) {
	if (x >= rawImg.cols || y >= rawImg.rows)
		return 0;
	int outR = pixR + neighThickness;
	cv::Rect rec(x - outR, y - outR, 2 * outR + 1, 2 * outR + 1);
	cv::Rect hotpixRec(x-pixR, y-pixR, 2*pixR + 1,2*pixR+1);
	cv::Rect region_image(0, 0, rawImg.cols, rawImg.rows);
	cv::Mat roi_img = rawImg(rec & region_image);
	cv::Mat hotpix_img = rawImg(hotpixRec & region_image);
	cv::Scalar sum= cv::sum(roi_img);
	/*int val = 0;
	if (rawImg.depth() == CV_16U) {
		val = rawImg.at<ushort>(y, x);
	}
	else {
		val = rawImg.at<uchar>(y, x);
	}*/
	cv::Scalar hotpixSum = cv::sum(hotpix_img);
	int num = roi_img.cols * roi_img.rows - hotpix_img.cols * hotpix_img.rows;
	return (sum[0] - hotpixSum[0]) / num;
}
cv::Mat EliminateHotPixel::deHotAndDeadPixel(cv::Mat rawImg, int cameraName) {
	switch (mode)
	{
	case 0:
		return deHotAndDeadPixelByNeighbourVal(rawImg, cameraName);
		break;
	case 1:
		return deHotAndDeadPixelByInpaint(rawImg, cameraName);
		break;
	}
}
cv::Mat EliminateHotPixel::deHotAndDeadPixelByNeighbourVal(cv::Mat& rawImg, int cameraName) {
    if (!rawImg.data)
    {
        return rawImg;
    }
	map<int, vector<PixPostion>>::iterator it = hotpixelInfo.find(cameraName);
	if (it != hotpixelInfo.end()) {
		cv::Mat newImg = rawImg.clone();
		vector<PixPostion> pixlocVec = it->second;
		vector<PixPostion>::iterator hotpixelVecIt;
		for (hotpixelVecIt = pixlocVec.begin(); hotpixelVecIt != pixlocVec.end(); hotpixelVecIt++) {   
			PixPostion pt= *hotpixelVecIt;
            if (pt.x > rawImg.cols || pt.y > rawImg.rows) continue;
			int meanVal = getNeighbourVal(rawImg, pt.x, pt.y,pt.pixR,pt.neighThick);
			int startx = pt.x - pt.pixR;
			int starty = pt.y - pt.pixR;
			for (int i = 0; i < 2*pt.pixR+1; i++) {
				int rowx = starty + i;
				for (int j = 0; j < 2 * pt.pixR + 1; j++) {
					int coly = startx + j;
					if (rowx >= 0 && coly >= 0 && rowx< newImg.rows && coly <newImg.cols) {
						if (rawImg.depth() == CV_16U) {
							newImg.at<ushort>(rowx, coly) = meanVal;
						}
						else {
							newImg.at<uchar>(rowx, coly) = meanVal;
						}
					}
				}
			}
		}
        return newImg;
	}
	else {
		return rawImg;
	}

}

cv::Mat EliminateHotPixel::deHotAndDeadPixelByInpaint(cv::Mat& rawImg, int cameraName)
{
	if (!rawImg.data)
	{
		return rawImg;
	}
	map<int, vector<PixPostion>>::iterator it = hotpixelInfo.find(cameraName);
	if (it != hotpixelInfo.end()) {
		cv::Mat newImg ;
		cv::Mat imageMask = cv::Mat(rawImg.size(), CV_8UC1, cv::Scalar::all(0));
		vector<PixPostion> pixlocVec = it->second;
		vector<PixPostion>::iterator hotpixelVecIt;
		for (hotpixelVecIt = pixlocVec.begin(); hotpixelVecIt != pixlocVec.end(); hotpixelVecIt++) {
			PixPostion pt = *hotpixelVecIt;
			if (pt.x >= rawImg.cols || pt.y >= rawImg.rows) continue;
			int startx = pt.x - pt.pixR;
			int starty = pt.y - pt.pixR;
			for (int i = 0; i < 2 * pt.pixR + 1; i++) {
				int rowx = starty + i;
				for (int j = 0; j < 2 * pt.pixR + 1; j++) {
					int coly = startx + j;
					if (rowx >= 0 && coly >= 0 && rowx < imageMask.rows && coly < imageMask.cols) {
						imageMask.at<uchar>(rowx, coly) = 255;
					}
				}
			}
		}
		inpaint(rawImg, imageMask, newImg, 3, cv::INPAINT_TELEA);
		return newImg;
	}
	else {
		return rawImg;
	}
}
