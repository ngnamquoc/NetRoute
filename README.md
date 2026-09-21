# NetRoute

**C++ Software Router & Telemetry Engine**

A personal portfolio project exploring C++ systems programming, Linux networking, and embedded Linux development. NetRoute will grow from an offline packet analyzer into an IPv4 software router with multithreaded processing and telemetry.

## Status

A minimal C++20 executable builds on macOS and prints the project name. Packet analysis, routing, and telemetry are planned features.

## Build and run

Requires CMake 3.20 or newer and a C++20-capable compiler. Run these commands from the project root:

```sh
# Configure a Debug build (first-time setup)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# Compile and link
cmake --build build

# Run the executable
./build/netroute
```

The configure command reads `CMakeLists.txt`, saves build settings in `build/`, and enables a Debug build for debugging. It does not compile the code.

After editing source files, rebuild and run:

```sh
cmake --build build && ./build/netroute
```

`&&` runs the executable only if the build succeeds. Configure again when creating a fresh build directory, building on another machine, or changing build options. Changes to `CMakeLists.txt` normally trigger automatic reconfiguration during the build.

## Roadmap

1. **Development setup:** C++20, CMake, a Debug build, and debugger support.
2. **Packet analysis:** Read captures with libpcap and implement Ethernet, IPv4, TCP, and UDP parsing, initially for unfragmented IPv4 traffic.
3. **Routing:** Longest-prefix matching and IPv4 forwarding in an isolated Linux network namespace lab.
4. **Multithreading:** Bounded queues, explicit packet ownership, and graceful shutdown.
5. **Telemetry:** Protocol counts, forwarding/drop statistics, queue depth, throughput, and processing latency.
6. **Automation:** Integration tests, CI, repeatable benchmarks, and Pi deployment documentation.

## Target platform

Raspberry Pi 4 Model B (8 GB), running Debian 13 on ARM64. Portable components are developed on macOS and will be built separately on the Pi. Linux networking integration will be developed and tested on the Pi through VS Code Remote SSH. The planned stack includes C++20, CMake, and libpcap.

## Planned deployment

The lab will run entirely on the Raspberry Pi. Two virtual networks will each contain one virtual endpoint, connected through a dedicated router namespace running NetRoute.

```mermaid
flowchart LR
    subgraph pi["Raspberry Pi 4 — Debian Linux"]
        subgraph left["Virtual network A"]
            a["Endpoint A namespace"]
        end
        subgraph router["Router namespace — kernel IP forwarding disabled"]
            ingress["Virtual interface A"]
            netroute["NetRoute C++ process"]
            egress["Virtual interface B"]
            ingress <--> netroute <--> egress
        end
        subgraph right["Virtual network B"]
            b["Endpoint B namespace"]
        end
        a <-->|"veth pair"| ingress
        egress <-->|"veth pair"| b
    end
```

NetRoute will forward IPv4 packets between the networks. Linux kernel IP forwarding will be disabled in the router namespace so it cannot bypass NetRoute. The lab will verify that the endpoints communicate while NetRoute runs and lose connectivity when it stops. The Pi's SSH management connection will remain outside the lab namespaces.
