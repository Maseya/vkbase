#include "ImageBase.hxx"

namespace maseya::vkbase {
ImageBase::ImageBase(VkFormat format, VkImageType image_type, const VkExtent2D& extent,
                     VkImage image)
        : format_(format), image_type_(image_type), extent_(extent), image_(image) {}
}  // namespace maseya::vkbase