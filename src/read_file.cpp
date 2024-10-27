#include "read_file.hpp"

#include <fstream>

#include <spdlog/spdlog.h>

std::vector<uint8_t> read_file(const std::string &path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        spdlog::error("read_file: failed to open file {}", path);
        throw std::runtime_error("failed to open file");
    }
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> data(file_size, 0);
    file.read(reinterpret_cast<char *>(data.data()), file_size);

    return data;
}
