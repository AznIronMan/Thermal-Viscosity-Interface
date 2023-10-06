#include <boost/asio.hpp>  // For serial communication
#include <iostream>  // For console I/O
#include <vector>  // For dynamic arrays
#include <Eigen/Dense>  // For matrix operations
#include <cmath>  // For mathematical functions
#include <map>  // For lookup table

// Lookup table for converting thermal conductivity to viscosity
std::map<double, double> thermal_to_viscosity = {
    {0.1, 1.0}, {0.2, 1.1}, {0.3, 1.2}, {0.4, 1.3}, 
    {0.5, 1.4}, {0.6, 1.5}, {0.7, 1.6}, {0.8, 1.7}, 
    {0.9, 1.8}, {1.0, 1.9}
};

// Default signal conditioning parameters
constexpr double default_gain = 1.0;
constexpr double default_offset = 0.0;

// Configuration for the curve-fitting algorithm
struct SignalConfig {
    double decayFactor = 0.1;  // Default decay factor for the curve fitting
};

// Parameters for signal conditioning (gain and offset)
struct SignalConditioning {
    double gain = default_gain;
    double offset = default_offset;
};

// Function prototypes - these will be filled in later
double lookupViscosity(double thermal_conductivity);  // Function to look up viscosity based on thermal conductivity
void getDataFromSerial(std::vector<double>& data);  // Reads data from the serial port
void formatData(const std::vector<double>& raw_data, Eigen::MatrixXd& formatted_data);  // Formats raw data into a matrix
void curveFitting(const Eigen::MatrixXd& data, Eigen::VectorXd& result);  // Applies curve fitting to the data
void averageAndDisplay(const Eigen::VectorXd& data);  // Calculates the average and displays it
void customCompilerFeature();  // Allows for runtime modification of certain parameters

int main() {
    try {
        std::vector<double> raw_data;  // To hold raw sensor data from the serial port
        Eigen::MatrixXd formatted_data;  // Matrix to store formatted data
        Eigen::VectorXd result;  // To hold the curve-fitted results
        SignalConfig config;  // Configuration for curve fitting
        SignalConditioning conditioning;  // Configuration for signal adjustments

        customSignalConditioning(conditioning);  // User can modify signal gain and offset here

        // Collects data from the serial port and applies any signal conditioning
        getDataFromSerial(raw_data, conditioning);  

        // Formats raw data into a matrix
        formatData(raw_data, formatted_data);  

        // Example lookup: Replace this with actual thermal conductivity values
        double example_thermal_conductivity = 0.1;  
        double example_viscosity = lookupViscosity(example_thermal_conductivity);

        // User can modify curve-fitting parameters like decay factor
        customCompilerFeature(config);  

        // Applies curve fitting on the formatted data
        curveFitting(formatted_data, result, config);  

        // Calculates and displays the average of the curve-fitted results
        averageAndDisplay(result);  
    } catch (const std::exception& e) {
        // Catches standard exceptions and prints them
        std::cerr << "Caught exception: " << e.what() << std::endl;  
        return 1;
    } catch (...) {
        // Catches any other exceptions
        std::cerr << "Caught unknown exception" << std::endl;  
        return 1;
    }

    return 0;  // Successful execution
}

// Function to look up viscosity based on thermal conductivity
double lookupViscosity(double thermal_conductivity) {
    // Check if the thermal conductivity value exists in the lookup table
    if (thermal_to_viscosity.find(thermal_conductivity) != thermal_to_viscosity.end()) {
        // If found, return the corresponding viscosity value
        return thermal_to_viscosity[thermal_conductivity];
    }
    // If the thermal conductivity value is not found, throw an exception
    throw std::invalid_argument("Invalid thermal conductivity value");
}

// Alternative function to get viscosity based on thermal conductivity
double getViscosity(double thermal_conductivity) {
    // Check if the thermal conductivity value exists in the lookup table
    if (thermal_to_viscosity.find(thermal_conductivity) != thermal_to_viscosity.end()) {
        // If found, return the corresponding viscosity value
        return thermal_to_viscosity[thermal_conductivity];
    }
    // If the thermal conductivity value is not found, return a sentinel value (-1.0)
    // Customer may want to handle this case explicitly in their application
    return -1.0;  
}

void getDataFromSerial(std::vector<double>& data, const SignalConditioning& conditioning) {
    using namespace boost::asio;

    try {
        io_service io;
        serial_port port(io, "/dev/ttyUSB0");  // IMPORTANT: Replace with your actual port name

        // Serial port configuration
        port.set_option(serial_port_base::baud_rate(9600));
        port.set_option(serial_port_base::character_size(8));
        port.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
        port.set_option(serial_port_base::parity(serial_port_base::parity::none));
        port.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));

        boost::asio::streambuf buffer;
        std::string raw_input;

        // Read until a newline character is encountered
        read_until(port, buffer, '\n');

        std::istream is(&buffer);
        std::getline(is, raw_input);

        // Parse the raw string data into individual double values
        std::istringstream iss(raw_input);
        data.reserve(1000);  // Preallocate memory for efficiency
        double value;
        while (iss >> value) {
            // Apply signal conditioning here. Adjust gain and add offset.
            value = value * conditioning.gain + conditioning.offset;
            data.push_back(value);
        }
    } catch (boost::system::system_error const& e) {
        // Catch any exceptions related to COM port issues
        throw std::runtime_error("Failed to open COM port: " + std::string(e.what()));
    }
}

