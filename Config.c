#include "Config.h"

#include <ctype.h>
#include <string.h>

typedef struct
{
    char *key;
    char *value;
} Entry;

typedef struct
{
    char *name;
    Entry *entries;
    size_t count;
    size_t capacity;
} Section;

struct ConfigData
{
    Section preamble;
    Section *sections;
    size_t count;
    size_t capacity;
};

static Config *current = NULL;

static char *duplicate(const char *text)
{
    size_t size = strlen(text) + 1;
    char *copy = malloc(size);
    if (copy)
    {
        memcpy(copy, text, size);
    }
    return copy;
}

static char *trim(char *text)
{
    while (isspace((unsigned char)*text))
    {
        text++;
    }
    char *end = text + strlen(text);
    while (end > text && isspace((unsigned char)end[-1]))
    {
        end--;
    }
    *end = '\0';
    return text;
}

static char *unquote(char *text)
{
    size_t length = strlen(text);
    if (length >= 2 && text[0] == '"' && text[length - 1] == '"')
    {
        text[length - 1] = '\0';
        return text + 1;
    }
    return text;
}

static bool hasEdgeSpace(const char *text)
{
    size_t length = strlen(text);
    return length > 0 && (isspace((unsigned char)text[0]) || isspace((unsigned char)text[length - 1]));
}

static bool validSectionName(const char *name)
{
    return name && *name && !hasEdgeSpace(name) && !strpbrk(name, "]\r\n");
}

static bool validKey(const char *key)
{
    return key && *key && !hasEdgeSpace(key) && !strpbrk(key, "=\r\n") && !strchr(";#[", key[0]);
}

static bool validValue(const char *value)
{
    return value && !strpbrk(value, "\r\n");
}

static void freeSection(Section *section)
{
    for (size_t i = 0; i < section->count; i++)
    {
        free(section->entries[i].key);
        free(section->entries[i].value);
    }
    free(section->entries);
    free(section->name);
    *section = (Section){0};
}

static void freeData(ConfigData *data)
{
    if (!data)
    {
        return;
    }
    freeSection(&data->preamble);
    for (size_t i = 0; i < data->count; i++)
    {
        freeSection(&data->sections[i]);
    }
    free(data->sections);
    free(data);
}

static Section *findSection(ConfigData *data, const char *name)
{
    for (size_t i = 0; data && name && i < data->count; i++)
    {
        if (strcmp(data->sections[i].name, name) == 0)
        {
            return &data->sections[i];
        }
    }
    return NULL;
}

static Entry *findEntry(Section *section, const char *key)
{
    for (size_t i = 0; section && key && i < section->count; i++)
    {
        if (section->entries[i].key && strcmp(section->entries[i].key, key) == 0)
        {
            return &section->entries[i];
        }
    }
    return NULL;
}

static Section *addSectionTo(ConfigData *data, const char *name)
{
    if (data->count == data->capacity)
    {
        size_t capacity = data->capacity ? data->capacity * 2 : 8;
        Section *sections = realloc(data->sections, capacity * sizeof *sections);
        if (!sections)
        {
            return NULL;
        }
        data->sections = sections;
        data->capacity = capacity;
    }
    char *copy = duplicate(name);
    if (!copy)
    {
        return NULL;
    }
    Section *section = &data->sections[data->count++];
    *section = (Section){.name = copy};
    return section;
}

static bool appendEntry(Section *section, const char *key, const char *value)
{
    if (section->count == section->capacity)
    {
        size_t capacity = section->capacity ? section->capacity * 2 : 8;
        Entry *entries = realloc(section->entries, capacity * sizeof *entries);
        if (!entries)
        {
            return false;
        }
        section->entries = entries;
        section->capacity = capacity;
    }
    Entry entry = {key ? duplicate(key) : NULL, duplicate(value)};
    if ((key && !entry.key) || !entry.value)
    {
        free(entry.key);
        free(entry.value);
        return false;
    }
    section->entries[section->count++] = entry;
    return true;
}

static bool replaceText(char **target, const char *text)
{
    char *copy = duplicate(text);
    if (!copy)
    {
        return false;
    }
    free(*target);
    *target = copy;
    return true;
}

