# Config File Parser in C

This is a C language library for managing configuration files. It provides functionality to read, write, and modify configuration files. 

## Features

* **Reading a configuration file:** Load a configuration file from the disk.
* **Writing to a configuration file:** Modify an existing configuration file, or create a new one if it doesn't exist.
* **Retrieving a value:** Retrieve the value associated with a given key in a specified section.
* **Setting a value:** Set a new key-value pair in a specified section, or update the value if the key already exists.
* **Deleting a section:** Delete a specified section from the configuration file, along with all the key-value pairs within it.

## Usage

First, initialize a `Config` object:

### Examples 

1. create and add section also deleteSection 

    ```c
    #include <stdio.h>
    #include "Config.h"


    int main(int argc, char **argv)
    {
        Config config;

        initializeConfig(&config); // just belong this config 

        config.setConfigFileName("config.ini");
        config.addSection("Language");
        config.addSection("Jobs");
        config.addSection("Amin");

        config.set("Language", "jobs", "programmer");
        config.set("Language", "lang", "C");
        config.set("Language", "Web", "Php");
        config.set("Jobs", "jobss", "programmer");

        config.addSection("Love");

        config.set("Amin", "Best", "Scientis");
        config.set("Amin", "Ip", "192.168.90.90");
        config.set("Love", "person", "I dont know");
        
        config.deleteSection("Amin");

        printf("%s\n", config.get("Language", "jobs"));
        printf("%s\n", config.get("Jobs", "jobss"));
        printf("%s\n", config.get("Amin", "Ip"));
        printf("%s\n", config.get("Amin", "Best"));

        config.showConfig();
        
        getchar();
        return 0;
    }
    ```
