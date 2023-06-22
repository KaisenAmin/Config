#include <stdio.h>
#include "Config.h"

int main(int argc, char **argv)
{
    Config config;

    initializeConfig(&config); // just belong this config 

    config.setConfigFileName("config.ini");

    config.addSection("Section1");
    config.addSection("Section2");

    config.set("Section1", "key1", "value1");
    config.set("Section1", "key2", "value2");
    config.set("Section1", "key3", "value3");
    config.set("Section2", "key4", "value4");
    config.set("Section2", "key5", "value5");

    config.showConfig();

    config.renameKey("Section1", "key1", "key6");  // Rename key1 to key6 in Section1
    config.showConfig();

    getchar();
    return 0;
}
