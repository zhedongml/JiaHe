#ifndef MLALGORITHM_MLNONLINEARFIT_H_
#define MLALGORITHM_MLNONLINEARFIT_H_
#include <vector>
#include <Eigen/Dense>


namespace algorithm {

	typedef double(*Equation)(const double& x, const double& y, const Eigen::VectorXd& params);

	struct EquationInfo
	{
		Eigen::VectorXd x, y;
		Eigen::VectorXd initial_params;
		Equation objective_equation;
	};


	class MLFit
	{
	public:
		MLFit();
		~MLFit();
		MLFit(EquationInfo equation_info);

		void levenMar();
		void GaussNewton();
		Eigen::VectorXd GetParameters();

	private:
		Eigen::VectorXd x_, y_;
		int data_length_, params_length_;
		Eigen::VectorXd params_;
		Equation objective_equation_;

		Eigen::VectorXd ObjectiveValue(Eigen::VectorXd params);
		double LeastSquareFunction(const Eigen::VectorXd& obj);

		double NumericalPartialDerivation(const double& x, const double& y, const Eigen::VectorXd& params, int params_index);
		Eigen::MatrixXd Jacobin();
		double MaxMatrixDiagonale(const Eigen::MatrixXd& Hessian);
		double LinerDeltaL(const Eigen::VectorXd& step, const Eigen::VectorXd& gradient, const double u);
	};

}



#endif // !MLALGORITHM_MLNONLINEARFIT_H_

