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