static bool setEntry(Section *section, const char *key, const char *value)
{
    Entry *entry = findEntry(section, key);
    return entry ? replaceText(&entry->value, value) : appendEntry(section, key, value);
}

static void removeEntry(Section *section, Entry *entry)
{
    size_t index = (size_t)(entry - section->entries);
    free(entry->key);
    free(entry->value);
    memmove(entry, entry + 1, (section->count - index - 1) * sizeof *entry);
    section->count--;
}

static void removeSection(ConfigData *data, Section *section)
{
    size_t index = (size_t)(section - data->sections);
    freeSection(section);
    memmove(section, section + 1, (data->count - index - 1) * sizeof *section);
    data->count--;
}

static uint32_t countKeys(const Section *section)
{
    uint32_t count = 0;
    for (size_t i = 0; i < section->count; i++)
    {
        count += section->entries[i].key != NULL;
    }
    return count;
}

static int readLine(FILE *file, char **line)
{
    size_t capacity = 128;
    size_t length = 0;
    char *buffer = malloc(capacity);
    if (!buffer)
    {
        return -1;
    }
    int c;
    while ((c = fgetc(file)) != EOF && c != '\n')
    {
        if (length + 1 == capacity)
        {
            char *bigger = realloc(buffer, capacity * 2);
            if (!bigger)
            {
                free(buffer);
                return -1;
            }
            buffer = bigger;
            capacity *= 2;
        }
        buffer[length++] = (char)c;
    }
    if (c == EOF && length == 0)
    {
        free(buffer);
        return ferror(file) ? -1 : 0;
    }
    buffer[length] = '\0';
    *line = buffer;
    return 1;
}

static bool parseLine(ConfigData *data, Section **section, char *text)
{
    char *equals = strchr(text, '=');
    if (*text == '\0')
    {
        return true;
    }
    if (*text == '[' && strchr(text, ']'))
    {
        *strchr(text, ']') = '\0';
        char *name = trim(text + 1);
        Section *existing = findSection(data, name);
        *section = existing ? existing : addSectionTo(data, name);
        return *section != NULL;
    }
    if (*text != ';' && *text != '#' && equals && equals != text)
    {
        *equals = '\0';
        return setEntry(*section, trim(text), unquote(trim(equals + 1)));
    }
    return appendEntry(*section, NULL, text);
}

static ConfigData *parseFile(FILE *file)
{
    ConfigData *data = calloc(1, sizeof *data);
    Section *section = data ? &data->preamble : NULL;
    bool firstLine = true;
    char *line = NULL;
    int status = data ? 1 : -1;
    while (status == 1 && (status = readLine(file, &line)) == 1)
    {
        char *text = line;
        if (firstLine && strncmp(text, "\xEF\xBB\xBF", 3) == 0)
        {
            text += 3;
        }
        firstLine = false;
        if (!parseLine(data, &section, trim(text)))
        {
            status = -1;
        }
        free(line);
    }
    if (status < 0)
    {
        freeData(data);
        return NULL;
    }
    return data;
}

static ConfigData *loadFile(const char *fileName, bool *exists)
{
    FILE *file = fopen(fileName, "r");
    if (exists)
    {
        *exists = file != NULL;
    }
    if (!file)
    {
        return calloc(1, sizeof(ConfigData));
    }
    ConfigData *data = parseFile(file);
    fclose(file);
    return data;
}

static void writeEntries(FILE *file, const Section *section, const char *indent)
{
    for (size_t i = 0; i < section->count; i++)
    {
        const Entry *entry = &section->entries[i];
        if (entry->key)
        {
            fprintf(file, "%s%s = \"%s\"\n", indent, entry->key, entry->value);
        }
        else
        {
            fprintf(file, "%s%s\n", indent, entry->value);
        }
    }
}

static void writeData(FILE *file, const ConfigData *data)
{
    writeEntries(file, &data->preamble, "");
    for (size_t i = 0; i < data->count; i++)
    {
        if (i > 0 || data->preamble.count > 0)
        {
            fputc('\n', file);
        }
        fprintf(file, "[%s]\n", data->sections[i].name);
        writeEntries(file, &data->sections[i], "\t");
    }
}

