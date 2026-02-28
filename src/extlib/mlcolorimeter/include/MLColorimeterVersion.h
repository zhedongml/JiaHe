// Copyright 2025 MLOptic

#ifndef SRC_MLCOLORIMETER_MLCOLORIMETERVERSION_H_
#define SRC_MLCOLORIMETER_MLCOLORIMETERVERSION_H_
#include <string>

#include "mlcolorimeter_global.h"

namespace ML {
namespace MLColorimeter {
class MLCOLORIMETER_EXPORT MLColorimeterVersion {
 public:
    std::string ML_GetColorimeterVersion(
        std::string dll_path = "\\MLColorimeterInterface.dll");
};

}  // namespace MLColorimeter
}  // namespace ML

#endif  // SRC_MLCOLORIMETER_MLCOLORIMETERVERSION_H_
