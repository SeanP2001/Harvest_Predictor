-- Create the 'crops' table
CREATE TABLE IF NOT EXISTS crops (
    id INT AUTO_INCREMENT PRIMARY KEY,
    cropName TINYTEXT NOT NULL,
    datePlanted DATE NOT NULL,
    timePlanted TIME NOT NULL,
    location TINYTEXT,
    baselineTemp FLOAT NOT NULL,
    harvestStartGDD INT NOT NULL,
    harvestEndGDD INT NOT NULL,
    accumulatedGDD FLOAT,
    harvestPercent FLOAT,
    predictedHarvestStart DATE,
    predictedHarvestEnd DATE
);

-- Create the 'temperatureReadings' table
CREATE TABLE IF NOT EXISTS temperatureReadings (
    ReadingNo INT AUTO_INCREMENT PRIMARY KEY,
    Date DATE,
    Time TIME,
    Device_ID TINYTEXT,
    Temperature FLOAT
);

-- Create the 'averageTemperatures' table
CREATE TABLE IF NOT EXISTS averageTemperatures (
    deviceID TINYTEXT,
    date DATE PRIMARY KEY,
    averageTemperature FLOAT
);
