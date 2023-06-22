# Config File Parser in C

This is a C language library for managing configuration files. It provides functionality to read, write, and modify configuration files. 

## Features

* **Reading a configuration file:** Load a configuration file from the disk.
* **Writing to a configuration file:** Modify an existing configuration file, or create a new one if it doesn't exist.
* **Retrieving a value:** Retrieve the value associated with a given key in a specified section.
* **Setting a value:** Set a new key-value pair in a specified section, or update the value if the key already exists.
* **Deleting a section:** Delete a specified section from the configuration file, along with all the key-value pairs within it.
* **Deleting a key-value pair:** Delete a specific key-value pair from a given section.
* **Updating a key-value pair:** Update a specefic value from given section and key.
* **Check if key-exists:** Check if specefic section and key is exists or not.
* **Check if the configuration file is empty:** Use isEmpty() to check if the configuration file is empty (contains no sections) or not.
* **Get keys from a section:** Retrieves and prints all keys in a specified section of the configuration file.

## Usage

First, initialize a `Config` object:

### Examples 

1. Create, add sections, delete sections, and delete a key-value pair, and update key-value pair, check key is exists or not in section

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
        
        // delete a key-value pair
        config.deleteKey("Language", "Web");
        
        config.deleteSection("Amin");

        // update a key-value pair
        config.update("Language", "lang", "C++");

        if (config.keyExists("Language1", "lang"))
            printf("Key exists\n");
        else
            printf("Key does not exist\n");

        printf("%s\n", config.get("Language", "jobs"));
        printf("%s\n", config.get("Jobs", "jobss"));
        // the following two will print nothing as the section "Amin" has been deleted
        printf("%s\n", config.get("Amin", "Ip"));
        printf("%s\n", config.get("Amin", "Best"));

        config.showConfig();
        
        getchar();
        return 0;
    }

    ```

2. You can use the isEmpty() function like this:

    ```c
    if (config.isEmpty())
        printf("Config is empty\n");
    else 
        printf("Config is not empty\n");

    ```

3. retrieve all keys from a section:

    ```c
    #include <stdio.h>
    #include "Config.h"

    int main(int argc, char **argv)
    {
        Config config;

        initializeConfig(&config); // just belong this config 
        config.setConfigFileName("config.ini");

        config.getKeysFromSection("Language");
        
        getchar();
        return 0;
    }
    ```