#include "../Config.h"

#include <string.h>

#define MAIN_FILE "tests/tmp_config.ini"
#define BACKUP_FILE "tests/tmp_config.ini.bak"
#define IMPORT_FILE "tests/tmp_import.ini"
#define RENAMED_FILE "tests/tmp_renamed.ini"
#define HANDWRITTEN_FILE "tests/tmp_handwritten.ini"

static int checks = 0;
static int failures = 0;

#define CHECK(condition) check((condition), #condition, __LINE__)

static void check(bool ok, const char *expression, int line)
{
    checks++;
    if (!ok)
    {
        failures++;
        printf("FAIL line %d: %s\n", line, expression);
    }
}

static bool equals(const char *actual, const char *expected)
{
    return actual && expected && strcmp(actual, expected) == 0;
}

static bool fileExists(const char *name)
{
    FILE *file = fopen(name, "r");
    if (file)
    {
        fclose(file);
    }
    return file != NULL;
}

static bool fileEquals(const char *name, const char *expected)
{
    static char buffer[8192];
    FILE *file = fopen(name, "r");
    if (!file)
    {
        return false;
    }
    size_t length = fread(buffer, 1, sizeof buffer - 1, file);
    fclose(file);
    buffer[length] = '\0';
    if (strcmp(buffer, expected) != 0)
    {
        printf("---- %s ----\n%s---- expected ----\n%s----\n", name, buffer, expected);
        return false;
    }
    return true;
}

static void writeFile(const char *name, const char *mode, const char *content)
{
    FILE *file = fopen(name, mode);
    if (file)
    {
        fputs(content, file);
        fclose(file);
    }
}

static void removeFiles(void)
{
    remove(MAIN_FILE);
    remove(BACKUP_FILE);
    remove(IMPORT_FILE);
    remove(RENAMED_FILE);
    remove(HANDWRITTEN_FILE);
}

static void openFresh(Config *config)
{
    removeFiles();
    initializeConfig(config);
    config->setConfigFileName(MAIN_FILE);
}

static void testReadmeScenario(void)
{
    Config config;
    openFresh(&config);
    CHECK(config.isEmpty() && !fileExists(MAIN_FILE));

    config.addSection("Language");
    config.addSection("Jobs");
    config.addSection("Amin");
    config.addSection("Language");
    config.set("Language", "jobs", "programmer");
    config.set("Language", "lang", "C");
    config.set("Language", "Web", "Php");
    config.set("Jobs", "jobss", "programmer");
    config.addSection("Love");
    config.set("Amin", "Best", "Scientis");
    config.set("Amin", "Ip", "192.168.90.90");
    config.set("Love", "person", "I dont know");
    config.deleteKey("Language", "Web");
    config.deleteSection("Amin");
    config.update("Language", "lang", "C++");

    CHECK(!config.keyExists("Language1", "lang") && config.keyExists("Language", "lang"));
    CHECK(equals(config.get("Language", "jobs"), "programmer"));
    CHECK(equals(config.get("Jobs", "jobss"), "programmer"));
    CHECK(equals(config.get("Language", "lang"), "C++"));
    CHECK(config.get("Amin", "Ip") == NULL && config.get("Language", "Web") == NULL);
    CHECK(config.sectionCount() == 3 && config.totalKeyCount() == 4 && !config.isEmpty());
    CHECK(fileEquals(MAIN_FILE,
                     "[Language]\n\tjobs = \"programmer\"\n\tlang = \"C++\"\n\n"
                     "[Jobs]\n\tjobss = \"programmer\"\n\n"
                     "[Love]\n\tperson = \"I dont know\"\n"));

    Config reopened;
    initializeConfig(&reopened);
    reopened.setConfigFileName(MAIN_FILE);
    CHECK(equals(reopened.get("Love", "person"), "I dont know") && reopened.totalKeyCount() == 4);
    destroyConfig(&reopened);
    destroyConfig(&config);
}

