# Data Preprocessor

## Table of contents

 - [Cloning](#cloning)
 - [Build Dependencies](#build-dependencies)
 - [Compiling the application](#compiling-the-application)
 - [Using the Application](#using-the-application)
 - [Data Input format](#data-input-format)
 - [Verify Data Integrity](#verify-data-integrity)

## Cloning

```bash
git clone https://github.com/Research-Project-Crypto/DataPreprocessor.git --recursive
```

If you forgot to clone recursively you can use the following command:
```bash
git submodule update --init --recursive
```

## Build Dependencies

The following instructions are made for Arch based linux systems but they will give you an idea on how to port it to any other systems.

```bash
pacman -S --noconfirm --needed gcc make
pacman -S --noconfirm --needed premake

yay -S ta-lib
# or
paru -S ta-lib
```

## Compiling the application

```bash
premake5 gmake2

make config=release
```

## Using the application

### With arguments

|Position|Argument|
|--|--|
|1|Data input folder|
|2|Data output folder|

Example:
```bash
DataPreprocessor data/input data/output
```

#### Downside of argument only

With argument only mode you are unable to specify the type of the input data, you can only parse CSV text data.

### Without argument using settings.json

If you don't give any arguments the application will default to reading the settings from settings.json.

```json
{
    "input": {
        "input_folder": "data/input",
        "is_binary": false
    },
    "output": {
        "output_folder": "data/output"
    }
}
```

## Data Input Format

### CSV

The csv reader expects 6 fields of which all of them should be double floating point numbers.

```csv
event_time,open,close,high,low,volume
```

### Binary

Usually you won't need this mode unless you've used the [TickerTimescaleSwap](https://github.com/Research-Project-Crypto/TickerTimescaleSwap), then you NEED to set the `is_binary` value to true in settings.json.

## Verify Data Integrity

Included with this project is a python script with which you can verify the binary output data.

```bash
python3 scripts/binary_reader.py
```
`requires numpy`

It will loop over all the cells slowly, this mostly to shortly verify calculation mistakes in the program.
