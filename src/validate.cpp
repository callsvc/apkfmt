#include <vector>
#include <array>
#include <fstream>
#include <mbedtls/sha256.h>

#include <validate.h>
#include <types.h>
namespace apkfmt {
    void Validate::doChecksum(std::stringstream& stream, const std::filesystem::path& origin) {
        mbedtls_sha256_context aes;
        mbedtls_sha256_init(&aes);
        std::vector<u8> ioAaa(file_size(origin));

        std::fstream verify(origin);
        verify.read(reinterpret_cast<char*>(ioAaa.data()), static_cast<std::streamsize>(ioAaa.size()));

        mbedtls_sha256_update(&aes, ioAaa.data(), ioAaa.size());
        std::array<u8, 32> sha{};
        mbedtls_sha256_finish(&aes, &sha[0]);

        for (const auto hex : sha) {
            constexpr auto hexStr{"0123456789abcdef"};
            stream << hexStr[hex >> 4 & 0xf];
            stream << hexStr[hex & 0xf];
        }

        mbedtls_sha256_free(&aes);
    }
}