static void testSetUpdateExactKeys(void)
{
    Config config;
    openFresh(&config);
    config.set("Section1", "key1", "value1");
    config.set("Section1", "key10", "value10");
    config.set("Section1", "key1", "changed");
    CHECK(config.sectionExists("Section1") && config.keyCount("Section1") == 2);
    CHECK(equals(config.get("Section1", "key1"), "changed") && equals(config.get("Section1", "key10"), "value10"));

    config.update("Section1", "key", "nothing");
    CHECK(!config.keyExists("Section1", "key") && config.keyCount("Section1") == 2);

    config.deleteKey("Section1", "key1");
    CHECK(!config.keyExists("Section1", "key1") && equals(config.get("Section1", "key10"), "value10"));

    config.set("Section1", "quoted", "say \"hi\"");
    config.set("Section1", "spaced", "  padded  ");
    config.set("Section1", "empty", "");
    config.set("Section1", "bad=key", "x");
    config.set("Section1", "newline", "a\nb");
    config.set("Bad]Name", "key", "x");
    config.addSection(" spaced ");
    CHECK(config.keyCount("Section1") == 4 && config.sectionCount() == 1);

    Config reopened;
    initializeConfig(&reopened);
    reopened.setConfigFileName(MAIN_FILE);
    CHECK(equals(reopened.get("Section1", "quoted"), "say \"hi\""));
    CHECK(equals(reopened.get("Section1", "spaced"), "  padded  "));
    CHECK(equals(reopened.get("Section1", "empty"), ""));
    destroyConfig(&reopened);
    destroyConfig(&config);
}

static void testCountsAndClear(void)
{
    Config config;
    openFresh(&config);
    config.addSection("Language");
    config.set("Language", "lang", "C++");
    config.set("Language", "jobs", "programmer");
    config.addSection("Jobs");
    CHECK(config.keyCount("Language") == 2 && config.keyCount("Jobs") == 0 && config.keyCount("None") == 0);
    CHECK(config.sectionCount() == 2 && config.totalKeyCount() == 2);

    config.clearSection("Language");
    config.clearSection("Missing");
    CHECK(config.sectionExists("Language") && config.keyCount("Language") == 0 && config.totalKeyCount() == 0);

    config.clearConfig();
    CHECK(config.isEmpty() && config.sectionCount() == 0 && fileEquals(MAIN_FILE, ""));
    destroyConfig(&config);
}

static void testBackup(void)
{
    Config config;
    openFresh(&config);
    config.loadBackup();
    CHECK(!fileExists(MAIN_FILE));

    config.addSection("Language");
    config.set("Language", "lang", "C++");
    config.backupConfig();
    CHECK(fileExists(BACKUP_FILE));

    config.clearConfig();
    CHECK(config.isEmpty());
    config.loadBackup();
    CHECK(equals(config.get("Language", "lang"), "C++") && fileEquals(MAIN_FILE, "[Language]\n\tlang = \"C++\"\n"));

    remove(BACKUP_FILE);
    config.loadBackup();
    CHECK(equals(config.get("Language", "lang"), "C++"));
    destroyConfig(&config);
}

static void testRenames(void)
{
    Config config;
    openFresh(&config);
    config.set("Section1", "key1", "value1");
    config.set("Section1", "key2", "value2");
    config.set("Section2", "key4", "value4");

    config.renameSection("Section2", "Section3");
    CHECK(!config.sectionExists("Section2") && equals(config.get("Section3", "key4"), "value4"));
    config.renameSection("Section3", "Section1");
    config.renameSection("Missing", "Other");
    CHECK(config.sectionExists("Section3") && config.sectionCount() == 2);

    config.renameKey("Section1", "key1", "key6");
    CHECK(!config.keyExists("Section1", "key1") && equals(config.get("Section1", "key6"), "value1"));
    config.renameKey("Section1", "key6", "key2");
    config.renameKey("Section1", "missing", "key9");
    config.renameKey("Missing", "key2", "key9");
    CHECK(equals(config.get("Section1", "key6"), "value1") && equals(config.get("Section1", "key2"), "value2"));
    CHECK(fileEquals(MAIN_FILE, "[Section1]\n\tkey6 = \"value1\"\n\tkey2 = \"value2\"\n\n[Section3]\n\tkey4 = \"value4\"\n"));
    destroyConfig(&config);
}

