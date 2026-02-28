#include "Spline.h"
#include <cstring>
#include <algorithm>

using namespace std;

namespace SplineSpace
{

    //Constructor
    Spline::Spline(double* x0, double* y0, int& num,
        BoundaryCondition bc, double leftBoundary, double rightBoundary)
    {
        try
        {
            if ((x0 == NULL) || (y0 == NULL) || (num < 3))
            {
                throw SplineFailure("Construction failed, too few known points");
            }
            GivenX = x0;
            GivenY = y0;
            GivenNum = num;
            Bc = bc;
            LeftB = leftBoundary;
            RightB = rightBoundary;
            PartialDerivative = new double[GivenNum]; //Allocate space for partial derivatives
            MaxX = *max_element(GivenX, GivenX + GivenNum);
            MinX = *min_element(GivenX, GivenX + GivenNum);
            if (Bc == GivenFirstOrder) //I-type boundary condition
                PartialDerivative1();
            else if (Bc == GivenSecondOrder) //Type II boundary condition
                PartialDerivative2();
            else
            {
                delete[] PartialDerivative;
                throw SplineFailure("Boundary condition parameter error");
            }
        }
        catch (SplineFailure& e)
        {
            e.GetMessage();
        }
    }

    //I type boundary conditions to find partial derivatives
    void Spline::PartialDerivative1(void)
    {
        // The pursuit method solves the equation to find the second order partial derivative
        double* a = new double[GivenNum]; // a: a string of numbers at the bottom of the sparse matrix
        double* b = new double[GivenNum]; // b: the middlemost string of sparse matrix
        double* c = new double[GivenNum]; // c: a string of numbers at the top of the sparse matrix
        double* d = new double[GivenNum];

        double* f = new double[GivenNum];

        double* bt = new double[GivenNum];
        double* gm = new double[GivenNum];

        double* h = new double[GivenNum];

        for (int i = 0; i < GivenNum; i++) b[i] = 2; // the middle string is 2
        for (int i = 0; i < GivenNum - 1; i++) h[i] = GivenX[i + 1] - GivenX[i]; // each segment step size
        for (int i = 1; i < GivenNum - 1; i++) a[i] = h[i - 1] / (h[i - 1] + h[i]);
        a[GivenNum - 1] = 1;

        c[0] = 1;
        for (int i = 1; i < GivenNum - 1; i++) c[i] = h[i] / (h[i - 1] + h[i]);

        for (int i = 0; i < GivenNum - 1; i++)
            f[i] = (GivenY[i + 1] - GivenY[i]) / (GivenX[i + 1] - GivenX[i]);

        d[0] = 6 * (f[0] - LeftB) / h[0];
        d[GivenNum - 1] = 6 * (RightB - f[GivenNum - 2]) / h[GivenNum - 2];

        for (int i = 1; i < GivenNum - 1; i++) d[i] = 6 * (f[i] - f[i - 1]) / (h[i - 1] + h[i]);

        bt[0] = c[0] / b[0]; // The chase method solves the equation
        for (int i = 1; i < GivenNum - 1; i++) bt[i] = c[i] / (b[i] - a[i] * bt[i - 1]);

        gm[0] = d[0] / b[0];
        for (int i = 1; i <= GivenNum - 1; i++) gm[i] = (d[i] - a[i] * gm[i - 1]) / (b[i] - a[i] * bt[i - 1]);

        PartialDerivative[GivenNum - 1] = gm[GivenNum - 1];
        for (int i = GivenNum - 2; i >= 0; i--) PartialDerivative[i] = gm[i] - bt[i] * PartialDerivative[i + 1];

        delete[] a;
        delete[] b;
        delete[] c;
        delete[] d;
        delete[] gm;
        delete[] bt;
        delete[] f;
        delete[] h;
    }

