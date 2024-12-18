# SCanner

**SCanner** is a CAN bus blackbox and live telemetry system developed by [Squadra Corse PoliTO](https://github.com/squadracorsepolito), the Formula SAE team of Politecnico di Torino. This project is designed to monitor and record CAN bus data in real-time, providing valuable insights for vehicle diagnostics and performance analysis.

## Features

- **Real-Time Data Monitoring**: Capture and display live telemetry data from the vehicle's CAN bus.
- **Data Logging**: Record CAN bus traffic for post-analysis and debugging.
- **Blackbox Functionality**: Store critical data during events for later review.

## Getting Started

To set up and run SCanner on your system, follow these steps:

### Prerequisites

- **Hardware**: STM32H743VGTx microcontroller
- **Software**:
  - A toolchain supporting Makefile-based projects (e.g., GCC for ARM, OpenOCD, etc.)

### Installation

1. **Clone the Repository**:

   ```bash
   git clone --recursive https://github.com/squadracorsepolito/SCanner.git
   cd SCanner
   ```

2. **Build the Project**:

   Use the `STM32Make.make` file to build the project:

   ```bash
   make -f STM32Make.make
   ```

3. **Flash the Microcontroller**:

   The recommended way to flash the microcontroller is to use the following `make` command:

   ```bash
   make -f STM32Make.make flash
   ```

   Alternatively, you can flash the compiled binary manually using OpenOCD:

   ```bash
   openocd -f interface/your_interface.cfg -f target/stm32h7x.cfg -c "program your_binary.elf verify reset exit"
   ```

## Usage

1. **Connect Hardware**:

   Ensure the microcontroller is connected to the vehicle's CAN bus.

2. **Run the System**:

   Power on the device to start monitoring and logging CAN bus data.

3. **Access Data**:

   - **Live Data**: Use [Cannelloni](https://github.com/mguentner/cannelloni) to read data live from the CAN bus.
   - **Logged Data**: Retrieve logged data from the storage for analysis.

## Contributing

We welcome contributions from the community. To contribute:

1. **Fork the Repository**:

   Click the "Fork" button at the top right of this page.

2. **Create a Branch**:

   ```bash
   git checkout -b feature/YourFeatureName
   ```

3. **Commit Your Changes**:

   ```bash
   git commit -m 'feat: Add some feature'
   ```

4. **Push to the Branch**:

   ```bash
   git push origin feature/YourFeatureName
   ```

5. **Open a Pull Request**:

   Navigate to your forked repository and click the "New Pull Request" button.

## License

"THE BEER-WARE LICENSE" (Revision 69):
Squadra Corse firmware team wrote this project. As long as you retain this notice
you can do whatever you want with this stuff. If we meet some day, and you
think this stuff is worth it, you can buy us a beer in return.

Authors
- Filippo Rossi <filippo.rossi.sc@gmail.com>
- Federico Carbone <federico.carbone.sc@gmail.com>

## Acknowledgments

Special thanks to the members of [Squadra Corse PoliTO](https://github.com/squadracorsepolito) for their dedication and contributions to this project.

For more information, visit our [GitHub page](https://github.com/squadracorsepolito/SCanner).

