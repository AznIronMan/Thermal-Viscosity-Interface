# Thermal Viscosity Interface

This C++ program is designed for interfacing with hardware to collect and process real-time data. It features curve-fitting algorithms, real-time data manipulation, and configurable compiler features.

## Requirements

- C++ Compiler (e.g., g++)
- Boost Libraries
- Eigen Library

## Installation

### Installing C++ Compiler

For Ubuntu, you can install the g++ compiler using:

```bash
sudo apt update
sudo apt install g++
```

For macOS, install Xcode Command Line Tools:

```bash
xcode-select --install
Installing Boost Libraries
```

### Install Boost Libraries

For Ubuntu:

```bash
sudo apt update
sudo apt install libboost-all-dev
```

For macOS:

```bash
brew install boost
Installing Eigen Library
```

### Install Eigen Library

For Ubuntu:

```bash
sudo apt update
sudo apt install libeigen3-dev
```

For macOS:

```bash
brew install eigen
```

### Compilation and Running
- Save the code into a file named ThermalViscosityInterface.cpp.
- Open a terminal and navigate to the folder containing the code.
- Compile the code using:

```bash
g++ -o ThermalViscosityInterface ThermalViscosityInterface.cpp -lboost_system
```

- Connect your microprocessor board via USB.
- Run the program:

```bash
./ThermalViscosityInterface
```

- Follow the on-screen instructions to adjust the decay factor, gain, and offset as needed.

## Credits:

- Written by [Geoff Clark](mailto:geoff@clarktribegames.com) of [ClarkTribeGames, LLC](https://www.clarktribegames.com).

## License

[MIT](https://choosealicense.com/licenses/mit/)
