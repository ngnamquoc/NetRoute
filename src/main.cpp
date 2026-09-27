#include <iostream>
#include <pcap/pcap.h>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <capture.pcap>\n";
        return 1;
    }
    const char* capture_path = argv[1];
    std::cout << "Capture files: " << capture_path << "\n";
    std::cout << "Using " << pcap_lib_version() << "\n";
    return 0;
}