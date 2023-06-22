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
