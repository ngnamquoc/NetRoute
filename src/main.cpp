#include <iostream>
#include <pcap/pcap.h>
#include <memory>
#include <stdexcept>
#include "ethernet.hpp"
#include <cstdint>
#include <iomanip>
#include "ipv4.hpp"
/** takes a 6 bytes MAC address arr (e.g. {0x00, 0x1A, 0x2B, 0x03, 0x04, 0xFF})
 * and prints it in the standard MAC address format (e.g. 00:1a:2b:03:04:ff)
 */
void print_mac(const std::array<unsigned char, 6> &mac)
{ // passes the arr by reference (avoid a copy)
    // save the current output formatting before changing the settings
    const auto previous_flags = std::cout.flags(); // current formatting flag
    const auto previous_fill = std::cout.fill();   // current padding char

    std::cout << std::hex << std::setfill('0'); // print int in hex, use 0 for padding
    for (std::size_t i = 0; i < mac.size(); ++i)
    {
        if (i != 0)
        {
            std::cout << ':'; // print colon between
        }
        // setw() to set min width of next formatted output
        std::cout << std::setw(2) << static_cast<unsigned int>(mac[i]);
    }
    // restore stdout settings
    std::cout.flags(previous_flags);
    std::cout.fill(previous_fill);
}

void print_ethernet(const EthernetHeader &header)
{
    std::cout << " Source MAC: ";
    print_mac(header.source);

    std::cout << " Destination MAC: ";
    print_mac(header.destination);

    const auto previous_flags = std::cout.flags();
    const auto previous_fill = std::cout.fill();

    std::cout << "\n  Type/length: 0x"
              << std::hex << std::setfill('0') << std::setw(4)
              << header.type_or_length << '\n';

    std::cout.flags(previous_flags);
    std::cout.fill(previous_fill);
}

void print_ipv4_address(
    const std::array<unsigned char, 4> &address)
{
    for (std::size_t i = 0; i < address.size(); ++i)
    {
        if (i != 0)
        {
            std::cout << '.';
        }

        std::cout << static_cast<unsigned int>(address[i]);
    }
}

void print_ipv4(const IPv4Header &header)
{
    std::cout << "  Source IP: ";
    print_ipv4_address(header.source);

    std::cout << "\n  Destination IP: ";
    print_ipv4_address(header.destination);

    std::cout << "\n  Header length: "
              << static_cast<unsigned int>(header.header_length)
              << " bytes"
              << "\n  Total length: " << header.total_length
              << " bytes"
              << "\n  TTL: "
              << static_cast<unsigned int>(header.ttl)
              << "\n  Protocol: "
              << static_cast<unsigned int>(header.protocol)
              << '\n';
}

/** opens a capture file, reads one packet record at a time
 * and return how many records it found
 */
std::uint64_t inspect_capture(const char *path)
{
    char error_buffer[PCAP_ERRBUF_SIZE]{}; // space for err mess
    // define a CaptureHandle type that automatically closes the capture
    // std::unique_ptr is a smart pointer releasing a resource when the pointer goes out of scope
    // pcap_t represents a packet capture session
    // decltype(&pcap_close) -> tell unique_ptr to use a function pointer compatible with pcap_close() to release the resource
    using CaptureHandle = std::unique_ptr<pcap_t, decltype(&pcap_close)>; // create a type alias

    CaptureHandle capture{
        pcap_open_offline(path, error_buffer),
        &pcap_close};
    if (!capture)
    {
        throw std::runtime_error(error_buffer);
    }
    if (pcap_datalink(capture.get()) != DLT_EN10MB)
    {
        throw std::runtime_error(
            "Unsupported capture link type: expected Ethernet");
    }
    std::uint64_t packet_count = 0;
    while (true)
    {
        pcap_pkthdr *header = nullptr; // pcap_pkthdr is a structure provided by libpcap to store metadata about a captured packet.
        const unsigned char *data = nullptr;
        // Ask libpcap for the next record
        const int result = pcap_next_ex(capture.get(), &header, &data);
        if (result == 1)
        {
            ++packet_count;
            std::cout << "Packet " << packet_count
                      << ": captured=" << header->caplen // header pointer is updated by libpcap
                      << ", original=" << header->len << '\n';

            const std::span<const unsigned char> packet{
                data,            // points to the beginning of packet's bytes in memory
                header->caplen}; // number of elements

            const auto ethernet = parse_ethernet(packet);

            if (!ethernet)
            {
                std::cout << "  Truncated Ethernet header\n";
                continue;
            }

            print_ethernet(*ethernet); //*ethernet accesses the successfully parsed header

            constexpr std::uint16_t ipv4_ether_type = 0x0800;

            if (ethernet->type_or_length != ipv4_ether_type)
            {
                std::cout << "  Skipping: not a directly carried IPv4 packet\n";
                continue;
            }

            constexpr std::size_t ethernet_header_size = 14;

            // View the bytes after the Ethernet header without copying them.
            const auto ip_packet = packet.subspan(ethernet_header_size);

            const auto ipv4 = parse_ipv4(ip_packet);

            if (!ipv4)
            {
                std::cout << "  Invalid or truncated IPv4 header\n";
                continue;
            }

            print_ipv4(*ipv4);

            if (ip_packet.size() < ipv4->total_length)
            {
                std::cout << "  IPv4 payload is truncated in this capture\n";
            }
        }
        else if (result == PCAP_ERROR_BREAK)
        {
            // reach the end of the capture file
            break;
        }
        else if (result == PCAP_ERROR)
        {
            throw std::runtime_error(pcap_geterr(capture.get()));
        }
    }
    return packet_count;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <capture.pcap>\n";
        return 1;
    }
    try
    {
        const auto packet_count = inspect_capture(argv[1]);
        std::cout << "Capture file: " << argv[1] << "\n";
        std::cout << "Packet records: " << packet_count << "\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}