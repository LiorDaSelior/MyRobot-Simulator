#include <exception>
#include <string>

class SimulatorException : public std::exception {
private:
    std::string filename;
public:
    explicit SimulatorException(const std::string filename_) : filename(filename_) {}
    virtual ~SimulatorException() noexcept = default;
    const char* what() const noexcept {
        return "File error while running Simulator";
    };
    virtual std::string getMessage() const noexcept = 0;
    std::string getFilename() {
        return filename;
    };
};

class HouseFailedToOpenFile : public SimulatorException {
private:
    std::string filepath;
public:
    explicit HouseFailedToOpenFile(const std::string house_name, const std::string house_filepath) 
    : SimulatorException(house_name), filepath(house_filepath) {}

    virtual std::string getMessage() const noexcept override {
        return ("Failed to open house file: " + filepath);
    }
};

class HouseFailedToParseFile : public SimulatorException {
private:
    std::string filepath;
public:
    explicit HouseFailedToParseFile(const std::string house_name, const std::string house_filepath) 
    : SimulatorException(house_name), filepath(house_filepath) {}

    virtual std::string getMessage() const noexcept override {
        return ("Failed to parse house file: " + filepath);
    }
};

class HouseMissingAttributeInFile : public SimulatorException {
private:
    std::string filepath;
    std::string attribute;
public:
    explicit HouseMissingAttributeInFile(const std::string house_name, const std::string house_filepath,  const std::string house_attribute) 
    : SimulatorException(house_name), filepath(house_filepath), attribute(house_attribute) {}

    virtual std::string getMessage() const noexcept override {
        return ("Missing attribute \"" + attribute + "\" in house file: " + filepath);
    }
};

class HouseInvalidDockingStationAmountInFile : public SimulatorException {
private:
    std::string filepath;
    std::string amount;
public:
    explicit HouseInvalidDockingStationAmountInFile(const std::string house_name, const std::string house_filepath, std::string amount_) 
    : SimulatorException(house_name), filepath(house_filepath), amount(amount_) {}

    virtual std::string getMessage() const noexcept override {
        return ("Invalid amount of docking stations (" + amount  + ") in house file: " + filepath);
    }
};

class AlgorithmFailedRegistration : public SimulatorException {
private:
    std::string filepath;
public:
    explicit AlgorithmFailedRegistration(const std::string algo_name, const std::string algo_filepath) 
    : SimulatorException(algo_name), filepath(algo_filepath) {}

    virtual std::string getMessage() const noexcept override {
        return ("Failed dlopen on algorithm library: " + filepath);
    }
};