static void testMerge(void)
{
    Config config;
    openFresh(&config);
    config.set("Section1", "key1", "original");
    writeFile(IMPORT_FILE, "w",
              "[Language]\n    lang=\"C++\"\n    ml=python\n\n"
              "[Section1]\n    key1=\"value1\"\n    key7 = \"value10\"\n");
    config.merge(IMPORT_FILE);
    CHECK(equals(config.get("Section1", "key1"), "original") && equals(config.get("Section1", "key7"), "value10"));
    CHECK(equals(config.get("Language", "lang"), "C++") && equals(config.get("Language", "ml"), "python"));
    CHECK(config.sectionCount() == 2 && config.totalKeyCount() == 4);

    config.merge("tests/does_not_exist.ini");
    CHECK(config.totalKeyCount() == 4);
    destroyConfig(&config);
}

static void testSaveConfigAs(void)
{
    Config config;
    openFresh(&config);
    config.saveConfigAs(RENAMED_FILE);
    CHECK(!fileExists(RENAMED_FILE) && equals(config.fileName, MAIN_FILE));

    config.set("Section", "key", "value");
    config.saveConfigAs(RENAMED_FILE);
    CHECK(!fileExists(MAIN_FILE) && fileExists(RENAMED_FILE) && equals(config.fileName, RENAMED_FILE));

    config.set("Section", "other", "value2");
    CHECK(!fileExists(MAIN_FILE));
    CHECK(fileEquals(RENAMED_FILE, "[Section]\n\tkey = \"value\"\n\tother = \"value2\"\n"));
    destroyConfig(&config);
}

static void testHandwrittenFile(void)
{
    writeFile(HANDWRITTEN_FILE, "wb",
              "\xEF\xBB\xBF; global comment\r\n"
              "[Server]\r\n"
              "  host = example.com  \r\n"
              "# port comment\r\n"
              "port=8080\r\n"
              "\r\n"
              "[Server]\r\n"
              "path = \"/api\"\r\n"
              "[ Client ]\r\n"
              "name = \"tool\"");

    Config config;
    initializeConfig(&config);
    config.setConfigFileName(HANDWRITTEN_FILE);
    CHECK(config.sectionCount() == 2 && config.keyCount("Server") == 3 && config.totalKeyCount() == 4);
    CHECK(equals(config.get("Server", "host"), "example.com") && equals(config.get("Server", "port"), "8080"));
    CHECK(equals(config.get("Server", "path"), "/api") && equals(config.get("Client", "name"), "tool"));

    config.update("Server", "port", "9090");
    CHECK(fileEquals(HANDWRITTEN_FILE,
                     "; global comment\n\n"
                     "[Server]\n\thost = \"example.com\"\n\t# port comment\n\tport = \"9090\"\n\tpath = \"/api\"\n\n"
                     "[Client]\n\tname = \"tool\"\n"));

    config.writeConfig("\n[Extra]\nkey = raw\n", "a");
    CHECK(equals(config.get("Extra", "key"), "raw") && config.sectionCount() == 3);
    destroyConfig(&config);
}

static void testUnsetFileName(void)
{
    Config config;
    initializeConfig(&config);
    config.set("Section", "key", "value");
    config.addSection("Section");
    CHECK(config.isEmpty() && config.get("Section", "key") == NULL && config.totalKeyCount() == 0);
    destroyConfig(&config);
    CHECK(config.data == NULL);
}

int main(void)
{
    testReadmeScenario();
    testSetUpdateExactKeys();
    testCountsAndClear();
    testBackup();
    testRenames();
    testMerge();
    testSaveConfigAs();
    testHandwrittenFile();
    testUnsetFileName();
    removeFiles();
    printf("%d checks, %d failures\n", checks, failures);
    return failures == 0 ? 0 : 1;
}
