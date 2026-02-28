#include "ml_nonlinear_fit.h"
#include <iostream>
using namespace Eigen;
using namespace algorithm;

#define MAX_ITER 1000
#define DERIV_STEP 0.001

using namespace std;


MLFit::MLFit()
{
}

MLFit::~MLFit()
{
}

MLFit::MLFit(EquationInfo equation_info) :
    x_(equation_info.x), y_(equation_info.y), params_(equation_info.initial_params), objective_equation_(equation_info.objective_equation)
{
    if (x_.size() != y_.size())
        return;
    data_length_ = x_.size();
    params_length_ = params_.size();
};


//return vector make up of objective equation element.
VectorXd MLFit::ObjectiveValue(VectorXd params)
{
    VectorXd obj(data_length_);
    for (int i = 0; i < data_length_; i++)
        obj(i) = objective_equation_(x_[i], y_[i], params);

    return obj;
}

//F = (f ^t * f)/2
double MLFit::LeastSquareFunction(const VectorXd& obj)
{
    return obj.squaredNorm() / 2;
}

// numerical derivation
double MLFit::NumericalPartialDerivation(const double& x, const double& y, const VectorXd& params, int params_index)
{
    VectorXd para1 = params;
    VectorXd para2 = params;

    para1[params_index] -= DERIV_STEP;
    para2[params_index] += DERIV_STEP;

    double obj1 = objective_equation_(x, y, para1);
    double obj2 = objective_equation_(x, y, para2);

    return (obj2 - obj1) / (2 * DERIV_STEP);
}

MatrixXd MLFit::Jacobin()
{
    int rowNum = data_length_;
    int colNum = params_length_;

    MatrixXd Jac(rowNum, colNum);

    for (int i = 0; i < rowNum; i++)
    {
        for (int j = 0; j < colNum; j++)
        {
            Jac(i, j) = NumericalPartialDerivation(x_[i], y_[i], params_, j);
        }
    }
    return Jac;
}

void MLFit::GaussNewton()
{
   
    VectorXd obj(data_length_);

    double last_sum = 0;

    int iterCnt = 0;
    while (iterCnt < MAX_ITER)
    {
        obj = ObjectiveValue(params_);

        double sum = 0;
        sum = LeastSquareFunction(obj);

        if (fabs(sum - last_sum) <= 1e-12)
            break;
        last_sum = sum;

        MatrixXd Jac = Jacobin();
        VectorXd delta(params_length_);
        delta = (Jac.transpose() * Jac).inverse() * Jac.transpose() * obj;

        params_ -= delta;
        iterCnt++;
    }
}

double MLFit::MaxMatrixDiagonale(const MatrixXd& Hessian)
{
    int max = 0;
    for (int i = 0; i < Hessian.rows(); i++)
    {
        if (Hessian(i, i) > max)
            max = Hessian(i, i);
    }

    return max;
}

//L(h) = F(x) + h^t*J^t*f + h^t*J^t*J*h/2
//deltaL = h^t * (u * h - g)/2
double MLFit::LinerDeltaL(const VectorXd& step, const VectorXd& gradient, const double u)
{
    double L = step.transpose() * (u * step - gradient);
    return L / 2;
}

void MLFit::levenMar()
{
    //int errNum = input.rows();      //error num
    //int paraNum = params.rows();    //parameter num

    //initial parameter 
    VectorXd obj = ObjectiveValue(params_);
    MatrixXd Jac = Jacobin();  //jacobin
    MatrixXd A = Jac.transpose() * Jac;             //Hessian
    VectorXd gradient = Jac.transpose() * obj;      //gradient

    //initial parameter tao v epsilon1 epsilon2
    double tao = 1e-3;
    long long v = 2;
    double eps1 = 1e-12, eps2 = 1e-12;
    double u = tao * MaxMatrixDiagonale(A);
    bool found = gradient.norm() <= eps1;
    if (found) return;

    double last_sum = 0;
    int iterCnt = 0;

    while (iterCnt < MAX_ITER)
    {
        VectorXd obj = ObjectiveValue(params_);

        MatrixXd Jac = Jacobin();  //jacobin
        MatrixXd A = Jac.transpose() * Jac;             //Hessian
        VectorXd gradient = Jac.transpose() * obj;      //gradient

        if (gradient.norm() <= eps1)
        {
            //cout << "stop g(x) = 0 for a local minimizer optimizer." << endl;
            break;
        }

        //cout << "A: " << endl << A << endl;

        VectorXd step = (A + u * MatrixXd::Identity(params_length_, params_length_)).inverse() * gradient; //negtive Hlm.

        //cout << "step: " << endl << step << endl;

        if (step.norm() <= eps2 * (params_.norm() + eps2))
        {
            //cout << "stop because change in x is small" << endl;
            break;
        }

        VectorXd paramsNew = params_ - step; //h_lm = -step;

        //compute f(x)
        //obj = objF(params);

        //compute f(x_new)
        VectorXd obj_new = ObjectiveValue(paramsNew);

        double deltaF = LeastSquareFunction(obj) - LeastSquareFunction(obj_new);
        double deltaL = LinerDeltaL(-1 * step, gradient, u);

        double ro = deltaF / deltaL;
        //cout << "roi is : " << roi << endl;
        if (ro > 0)
        {
            params_ = paramsNew;
            u *= max(1.0 / 3.0, 1 - pow(2 * ro - 1, 3));
            v = 2;
        }
        else
        {
            u = u * v;
            v = v * 2;
        }

        //cout << "u = " << u << " v = " << v << endl;

        iterCnt++;
        //cout << "Iterator " << iterCnt << " times, result is :" << endl << endl;
    }

    //cout << "Parameters: " << params_ << endl;

}

VectorXd MLFit::GetParameters()
{
    return params_;
}