    //Type II boundary conditions for partial derivatives
    void Spline::PartialDerivative2(void)
    {
        // The pursuit method solves the equation to find the second order partial derivative

        double* a = new double[GivenNum]; // a: a string of numbers at the bottom of the sparse matrix
        double* b = new double[GivenNum]; // b: the middlemost string of sparse matrix
        double* c = new double[GivenNum]; // c: a string of numbers at the top of the sparse matrix
        double* d = new double[GivenNum];

        double* f = new double[GivenNum];

        double* bt = new double[GivenNum];
        double* gm = new double[GivenNum];

        double* h = new double[GivenNum];

        for (int i = 0; i < GivenNum; i++) b[i] = 2;
        for (int i = 0; i < GivenNum - 1; i++) h[i] = GivenX[i + 1] - GivenX[i];
        for (int i = 1; i < GivenNum - 1; i++) a[i] = h[i - 1] / (h[i - 1] + h[i]);
        a[GivenNum - 1] = 1;

        c[0] = 1;
        for (int i = 1; i < GivenNum - 1; i++) c[i] = h[i] / (h[i - 1] + h[i]);

        for (int i = 0; i < GivenNum - 1; i++)
            f[i] = (GivenY[i + 1] - GivenY[i]) / (GivenX[i + 1] - GivenX[i]);

        for (int i = 1; i < GivenNum - 1; i++) d[i] = 6 * (f[i] - f[i - 1]) / (h[i - 1] + h[i]);

        d[1] = d[1] - a[1] * LeftB;
        d[GivenNum - 2] = d[GivenNum - 2] - c[GivenNum - 2] * RightB;

        bt[1] = c[1] / b[1];
        for (int i = 2; i < GivenNum - 2; i++) bt[i] = c[i] / (b[i] - a[i] * bt[i - 1]);

        gm[1] = d[1] / b[1];
        for (int i = 2; i <= GivenNum - 2; i++) gm[i] = (d[i] - a[i] * gm[i - 1]) / (b[i] - a[i] * bt[i - 1]);

        PartialDerivative[GivenNum - 2] = gm[GivenNum - 2];//
        for (int i = GivenNum - 3; i >= 1; i--) PartialDerivative[i] = gm[i] - bt[i] * PartialDerivative[i + 1];

        PartialDerivative[0] = LeftB;
        PartialDerivative[GivenNum - 1] = RightB;

        delete[] a;
        delete[] b;
        delete[] c;
        delete[] d;
        delete[] gm;
        delete[] bt;
        delete[] f;
        delete[] h;
    }

    //implementation of a single interpolation
    bool Spline::SinglePointInterp(const double& x, double& y) throw(SplineFailure)
    {
        try
        {
            if ((x < MinX) || (x > MaxX))
                throw SplineFailure("Extrapolation is not supported");
            int klo, khi, k;
            klo = 0; khi = GivenNum - 1;
            double hh, bb, aa;

            while (khi - klo > 1) // binary method to find the interval segment where x is located
            {
                k = (khi + klo) >> 1;
                if (GivenX[k] > x) khi = k;
                else klo = k;
            }
            hh = GivenX[khi] - GivenX[klo];

            aa = (GivenX[khi] - x) / hh;
            bb = (x - GivenX[klo]) / hh;

            y = aa * GivenY[klo] + bb * GivenY[khi] + ((aa * aa * aa - aa) * PartialDerivative[klo] + (bb * bb * bb - bb) * PartialDerivative[khi]) * hh * hh / 6.0;
            return true;
        }
        catch (const std::exception&)
        {

        }
    }

    //implementation of multiple interpolation
    bool Spline::MultiPointInterp(const double* x, const int& num, double* y)throw(SplineFailure)
    {
        for (int i = 0; i < num; i++)
        {
            SinglePointInterp(x[i], y[i]);
        }
        return true;
    }

    // Implementation of automatic multiple interpolation
    bool Spline::AutoInterp(const int& num, double* x, double* y) throw(SplineFailure)
    {
        if (num < 2)
            throw SplineFailure("At least two points should be output");
        double perStep = (MaxX - MinX) / (num - 1);

        for (int i = 0; i < num; i++)
        {
            x[i] = MinX + i * perStep;
            SinglePointInterp(x[i], y[i]);
        }
        return true;
    }

    Spline::~Spline()
    {
        delete[] PartialDerivative;
    }

    //implementation of exception class
    SplineFailure::SplineFailure(const char* msg) :Message(msg) {};
    const char* SplineFailure::GetMessage() { return Message; }

}