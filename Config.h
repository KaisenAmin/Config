#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define CONFIG_FILE_NAME_SIZE 256

typedef struct ConfigData ConfigData;

typedef struct Config
{
    char fileName[CONFIG_FILE_NAME_SIZE];
    ConfigData *data;

    void (*setConfigFileName)(const char *configFileName);
    void (*addSection)(const char *sectionName);
    void (*set)(const char *sectionName, const char *key, const char *value);
    void (*showConfig)(void);
    void (*writeConfig)(const char *config, const char *fileMode);
    char *(*get)(const char *sectionName, const char *key);
    void (*deleteSection)(const char *sectionName);
    void (*deleteKey)(const char *sectionName, const char *key);
    void (*update)(const char *sectionName, const char *key, const char *value);
    bool (*keyExists)(const char *sectionName, const char *key);
    bool (*isEmpty)(void);
    void (*getKeysFromSection)(const char *sectionName);
    bool (*sectionExists)(const char *sectionName);
    uint32_t (*keyCount)(const char *sectionName);
    uint32_t (*sectionCount)(void);
    void (*clearConfig)(void);
    void (*backupConfig)(void);
    void (*loadBackup)(void);
    void (*clearSection)(const char *sectionName);
    void (*renameSection)(const char *oldSectionName, const char *newSectionName);
    void (*renameKey)(const char *sectionName, const char *oldKeyName, const char *newKeyName);
    uint32_t (*totalKeyCount)(void);
    void (*merge)(const char *importFileName);
    void (*saveConfigAs)(const char *newFileName);
} Config;

void initializeConfig(Config *configObject);
void destroyConfig(Config *configObject);

#endif
