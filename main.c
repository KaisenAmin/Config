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