static bool ready(void)
{
    if (!current || !current->data || current->fileName[0] == '\0')
    {
        fprintf(stderr, "Config file name is not set\n");
        return false;
    }
    return true;
}

static ConfigData *currentData(void)
{
    return current ? current->data : NULL;
}

static bool save(void)
{
    FILE *file = fopen(current->fileName, "w");
    if (!file)
    {
        fprintf(stderr, "Can not open or create %s file\n", current->fileName);
        return false;
    }
    writeData(file, current->data);
    bool ok = !ferror(file);
    if (fclose(file) != 0 || !ok)
    {
        fprintf(stderr, "Can not write %s file\n", current->fileName);
        return false;
    }
    return true;
}

static bool reload(void)
{
    ConfigData *data = loadFile(current->fileName, NULL);
    if (!data)
    {
        fprintf(stderr, "Can not read %s file\n", current->fileName);
        return false;
    }
    freeData(current->data);
    current->data = data;
    return true;
}

static bool copyFile(const char *sourceName, const char *targetName)
{
    FILE *source = fopen(sourceName, "rb");
    if (!source)
    {
        fprintf(stderr, "Can not open %s file\n", sourceName);
        return false;
    }
    FILE *target = fopen(targetName, "wb");
    if (!target)
    {
        fprintf(stderr, "Can not open or create %s file\n", targetName);
        fclose(source);
        return false;
    }
    char buffer[4096];
    size_t bytes;
    bool ok = true;
    while (ok && (bytes = fread(buffer, 1, sizeof buffer, source)) > 0)
    {
        ok = fwrite(buffer, 1, bytes, target) == bytes;
    }
    ok = ok && !ferror(source);
    fclose(source);
    if (fclose(target) != 0)
    {
        ok = false;
    }
    if (!ok)
    {
        fprintf(stderr, "Can not copy %s to %s\n", sourceName, targetName);
    }
    return ok;
}

static bool backupFileName(char *buffer, size_t size)
{
    int length = snprintf(buffer, size, "%s.bak", current->fileName);
    return length > 0 && (size_t)length < size;
}

static bool checkEntryArguments(const char *sectionName, const char *key, const char *value)
{
    if (!validSectionName(sectionName))
    {
        fprintf(stderr, "Invalid section name\n");
        return false;
    }
    if (!validKey(key))
    {
        fprintf(stderr, "Invalid key name\n");
        return false;
    }
    if (!validValue(value))
    {
        fprintf(stderr, "Invalid value for key %s\n", key);
        return false;
    }
    return true;
}

static Section *existingSection(const char *sectionName)
{
    Section *section = findSection(currentData(), sectionName);
    if (!section)
    {
        fprintf(stderr, "Section %s does not exist\n", sectionName ? sectionName : "(null)");
    }
    return section;
}

static void setFileName(const char *configFileName)
{
    if (!current)
    {
        return;
    }
    if (!configFileName || *configFileName == '\0' || strlen(configFileName) >= sizeof current->fileName)
    {
        fprintf(stderr, "Invalid config file name\n");
        return;
    }
    strcpy(current->fileName, configFileName);
    reload();
}

static void addSection(const char *sectionName)
{
    if (!ready())
    {
        return;
    }
    if (!validSectionName(sectionName))
    {
        fprintf(stderr, "Invalid section name\n");
        return;
    }
    if (findSection(current->data, sectionName))
    {
        fprintf(stderr, "%s section already exists in %s file\n", sectionName, current->fileName);
        return;
    }
    if (addSectionTo(current->data, sectionName))
    {
        save();
    }
}

static void set(const char *sectionName, const char *key, const char *value)
{
    if (!ready() || !checkEntryArguments(sectionName, key, value))
    {
        return;
    }
    Section *section = findSection(current->data, sectionName);
    if (!section)
    {
        section = addSectionTo(current->data, sectionName);
    }
    if (section && setEntry(section, key, value))
    {
        save();
    }
}

static char *get(const char *sectionName, const char *key)
{
    Entry *entry = findEntry(findSection(currentData(), sectionName), key);
    return entry ? entry->value : NULL;
}

