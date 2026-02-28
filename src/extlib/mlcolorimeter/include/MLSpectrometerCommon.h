#ifndef MLSPECTROMETERCOMMON_H
#define MLSPECTROMETERCOMMON_H

#include <string>

namespace ML {
namespace MLSpectrometer {
enum class InterpMethod { LAGRANGE = 1, SPLINE };

enum class SpecStatus {
    Spec_normal,
    Spec_max_iterations_reached,
    Spec_lowLevel,
    Spec_highLevel,
    Spec_error
};

struct SpectrumStruct {
    SpecStatus status;
    std::vector<double> spectrum;
    bool useFinalSpectrum = false;
};

struct SpectrometerInfo {
    bool Enable = false;
    std::string Type;
    std::string Name = "Spectrometer";
    int ConnectAddress;
};

struct MLCIE {
    double Cx;
    double Cy;
    double X;
    double Y;
    double Z;
};
}  // namespace MLSpectrometer
}  // namespace ML

#endif  // MLSPECTROMETERCOMMON_H