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