static void showConfig(void)
{
    if (!ready())
    {
        return;
    }
    FILE *file = fopen(current->fileName, "r");
    if (!file)
    {
        return;
    }
    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof buffer, file)) > 0)
    {
        fwrite(buffer, 1, bytes, stdout);
    }
    fclose(file);
}

static void writeConfig(const char *config, const char *fileMode)
{
    if (!ready() || !config || !fileMode)
    {
        return;
    }
    FILE *file = fopen(current->fileName, fileMode);
    if (!file)
    {
        fprintf(stderr, "Can not open or create %s file\n", current->fileName);
        return;
    }
    if (fputs(config, file) == EOF)
    {
        fprintf(stderr, "Can not write %s file\n", current->fileName);
    }
    fclose(file);
    reload();
}

static void deleteSection(const char *sectionName)
{
    Section *section = ready() ? findSection(current->data, sectionName) : NULL;
    if (section)
    {
        removeSection(current->data, section);
        save();
    }
}

static void deleteKey(const char *sectionName, const char *key)
{
    Section *section = ready() ? findSection(current->data, sectionName) : NULL;
    Entry *entry = findEntry(section, key);
    if (entry)
    {
        removeEntry(section, entry);
        save();
    }
}

static void update(const char *sectionName, const char *key, const char *value)
{
    if (!ready() || !checkEntryArguments(sectionName, key, value))
    {
        return;
    }
    Entry *entry = findEntry(findSection(current->data, sectionName), key);
    if (!entry)
    {
        fprintf(stderr, "Key %s does not exist in section %s\n", key, sectionName);
        return;
    }
    if (replaceText(&entry->value, value))
    {
        save();
    }
}

static bool keyExists(const char *sectionName, const char *key)
{
    return get(sectionName, key) != NULL;
}

static bool isEmpty(void)
{
    ConfigData *data = currentData();
    return !data || data->count == 0;
}

static void getKeysFromSection(const char *sectionName)
{
    Section *section = findSection(currentData(), sectionName);
    for (size_t i = 0; section && i < section->count; i++)
    {
        if (section->entries[i].key)
        {
            printf("Key: %s\n", section->entries[i].key);
        }
    }
}

static bool sectionExists(const char *sectionName)
{
    return findSection(currentData(), sectionName) != NULL;
}

static uint32_t keyCount(const char *sectionName)
{
    Section *section = findSection(currentData(), sectionName);
    return section ? countKeys(section) : 0;
}

static uint32_t sectionCount(void)
{
    ConfigData *data = currentData();
    return data ? (uint32_t)data->count : 0;
}

static void clearConfig(void)
{
    if (!ready())
    {
        return;
    }
    ConfigData *empty = calloc(1, sizeof *empty);
    if (!empty)
    {
        fprintf(stderr, "Can not allocate memory\n");
        return;
    }
    freeData(current->data);
    current->data = empty;
    save();
}

static void backupConfig(void)
{
    char backupName[CONFIG_FILE_NAME_SIZE + 8];
    if (ready() && backupFileName(backupName, sizeof backupName))
    {
        copyFile(current->fileName, backupName);
    }
}

static void loadBackup(void)
{
    char backupName[CONFIG_FILE_NAME_SIZE + 8];
    if (ready() && backupFileName(backupName, sizeof backupName) && copyFile(backupName, current->fileName))
    {
        reload();
    }
}

static void clearSection(const char *sectionName)
{
    if (!ready())
    {
        return;
    }
    Section *section = existingSection(sectionName);
    if (!section)
    {
        return;
    }
    char *name = section->name;
    section->name = NULL;
    freeSection(section);
    section->name = name;
    save();
}

static void renameSection(const char *oldSectionName, const char *newSectionName)
{
    if (!ready())
    {
        return;
    }
    Section *section = existingSection(oldSectionName);
    if (!section)
    {
        return;
    }
    if (!validSectionName(newSectionName))
    {
        fprintf(stderr, "Invalid section name\n");
        return;
    }
    if (findSection(current->data, newSectionName))
    {
        fprintf(stderr, "Section %s already exists\n", newSectionName);
        return;
    }
    if (replaceText(&section->name, newSectionName))
    {
        save();
    }
}

