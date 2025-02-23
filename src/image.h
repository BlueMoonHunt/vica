#include<filesystem>
#include "uuid.h"

namespace vica {
    class Image {
    public:
        Image(const std::filesystem::path& path);
        // TODO: Implement this constructor
        // Image(const unsigned char* imageData);
        Image(const char* name, void* data, const uint32_t size, const uint32_t width, const uint32_t height);
        ~Image();

        uint32_t getWidth() const { return m_Width; }
        uint32_t getHeight() const { return m_Height; }
        const std::string& getName() const { return m_Name; }
        uint32_t getID() const { return m_ImageID; }

        bool operator==(const Image& other) const;
        void bind(uint32_t slot = 0) const;

        inline const std::filesystem::path& getPath() const { return m_Path; }
    private:
        std::filesystem::path m_Path;
        std::string m_Name;
        uint32_t m_Width, m_Height;
        uint32_t m_InternalFormat, m_DataFormat;
        uint32_t m_ImageID;
    };
} // namespace vica
