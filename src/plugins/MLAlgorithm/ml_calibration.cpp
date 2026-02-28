#include "ml_cailbration.h"
//#include <iostream>


using namespace algorithm;

MLCalibration::MLCalibration()
{
}

MLCalibration::~MLCalibration()
{
}

MLCalibration& MLCalibration::GetInstance()
{
	static MLCalibration calibration;
	return calibration;
}

double MLCalibration::Gaussion(const double& x, const double& y, const Eigen::VectorXd& params)
{
	return params(0) * exp(-pow((x - params(1)) / params(2), 2)) - y;
}


void MLCalibration::CalculateDervative(std::vector<double>& x, std::vector<double>& y)
{
	
	for (int i = 0; i < x.size() - 1; i++)
	{
		x[i] = x[i] + (x[i + 1] - x[i]) / 2;
		y[i] = y[i + 1] - y[i];
	}
	x.pop_back();
	y.pop_back();
}




double MLCalibration::ScanLocation(std::vector<double> step_list, std::vector<double> data_list)
{
	//calculate derivation
	CalculateDervative(step_list, data_list);


	//sperate positive and negative parts
	std::vector<double> step_derivation_positive, data_derivation_positive, step_derivation_negative, data_derivation_negative;
	for (int i = 0; i < data_list.size(); i++)
	{
		if (data_list[i] >= 0)
		{
			step_derivation_positive.push_back(step_list[i]);
			data_derivation_positive.push_back(data_list[i]);
		}
		else
		{
			step_derivation_negative.push_back(step_list[i]);
			data_derivation_negative.push_back(fabs(data_list[i]));
		}
	}


	algorithm::EquationInfo gaussion_positive, gaussion_negative;
	Eigen::VectorXd params_positive(3), params_negative(3);

	gaussion_positive.x = Eigen::Map<Eigen::VectorXd>(step_derivation_positive.data(), step_derivation_positive.size());
	gaussion_positive.y = Eigen::Map<Eigen::VectorXd>(data_derivation_positive.data(), data_derivation_positive.size());
	gaussion_negative.x = Eigen::Map<Eigen::VectorXd>(step_derivation_negative.data(), step_derivation_negative.size());
	gaussion_negative.y = Eigen::Map<Eigen::VectorXd>(data_derivation_negative.data(), data_derivation_negative.size());

	params_positive << gaussion_positive.y.maxCoeff(), 1, 1;
	params_negative << gaussion_negative.y.maxCoeff(), 1, 1;

	gaussion_positive.initial_params = params_positive;
	gaussion_negative.initial_params = params_negative;
	gaussion_positive.objective_equation = Gaussion;
	gaussion_negative.objective_equation = Gaussion;

	MLFit fit_positive(gaussion_positive);
	fit_positive.levenMar();
	MLFit fit_negative(gaussion_negative);
	fit_negative.levenMar();
	
	Eigen::VectorXd parameters_positive = fit_positive.GetParameters();
	Eigen::VectorXd parameters_negative = fit_negative.GetParameters();


	return (parameters_positive(1) + parameters_negative(1)) / 2;
}