// Formats raw data into a matrix for further processing.
// The size of the matrix is determined by the square root of the raw data size.
void formatData(const std::vector<double>& raw_data, Eigen::MatrixXd& formatted_data) {
    if (raw_data.empty()) {
        throw std::runtime_error("Raw data is empty");  // Ensure data is not empty
    }
    int size = std::sqrt(raw_data.size());  // Calculate matrix dimensions
    formatted_data.resize(size, size);  // Resize Eigen matrix
    int k = 0;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            formatted_data(i, j) = raw_data[k++];
        }
    }
}

// Averages the data vector and displays it to the console.
// Uses Eigen's sum() function for faster computation.
void averageAndDisplay(const Eigen::VectorXd& data) {
    double average = data.sum() / data.size();  // Eigen function used for sum
    std::cout << average << std::endl;
}

// Performs curve fitting on a given data matrix.
// Uses exponentially decaying least squares method.
// Takes into account the decay factor specified in SignalConfig.
void curveFitting(const Eigen::MatrixXd& data, Eigen::VectorXd& result, const SignalConfig& config) {
    if (data.rows() == 0 || data.cols() == 0) {
        throw std::runtime_error("Data matrix is empty");  // Ensure matrix is not empty
    }
    result.resize(data.cols());  // Resize result vector to match data columns
    for (int i = 0; i < data.cols(); ++i) {
        double sum = 0.0;
        for (int j = 0; j < data.rows(); ++j) {
            sum += data(j, i) * std::exp(-config.decayFactor * j);  // Exponential decay
        }
        result(i) = sum / data.rows();  // Averaging and storing in result
    }
}

// This function allows for customizing the signal processing parameters at runtime.
void customCompilerFeature(SignalConfig& config, SignalConditioning& conditioning) {
    std::string input;

    // Prompt for a new decay factor for curve fitting
    // This value affects how much the recent readings are weighted.
    std::cout << "Current decay factor: " << config.decayFactor << std::endl;
    std::cout << "Enter new decay factor or press Enter to keep default: ";
    std::getline(std::cin, input);
    try {
        // Update the decay factor or keep the default
        config.decayFactor = input.empty() ? config.decayFactor : std::stod(input);
    } catch (const std::invalid_argument&) {
        // Catch any conversion errors
        throw std::invalid_argument("Invalid input for decay factor");
    }

    // Prompt for a new gain for signal conditioning
    // This value scales the incoming data, useful for calibration.
    std::cout << "Current gain: " << conditioning.gain << std::endl;
    std::cout << "Enter new gain or press Enter to keep default: ";
    std::getline(std::cin, input);
    try {
        // Update the gain or keep the default
        conditioning.gain = input.empty() ? default_gain : std::stod(input);
    } catch (const std::invalid_argument&) {
        // Catch any conversion errors
        throw std::invalid_argument("Invalid input for gain");
    }

    // Prompt for a new offset for signal conditioning
    // This value shifts the incoming data, useful for zero-point adjustment.
    std::cout << "Current offset: " << conditioning.offset << std::endl;
    std::cout << "Enter new offset or press Enter to keep default: ";
    std::getline(std::cin, input);
    try {
        // Update the offset or keep the default
        conditioning.offset = input.empty() ? default_offset : std::stod(input);
    } catch (const std::invalid_argument&) {
        // Catch any conversion errors
        throw std::invalid_argument("Invalid input for offset");
    }
}

// Customizes signal conditioning parameters (gain and offset) for the incoming data
void customSignalConditioning(SignalConditioning& conditioning) {
    std::string input;
    
    // Display the current gain and ask the user if they want to change it
    std::cout << "Current gain: " << conditioning.gain << std::endl;
    std::cout << "Enter new gain or press Enter to keep default: ";
    std::getline(std::cin, input);
    try {
        // If input is empty, keep the default gain; otherwise, update with the new gain
        // The std::stod function converts string to double
        conditioning.gain = input.empty() ? default_gain : std::stod(input);
    } catch (const std::invalid_argument&) {
        // Handling incorrect input format for gain
        throw std::invalid_argument("Invalid input for gain");
    }

    // Same logic applies to the offset as it does for the gain
    std::cout << "Current offset: " << conditioning.offset << std::endl;
    std::cout << "Enter new offset or press Enter to keep default: ";
    std::getline(std::cin, input);
    try {
        // Update the offset or keep it default based on user input
        conditioning.offset = input.empty() ? default_offset : std::stod(input);
    } catch (const std::invalid_argument&) {
        // Handling incorrect input format for offset
        throw std::invalid_argument("Invalid input for offset");
    }
}
