# Setup Guide

## <a name='TableofContents'></a>Table of Contents

<!-- vscode-markdown-toc -->
- [Setup Guide](#setup-guide)
  - [Table of Contents](#table-of-contents)
  - [TTN](#ttn)
  - [Hardware Preparation](#hardware-preparation)
  - [Arduino Software Installation](#arduino-software-installation)
  - [Library Installation](#library-installation)
  - [Register a Device](#register-a-device)
  - [Payload Decoder](#payload-decoder)
  - [Arduino Code Upload](#arduino-code-upload)
  - [Node-RED](#node-red)
  - [MQTT](#mqtt)
  - [Configure SQL Database](#configure-sql-database)
  - [Deploy](#deploy)

<!-- vscode-markdown-toc-config
	numbering=false
	autoSave=false
	/vscode-markdown-toc-config -->
<!-- /vscode-markdown-toc -->

## <a name='TTN'></a>TTN

- Go to [The Things Network website](https://id.thethingsnetwork.org/) and sign up for an account.
- In [the TTN Console](https://console.cloud.thethings.network/), select the region and create a new application for the project

## <a name='HardwarePreparation'></a>Hardware Preparation

- Connect the Arduino MKR1310 to the Grove carrier board.
- Connect the Antenna to the micro UFL connector on the Arduino.
- Connect the Grove Relay to the D0 connector on the carrier board.
- Connect the Grove Temperature and Humidity sensor to the D2 connector on the carrier board.
- Connect the Grove LCD to the D6/TWI connector on the carrier board.
- Use a micro USB cable to connect the Arduino to a PC.

## <a name='ArduinoSoftwareInstallation'></a>Arduino Software Installation

- Download and install the Arduino IDE from the official website.
- Open the Arduino IDE.

## <a name='LibraryInstallation'></a>Library Installation

- In the Arduino IDE, go to "Sketch" -> "Include Library" -> "Add .ZIP Library..."
- Select `./Arduino_Code/Libraries/arduino-DHT-master.zip` to install the library for the Temperature and Humidity sensor.
- Repeat again, this time selecting `./Arduino_Code/Libraries/Grove_LCD_RGB_Backlight-master.zip` to install the library for the Grove LCD.
- To install the MKRWAN library, go to "Sketch" -> "Include Library" -> "Manage Libraries" and search for "MKRWAN" before clicking "Install".

## <a name='RegisteraDevice'></a>Register a Device

- In the Arduino IDE, go to "File" -> "Examples" -> "MKRWAN" -> "FirstConfiguration"
- Ensure that the Arduino is connected to the PC, select the correct board and port, and click the "Upload" button to flash the code to the Arduino.
- Open the serial monitor and note the module version number e.g."1.2.3" and the device EUI e.g."a7362a394820370c".
- On the TTN console, click "Register end device"
- Click “Select the end device in the LoRaWAN Device Repository” and enter the device brand (Arduino SA), the model (Arduino MKR WAN 1310), the hardware version, the firmware version (module version number from the serial monitor) and the region. 
- Under "Provisioning Information" enter the Join EUI (0000000000000000) and the Dev EUI (the device EUI from the serial monitor). Generate an AppKey and give an end device id (name) before clicking "Register end device" 
- In the Arduino serial monitor, type "1" and press send. Then enter the AppEUI (0000000000000000) followed by the AppKey (click on the newly created end device in the TTN consele and copy the AppKey from there).
- If the message is successfully sent then the device has been registered, if it fails, try moving the aerial.
  

## <a name='PayloadDecoder'></a>Payload Decoder

- On the TTN console, navigate to "End devices", select the device and go to "Payload formatters".
- Set the formatter type to "Custom Javascript Formatter" and copy and paste the formatter code from the `./Uplink_Payload_Formatter.js` file before saving the changes.

## <a name='ArduinoCodeUpload'></a>Arduino Code Upload

- Open the Arduino sketch `./Arduino_Code/Arduino_Code.ino` in the Arduino IDE.
- In the `./Arduino_Code/arduino_secrets.h` file set the `SECRET_APP_EUI` to the EUI of the TTN application (0000000000000000) and set the `SECRET_APP_KEY` to the the Access Key of the TTN application.
- Ensure that the Arduino is connected to the PC, select the correct board and port, and click the "Upload" button to flash the code to the Arduino.

## <a name='Node-RED'></a>Node-RED

- Install Node-RED on your PC or server using the [official installation guide](https://nodered.org/docs/getting-started/local).
- Open the terminal and navigate to the Node-RED project directory. Install the required Node modules by running the command:
  ```
  npm install node-red-dashboard node-red-node-mysql node-red-node-ui-table
  ```
- Start Node-RED by running the command:
  ```
  sudo node-red-start
  ```
- To see the Node-RED workspace, open a web browser (Firefox recommended) and navigate to http://localhost:1880.
- To import the Node-RED flow, click on the menu in the top right and select "Import", then select the `./Node_RED.json` file and click "Import".

## <a name='MQTT'></a>MQTT

- On the TTN console, click "Integrations" and "MQTT".
- Click "Generate new API key" and take note of the details **especially the API key (password) as this can only be viewed once**.
- In the Node-RED workspace, double click on the MQTT in node and edit its properties.
- Click the pencil icon to edit the MQTT broker.
- On the "Connection" tab, type the server e.g."eu1.cloud.thethings.network" and the port e.g."8888", then select the "Use TLS" checkbox.
- On the "Security" tab enter the username and password from the MQTT credentials page on TTN.

## <a name='ConfigureSQLDatabase'></a>Configure SQL Database

**The following instructions presume that MariaDB is being set up on a Debian-based Linux PC or server. If a different OS is being used please consult the [official installation guide](https://www.mariadbtutorial.com/getting-started/install-mariadb/)**

- Install MariaDB using the following commands:
  ```
  sudo apt update
  sudo apt install mariadb-server
  ```
- Start the MariaDB service
  ```
  sudo systemctl start mariadb
  ```
- (Optional) To enable the service to start automatically at boot time run the following command:
  ```
  sudo systemctl enable mariadb
  ```
- Log into MariaDB as root with the password set at installation
  ```
  mysql -u root -p
  ```
- Create a new database
  ```
  CREATE DATABASE Harvest_Predictor;
  ```
- Create a new user (Change myuser and mypassword) an grant them all privileges on all tables in the database
  ```
  CREATE USER 'myuser'@'localhost' IDENTIFIED BY 'mypassword';
  GRANT ALL PRIVILEGES ON Harvest_Predictor.* TO 'myuser'@'localhost';
  FLUSH PRIVILEGES;
  ```
- Type `exit` and press enter to quit the MariaDB shell.
- Log back into MariaDB as the user created earlier with the password set
  ```
  mysql -u myuser -p
  ```
- Select the database
  '''
  USE Harvest_Predictor;
  '''
- Create the three required tables (temperatureReadings, crops, and averageTemperatures) using the script `./Create_SQL_Tables.sql`. Run the following command:
  '''
  source /path/to/script/Create_SQL_Tables.sql;
  '''
- Check that all three tables have been created using the following command:
  ```
  SHOW TABLES;
  ```
- In the Node-RED workspace, double-click on the MySQL/MariaDB node to open its configuration dialog and click on the pencil icon. Provide the details of the database connection:

    - Server: The hostname or IP address of the MySQL/MariaDB server. If the database is on the same machine as Node-RED, use "localhost."
    - Port: The port number for the MySQL/MariaDB server. The default is typically 3306.
    - User: The username for the MySQL/MariaDB database.
    - Password: The password for the specified user.
    - Database: The name of the specific database to connect to.

- Click the "Add" button to save this configuration.

## <a name='Deploy'></a>Deploy

- Start the MariaDB service
  ```
  sudo systemctl start mariadb
  ```
- In the Node-RED workspace, click the deploy button in the top right.
- Ensure that the Arduino is powered on.
- To view the incoming data and troubleshoot any issues, view the live data feed on the TTN console.
- To view the dashboard go to `http://localhost:1880/ui`.