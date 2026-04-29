# Running Ada Programs on the NEORV32

This repository contains links to helpful material and code files to help you develop programs for the NEORV32

## Relevant Repositories
- **[Ada NEORV32 HAL](https://github.com/GNAT-Academic-Program/neorv32-hal/tree/main)** - Ada NEORV32 HAL Required to run Ada programs on the NEORV32
- **[Ada Compatible NEORV32 Setups Folder](https://github.com/GNAT-Academic-Program/neorv32-setups)** - As of March 29th, the Ada HAL works with only a specific fork of the NEORV32. Here is the setups folder fork
- **[Ada Compatible NEORV32 Fork](https://github.com/GNAT-Academic-Program/neorv32/tree/7349c409eea4e129d5d11b6ecbcfaea76c470709)** - As of March 29th, the Ada HAL works with only a specific fork of the NEORV32. Here is the NEORV32 fork itself 
- **[Input Output Helper Library](https://github.com/dipenarathod/Input-Output-Helper-Library-for-NEORV32-Ada-Projects)** - The base library used by the Wishbone projects (NPU and Camera Controller) - Ada and C versions available 
- **[Wishbone NPU](https://github.com/dipenarathod/Wishbone-NPU)** - Wishbone Peripheral used to accelerate common ML workloads in hardware
- **[Wishbone Camera Controller for OV5640](https://github.com/dipenarathod/Wishbone-Camera-Controller-for-OV5640)** - Wishbone Peripheral used to interface the Waveshare OV5640 Camera (Version C) with the NEORV32 (Ada and C versions of the libraries available)
- **[Wishbone Interconnect](https://github.com/dipenarathod/Wishbone-Interconnect-1-Master-2-Slaves)** - Wishbone Interconnect to connect 2 Wishbone Peripherals to a Master

## Video Guides:
- **[Running and Developing Ada Programs on the NEORV32](https://www.youtube.com/playlist?list=PLTuulhiizN0IIO0SsckqQsp6VUrNsisH5)** - How to run Ada programs on the NEORV32
- **[Deploying Models on the NEORV32 + NPU System](https://www.youtube.com/playlist?list=PLTuulhiizN0KNPv-PT1-1Z_EG6jP5cCUH)** - Playlist demonstrating the pipeline to deploy ML Models on the NEORV32 + NPU system
- **[NEORV32 + NPU Guide](https://www.youtube.com/playlist?list=PLTuulhiizN0KFKIZwFJnOU0KGaqpqDNzj)** - Playlist showing how to connect the NEORV32 to the Wishbone NPU in VHDL
- **[NEORV32 + Camera Controller Guide](https://www.youtube.com/playlist?list=PLTuulhiizN0K-HTymHKr1Nurv-iq_RdWK)** - Playlist showing how to connect the NEORV32 to the Wishbone Camera Controller for the OV5640 in VHDL
- **[NEORV32 + Interconnect Guide](https://www.youtube.com/playlist?list=PLTuulhiizN0KFKIZwFJnOU0KGaqpqDNzj)** - Playlist showing how to use the developed Wishbone Interconnect to connect the NEORV32 to the Camera Controller + NPU simultaneously

## Walthroughs + Features + Presentation
- **[NEORV32 + NPU + Camera Controller Features](https://youtu.be/YZdjy-vXNMk)** - Walkthrough of NEORv32 + NPU + Camera Controller VHDL files to show what to change to personalize the system/migrate to a new platform
- **[Quick Python convert_weight script Usage + Ada Project Setup](https://youtu.be/nBpLutpjnVk?si=aR93uhGkc-HtZxYG)** - Quickly go over how to use the Python convert_weights script + what to do to develop NEORV32 + NPU programs in Ada
- **[Project Overview](https://youtu.be/6WUX9J_9Snc)** - Presentation showcasing the project, system features, and worst case performance
