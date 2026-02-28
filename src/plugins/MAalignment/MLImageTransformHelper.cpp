#include "MLImageTransformHelper.h"

namespace ImageHelper 
{
    bool MLIamgeTransform(QImage& Dst, void* Src, MLFrame* Frame)
    {
        VmbError_t Result;
        if (Dst.isNull())
        {
            return false;
        }
        VmbImage                    source_image;
        VmbImage                    destination_image;
        source_image.Size = sizeof(source_image);
        destination_image.Size = sizeof(destination_image);
        std::string pixel_in;
        switch (Frame->format)
        {
        case MLPixelFormat::kMLMono8:
            pixel_in = "Mono8";
            break;
        case MLPixelFormat::kMLMono12:
            pixel_in = "Mono12";
            break;
        case MLPixelFormat::kMLMono16:
            pixel_in = "Mono16";
            break;
        default:
            pixel_in = "BGR24";
            break;
        }
        Result = VmbSetImageInfoFromString(pixel_in.c_str(),
            static_cast<VmbUint32_t>(pixel_in.size()),
            static_cast<VmbUint32_t>(Frame->width),
            static_cast<VmbUint32_t>(Frame->height),
            &source_image);
        if (VmbErrorSuccess != Result)
        {
            return false;
        }

        unsigned int     dst_bits_per_pixel = Dst.depth();
        std::string pixel_out;
        switch (dst_bits_per_pixel)
        {
        default:
            return false;
        case 8:
            pixel_out = "Mono8";
            break;
        case 24:
            pixel_out = "BGR24";
            break;
        case 32:
            pixel_out = "BGRA32";
            break;
        }
        Result = VmbSetImageInfoFromString(pixel_out.c_str(),
            static_cast<VmbUint32_t>(pixel_out.size()),
            static_cast<VmbUint32_t> (Dst.width()),
            static_cast<VmbUint32_t> (Dst.height()),
            &destination_image);
        source_image.Data = Src;
        destination_image.Data = Dst.bits();
        Result = VmbImageTransform(&source_image, &destination_image, NULL, 0);
        if (VmbErrorSuccess == Result)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}