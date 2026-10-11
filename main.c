#include <stdio.h>
#include "Config.h"

int main(void)
{
    Config config;
    initializeConfig(&config);

    config.setConfigFileName("config.ini");
    config.clearConfig();

    config.addSection("Language");
    config.addSection("Jobs");
    config.set("Language", "lang", "C");
    config.set("Language", "web", "Php");
    config.set("Jobs", "job", "programmer");
    config.update("Language", "lang", "C++");
    config.deleteKey("Language", "web");
    config.merge("merge.ini");

    config.showConfig();
    printf("\nlang = %s\n", config.get("Language", "lang"));
    printf("Sections: %u, keys: %u\n", (unsigned)config.sectionCount(), (unsigned)config.totalKeyCount());

    destroyConfig(&config);
    return 0;
}