static void renameKey(const char *sectionName, const char *oldKeyName, const char *newKeyName)
{
    if (!ready())
    {
        return;
    }
    Section *section = existingSection(sectionName);
    if (!section)
    {
        return;
    }
    Entry *entry = findEntry(section, oldKeyName);
    if (!entry)
    {
        fprintf(stderr, "Key %s does not exist in section %s\n", oldKeyName ? oldKeyName : "(null)", sectionName);
        return;
    }
    if (!validKey(newKeyName))
    {
        fprintf(stderr, "Invalid key name\n");
        return;
    }
    if (findEntry(section, newKeyName))
    {
        fprintf(stderr, "Key %s already exists in section %s\n", newKeyName, sectionName);
        return;
    }
    if (replaceText(&entry->key, newKeyName))
    {
        save();
    }
}

static uint32_t totalKeyCount(void)
{
    ConfigData *data = currentData();
    if (!data)
    {
        return 0;
    }
    uint32_t total = countKeys(&data->preamble);
    for (size_t i = 0; i < data->count; i++)
    {
        total += countKeys(&data->sections[i]);
    }
    return total;
}

static void merge(const char *importFileName)
{
    if (!ready() || !importFileName)
    {
        return;
    }
    bool exists = false;
    ConfigData *imported = loadFile(importFileName, &exists);
    if (!exists || !imported)
    {
        fprintf(stderr, "Cannot open import file: %s\n", importFileName);
        freeData(imported);
        return;
    }
    bool ok = true;
    for (size_t i = 0; ok && i < imported->count; i++)
    {
        Section *source = &imported->sections[i];
        Section *target = findSection(current->data, source->name);
        if (!target)
        {
            target = addSectionTo(current->data, source->name);
        }
        for (size_t j = 0; target && j < source->count; j++)
        {
            Entry *entry = &source->entries[j];
            if (entry->key && !findEntry(target, entry->key))
            {
                ok = appendEntry(target, entry->key, entry->value);
            }
        }
        ok = ok && target != NULL;
    }
    freeData(imported);
    save();
}

static void saveConfigAs(const char *newFileName)
{
    if (!ready())
    {
        return;
    }
    if (!newFileName || *newFileName == '\0' || strlen(newFileName) >= sizeof current->fileName)
    {
        fprintf(stderr, "Invalid config file name\n");
        return;
    }
    if (strcmp(newFileName, current->fileName) == 0)
    {
        save();
        return;
    }
    if (!copyFile(current->fileName, newFileName))
    {
        return;
    }
    if (remove(current->fileName) != 0)
    {
        fprintf(stderr, "Unable to delete %s file\n", current->fileName);
    }
    strcpy(current->fileName, newFileName);
}

void initializeConfig(Config *configObject)
{
    if (!configObject)
    {
        return;
    }
    memset(configObject, 0, sizeof *configObject);
    configObject->data = calloc(1, sizeof *configObject->data);

    configObject->setConfigFileName = setFileName;
    configObject->addSection = addSection;
    configObject->set = set;
    configObject->showConfig = showConfig;
    configObject->writeConfig = writeConfig;
    configObject->get = get;
    configObject->deleteSection = deleteSection;
    configObject->deleteKey = deleteKey;
    configObject->update = update;
    configObject->keyExists = keyExists;
    configObject->isEmpty = isEmpty;
    configObject->getKeysFromSection = getKeysFromSection;
    configObject->sectionExists = sectionExists;
    configObject->keyCount = keyCount;
    configObject->sectionCount = sectionCount;
    configObject->clearConfig = clearConfig;
    configObject->backupConfig = backupConfig;
    configObject->loadBackup = loadBackup;
    configObject->clearSection = clearSection;
    configObject->renameSection = renameSection;
    configObject->renameKey = renameKey;
    configObject->totalKeyCount = totalKeyCount;
    configObject->merge = merge;
    configObject->saveConfigAs = saveConfigAs;

    current = configObject;
}

void destroyConfig(Config *configObject)
{
    if (!configObject)
    {
        return;
    }
    freeData(configObject->data);
    configObject->data = NULL;
    if (current == configObject)
    {
        current = NULL;
    }
}
