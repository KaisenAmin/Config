#include <stdio.h>

#include "KConfig.h"


int main(int argc, char **argv)
{
    KConfig config;

    initializeKConfig(&config); // just belong this config 

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

    printf("%s\n", config.get("Language", "jobs"));
    printf("%s\n", config.get("Jobs", "jobss"));
    printf("%s\n", config.get("Amin", "Ip"));
    printf("%s\n", config.get("Amin", "Best"));

    config.showConfig();
    
    getchar();
    return 0;
}