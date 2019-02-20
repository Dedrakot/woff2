/* A very simple commandline tool for decompressing woff2 format from stdin to stdout. */

#include <cstdio>
#include <iostream>
#include <fstream>
#include <iterator>
#include <ext/stdio_filebuf.h>
#include <woff2/decode.h>


bool readInput(std::string &input);
void writeOutput(const std::string &input, size_t size);

int main(int argc, char **argv) {
    using std::string;

    string input;

    if (!readInput(input)) {
        return EXIT_FAILURE;
    }

    const uint8_t *raw_input = reinterpret_cast<const uint8_t *>(input.data());
    string output(std::min(woff2::ComputeWOFF2FinalSize(raw_input, input.size()),
                           woff2::kDefaultMaxSize), 0);
    woff2::WOFF2StringOut out(&output);

    const bool ok = woff2::ConvertWOFF2ToTTF(raw_input, input.size(), &out);

    if (!ok) {
        return EXIT_FAILURE;
    }

    writeOutput(output, out.Size());

    return EXIT_SUCCESS;
}

const std::size_t INIT_BUFFER_SIZE = 131072;

bool readInput(std::string &input) {
    try {
        // on some systems you may need to reopen stdin in binary mode
        // this is supposed to be reasonably portable
        std::freopen(nullptr, "rb", stdin);

        if (std::ferror(stdin))
            throw std::runtime_error(std::strerror(errno));

        std::size_t len;
        std::array<char, INIT_BUFFER_SIZE> buf;

        while ((len = std::fread(buf.data(), sizeof(buf[0]), buf.size(), stdin)) > 0) {
            if (std::ferror(stdin) && !std::feof(stdin))
                throw std::runtime_error(std::strerror(errno));

            input.append(buf.data(), buf.data() + len);
        }

        return true;
    }
    catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

void writeOutput(const std::string &output, size_t size) {
    std::copy(output.begin(), output.begin() + size,
            std::ostream_iterator<char>(std::cout));
}