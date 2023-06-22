#ifndef Config_H_
#define Config_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <direct.h>

#define BRACET_NUMBER 2
#define SECTION_SIZE 256

#pragma pack (push, 1)
typedef struct Config
{
    char fileName[256];
    char sectionList[SECTION_SIZE][SECTION_SIZE];
    char keys[SECTION_SIZE][SECTION_SIZE];
    uint32_t counterList;
    uint32_t keysCounter;

    void (*setConfigFileName)(const char *configFileName);
    void (*addSection)(const char *sectionName);
    void (*set)(const char* sectionName, const char *key, const char *value);
    void (*showConfig)();
    void (*writeConfig)(const char *config, const char *fileMode);
    char* (*get)(const char *sectionName, const char *key);
    void (*deleteSection)(const char *sectionName);
    void (*deleteKey)(const char *sectionName, const char *key);
    void (*update)(const char* sectionName, const char *key, const char *value);
    bool (*keyExists)(const char *sectionName, const char *key);
    bool (*isEmpty)();
    void (*getKeysFromSection)(const char *sectionName);
    bool (*sectionExists)(const char *sectionName);
    uint32_t (*keyCount)(const char *sectionName);
    uint32_t (*sectionCount)();
    void (*clearConfig)();
    void (*backupConfig)();
    void (*loadBackup)();
    void (*clearSection)(const char* sectionName);
    void (*renameSection)(const char *oldSectionName, const char *newSectionName);

} Config;
#pragma pack (pop)

static Config *this;

void initializeConfig(Config *configObject);
bool checkSectionList(const char* sectionName);
bool checkSectionInFile(const char* sectionName);
void searchSectionInFile(const char* sectionName, int *position, int *fileCursorPos);
void writeSetForSection(const char* key, const char *value, const char *sectionName, uint32_t pos, int fileCursorPos);
bool checkSet(const char* key, const char *value, const char* sectionName);
char* substr(const char *src, int m, int n);

static void renameSection(const char *oldSectionName, const char *newSectionName);
static void clearSection(const char* sectionName);
static void loadBackup();
static void backupConfig();
static void clearConfig();
static uint32_t sectionCount();
static uint32_t keyCount(const char *sectionName);
static bool sectionExists(const char *sectionName);
static void getKeysFromSection(const char *sectionName);
static bool isEmpty();
static bool keyExists(const char *sectionName, const char *key);
static void update(const char* sectionName, const char *key, const char *value);
static void deleteKey(const char *sectionName, const char *key);
static void deleteSection(const char *sectionName);
static void setFileName(const char *configFileName);
static void addSection(const char *sectionName);
static void set(const char* sectionName, const char *key, const char *value);
static char *get(const char *sectionName, const char *key);
static void readFile();
static void writeFile(const char *config, const char *fileMode);

#endif 