#ifndef MLALGORITHM_MLCALIBRATION_H_
#define MLALGORITHM_MLCALIBRATION_H_

#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif

#include "ml_nonlinear_fit.h"

namespace algorithm {

	class ALGORITHM_API MLCalibration
	{
	public:
		static MLCalibration& GetInstance();
		double ScanLocation(std::vector<double> step_list, std::vector<double> data_list);

	private:

		MLCalibration();
		~MLCalibration();
		MLCalibration(const MLCalibration& calibration);
		const MLCalibration& operator=(const MLCalibration& calibration);

		void CalculateDervative(std::vector<double>& x, std::vector<double>& y);
		static double Gaussion(const double& x, const double& y, const Eigen::VectorXd& params);



	};




}



#endif // !MLALGORITHM_MLCALIBRATION_H_

