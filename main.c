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

    printf("%s\n", config.get("Language", "jobs"));
    printf("%s\n", config.get("Jobs", "jobss"));
    // the following two will print nothing as the section "Amin" has been deleted
    printf("%s\n", config.get("Amin", "Ip"));
    printf("%s\n", config.get("Amin", "Best"));

    config.showConfig();
    
    getchar();
    return 0;
}
