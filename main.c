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
    config.loadBackup();

    getchar();
    return 0;
}
