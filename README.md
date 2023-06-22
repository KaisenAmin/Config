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
* **Check if a section exists:** This function checks if a specific section exists in the configuration file. This can be useful to avoid errors when trying to get or set keys in non-existent sections.
* **Count keys in a section:** This function returns the number of keys in a specific section. This can be useful for understanding how many settings are in a specific section.
* **Count sections in the configuration file:** This function returns the number of sections in the configuration file. It is useful to know the number of different sections in the configuration file.
* **Clear all sections from the configuration file:** This function deletes all sections and their corresponding keys from the configuration file. It is useful when you want to start with a clean configuration file.

* **Create a backup of the configuration file:** This function creates a backup of the current configuration file. It is useful when you want to save the current state of the configuration file before making some changes, and you want to be able to restore it if something goes wrong.

* **Load a backup of the configuration file:** This function loads a backup of the configuration file and replaces the current configuration with the backup one. It is useful when some changes in the configuration lead to errors and you want to restore the previous valid configuration.

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

2. You can use the "isEmpty" function like this:

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

3. "sectionExists" function to check whether a certain section exists in the configuration file:

    ```c
    #include <stdio.h>
    #include "Config.h"

    int main(int argc, char **argv)
    {
        Config config;

        initializeConfig(&config); // just belong this config 
        config.setConfigFileName("config.ini");

        if (config.sectionExists("Section1"))
            config.deleteSection("Section1");
        else 
            config.addSection("Section1");
        
        getchar();
        return 0;
    }
    ```

4.  below demonstrates how to use the "keyCount" function to count the keys in a given section:

    ```c
    #include <stdio.h>
    #include "Config.h"

    int main(int argc, char **argv)
    {
        Config config;

        initializeConfig(&config); // just belong this config 

        config.setConfigFileName("config.ini");

        // Add some sections and keys for demonstration
        config.addSection("Language");
        config.set("Language", "lang", "C++");
        config.set("Language", "jobs", "programmer");

        // Count keys in 'Language' section
        uint32_t keyCount = config.keyCount("Language");
        printf("There are %d keys in 'Language' section\n", keyCount);

        return 0;
    }
    ```

5.  example below shows how to use the "sectionCount" function to count the sections in the configuration file:

    ```c
    #include <stdio.h>

    #include "Config.h"


    int main(int argc, char **argv)
    {
        Config config;

        initializeConfig(&config); // just belong this config 

        config.setConfigFileName("config.ini");

        // Add some sections for demonstration
        config.addSection("Language");
        config.addSection("Jobs");

        // Count sections in the configuration file
        uint32_t sectionCount = config.sectionCount();
        printf("There are %d sections in the configuration file\n", sectionCount);

        return 0;
    }
    ```

6. example below shows how to use the "clearConfig" function to delete all sections from the configuration file:

    ```c
    #include <stdio.h>

    #include "Config.h"


    int main(int argc, char **argv)
    {
        Config config;

        initializeConfig(&config); // just belong this config 

        config.setConfigFileName("config.ini");

        // Add some sections for demonstration
        config.addSection("Language");
        config.addSection("Jobs");

        // Clear the configuration file
        config.clearConfig();

        getchar();
        return 0;
    }
    ```

7. create backup from config file with "backupConfig":

    ```c
    #include <stdio.h>
    #include "Config.h"

    int main(int argc, char **argv)
    {
        Config config;

        initializeConfig(&config); // just belong this config 

        config.setConfigFileName("config.ini");

        // Add some sections and keys for demonstration
        config.addSection("Language");
        config.set("Language", "lang", "C++");
        config.set("Language", "jobs", "programmer");

        // clear back of config.ini -> config.ini.bak
        config.backupConfig();

        // clear config.ini file
        config.clearConfig();

        getchar();
        return 0;
    }
    ```

8. load from backupfile with "loadBackup":

    ```c 
        Config config;

        initializeConfig(&config); // just belong this config 

        config.setConfigFileName("config.ini");

        // Add some sections and keys for demonstration
        config.addSection("Language");
        config.set("Language", "lang", "C++");
        config.set("Language", "jobs", "programmer");

        // clear back of config.ini -> config.ini.bak
        config.backupConfig();

        // clear config.ini file
        config.clearConfig();
        config.loadBackup();
    ```