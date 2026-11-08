#include "ethernet.hpp"

std::optional<EthernetHeader> parse_ethernet(std::span<const unsigned char> packet)
{
    // constexpr means the var must be initialized using a compile-time constant expression
    // this value is known at compile time and is not intended to change
    constexpr std::size_t ethernet_header_size = 14; // std::size_t is used represent object sizes and arr indices
    if (packet.size() < ethernet_header_size)
    {
        return std::nullopt;
    }
    EthernetHeader header{};
    // extract des and source IPs in parallel
    for (std::size_t i = 0; i < 6; ++i)
    {
        header.destination[i] = packet[i];
        header.source[i] = packet[i + 6];
    }
    header.type_or_length = static_cast<std::uint16_t>( //static_cast<target_type>(value)
        // bitwise operations: &,|,^,~,<<,>>
        (static_cast<unsigned int>(packet[12]) << 8) //left shift 8 bits, we cast to unsigned int here due to integer promotion
        | static_cast<unsigned int>(packet[13]) //
    );
    return header;
}