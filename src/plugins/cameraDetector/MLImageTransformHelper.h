#ifndef MLX_IMAGE_TRANSFORM_HELPER_H_
#define MLX_IMAGE_TRANSFORM_HELPER_H_
#include <VmbTransform.h>
#include <QImage>
#include "PrjCommon/service/MLFrame.h"

using namespace CORE;

namespace ImageHelper
{
    bool MLIamgeTransform(QImage& Dst, void* Src, MLFrame* Frame);
}
#endif // !MLX_IMAGE_TRANSFORM_HELPER_H_

