![nectarbuzzz logo](http://static1.squarespace.com/static/5754533555598617f52d7d0c/t/582fb67e6b8f5bca572ab4c9/1481230873609/?format=200w)

# BeeSystem

#### System to monitor a bee hive. 

We are a team of electrical and computer engineers students from **Université Laval** that worked in collaboration with [nectar.buzz](www.nectar.buzz) to build a system that could change the honey industry. Our system can monitor temperature, humidity, weight, sound amplitude and sound frequencies of a bee hive.

Don't hesitate to ask questions.

## Captors

- **Temperature & Humidity** : HIH8120
- **Weight** : 4x SEN-10245
- **Microphone** : MP33AB01

## BeeOs

Our microcontroller is the [STM32l0538-Discovery](http://www.st.com/en/evaluation-tools/32l0538discovery.html). We coded a minimal operating system to control the monitoring and the reception of commands. Here is a brief description of the code.

### Dependencies 

1. STM32l0 compilator (included with this [IDE](http://www.openstm32.org/HomePage))
2. [HAL Library](http://bit.ly/2gssXnF)

### Installation Instructions

1. Install an IDE ([OpenSTM](http://bit.ly/2hn5fuF) is a great choice)
2. Create a new project with the source code provided by us
3. Compile the code
4. Flash the code on the STM32l0

### Code Structure

- **command** : Protocol commands between computer and STM32l0 
- **communication** : Communication with the XBee to send on WiFi
- **config** : Regarding configuration of the OS
- **filesystem** : Minimal filesystem on RAM to keep data before sending
- **init** : Initialization functions et interrupt callback functions
- **liir** : Code to generate signal filters
- **main** : Main function, starting point of the OS
- **sleep** : Code to manage the sleep/wakeup cycle of the STM32l0
- **take_measures** : Code to read captors measures

### OS Configuration

The OS can be configured by the computer station or by changing the code. The former is prefered unless you are an expert. The OS wake up every minutes.

| Constraints  | Config | What's that ? |
|---|---|---|
| 1 <= t < 100 |  measure_time | Second between measures |
| 1 <= n < 100 | sending_time | Number of packets before sending data |
| 1 <= f < 500 |  freqs | 3 sound frequencies to inspect |

### Data Packet Format

- Each data packet is **32 bytes**. 
- The OS can keep up to **100 packets** before sending all to the computer station. 
- For instance, if the measure\_time is 30s and the sending\_time is 4 packets, the STM32l0 will send each 2min

| Type  | Data |
|---|---|
| 4 bytes (Float) |  Timestamp indicator |
| 4 bytes (Float) | Temperature |
| 4 bytes (Float)  |  Humidity |
| 4 bytes (Float)  |  Weight |
| 4 bytes (Float)  |  Sound volume |
| 12 bytes (3 Float)  | Sound frequencies response |

### Command Packet Format

We use a basic command protocol on **5 bytes** between the computer station and the STM32l0

- The header is used to indicate the command (**256 commands potentially supported**)
- The arguments complete the associate command and can be encoded on 4 bytes maximum

| Type  | Part |
|---|---|
| 1 byte (int) |  Header |
| 4 bytes (int) | Arguments |

#### take_measures
The function take_measures is the function used to take the measure from the sensor. The code is devide into 3 sections (one for each of the captors). The function don't have parameter and don't produce any exit. The results of the reading from the captor is writen into the data handler.

#### Humidity & temperature sensor section
The humidity and temperature sensor work with I2C communication. The code create an handle for the I2C which is then use to initialize the communication with the HAL library. The sensor is read in block mode with the memory read function from the HAL library. The code then convert the reading of the sensor into data and is stored into the data handler.

#### Weight sensor section
The weight sensor work with SPI communication. The code create an handle for the SPI which is then use to initialize the communication with the HAL library. The SPI communication is used to communicate with an external ADC, which read the data from an switch. The switch provide the data from one of the 4 sensor of the balance and is controled by the microcontroler. The actual reading work by pooling, each sensors is read individualy (with the read function from the HAL library) 2 times to create a pool of 8 mesures. An average is done on the 8 mesures, the results is then stored into the data handler.

#### Microphone section
The microphone is used to get two informations: the ambiant volume and the frequency in the hive. The microphone is linked to the internal ADC of the microcontroler. Like the 2 precedent section, the initializing and the reading of the ADC is handled by the HAL library. The first reading of the volume is send at the data handler as the actual volume of sound of the hive. It is to note that the system dont messure the frequency in the hive, but compare 3 differents regions of frequency to determinate to domminant one.

For the frequency, the data is process by 3 real time IIR filter with fellowing spec: butterwoth filter, 6th order, sampling rate of 2000 hertz, 1000 points, frequency cutoff given by the user ([ 200 Hz , 300 Hz ], [ 300 Hz , 400 Hz ] and [ 400 Hz , 500 Hz ] as default value). For the actual reading, 3 consecutives and differents samples are use for the differents filters. The sum of the answer of the 3 filters are logged in the data handle as the power, which can be use to determine the domminant zone.

#### load_coefficient
The function load_coefficent is use to calculate the coefficient for the butterworth IIR filter. The function takes 3 parameters, which are the middle frequency of the cutoff frequency for the filter (for exemple: the value 250 gives a cutoff frequency of [ 300 Hz , 400 Hz ]). The coefficients are calculated using the butterworth library from Exstrom Laboratories LLC (the library had been adapt to work with the project).

## BeeFi

BeeFi is a gui app to send command and receive data from BeeOS

### Dependencies

1. [MongoDB](https://www.mongodb.com/) 
2. [pymongo](https://api.mongodb.com/python/current/)
3. [pip](https://pypi.python.org/pypi/pip)
4. [python 3.4](https://www.python.org/) and later (though support to 2.7 can be easily made)

### Usage to start the BeeFi

```bash
python gui.py
```

### Installation Instruction

1. Install MongoDB on your computer, follow instructions [here](https://docs.mongodb.com/manual/administration/install-community/)
2. Install pymongo, simple ```pip install pymongo```
3. Start MongoDB

### Code Structure

- **beefi** : communication module
- **database** : data related module
- **definitions** : macro definitions
- **gui** : Entry point, start the gui
