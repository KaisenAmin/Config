#include "Config.h"

void initializeConfig(Config *configObject)
{
    configObject->showConfig = readFile;
    configObject->writeConfig = writeFile;
    configObject->setConfigFileName = setFileName;
    configObject->addSection = addSection;
    configObject->set = set;
    configObject->get = get;
    configObject->counterList = 0;
    configObject->keysCounter = 0;
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

    this = configObject;
}

char* trim(char* s)
{
    while (isspace((unsigned char) *s)) s++;  // Skip leading whitespace

    if (*s)  // Nonempty string?
    {
        char* back = s + strlen(s);
        while (isspace((unsigned char) *--back) );  // Skip trailing whitespace
        *(back+1) = '\0';  // Null terminate
    }

    return s;
}

char* substr(const char *src, int m, int n)
{
    int len = n - m;
 
    char *dest = (char*)malloc(sizeof(char) * (len + 1));
 
    for (int i = m; i < n  && (*(src + i) != '\0'); i++)
    {
        *dest = *(src + i);
        dest++;
    }

    *dest = '\0';
 
    return dest - len;
}

bool checkSectionList(const char *sectionName)
{
    bool flag = false;

    if (this->counterList > 0)
    {
        for (uint32_t index = 0; index < this->counterList; index++)
        {
            printf("%s\n", this->sectionList[index]);
            if (strcmp(this->sectionList[index], sectionName) == 0)
            {
                flag = true;
                break;
            }
        }

        if (!flag)
            strcpy(this->sectionList[this->counterList++], sectionName);
    }
    else
        strcpy(this->sectionList[this->counterList++], sectionName);

    return flag;
}

bool checkSectionInFile(const char *sectionName)
{
    int pos = -1;

    searchSectionInFile(sectionName, &pos, NULL);

    if (pos >= 0)
        return true;
    else 
        return false;

}

bool checkSet(const char* key, const char *value, const char* sectionName)
{
    FILE* readerFile = fopen(this->fileName, "r");

    if (readerFile != NULL)
    {
        char lineStr[SECTION_SIZE + SECTION_SIZE];
        char keyValue[SECTION_SIZE + SECTION_SIZE];

        sprintf(keyValue, "\t%s = \"%s\"\n", key, value);

        bool flag = false;
        uint32_t counter = 0;
        
        while(fgets(lineStr, SECTION_SIZE, readerFile) != NULL)
        {
            if (counter == 2 && flag)
                break;
            if (strstr(lineStr, sectionName) != NULL)
            {
                counter++;
                flag = true;
            }
            else if (strstr(lineStr, "[") != NULL && flag)
            {
                counter++;
            }
            else if (strcmp(keyValue, lineStr) == 0 && flag)
            {
                fclose(readerFile);
                return true;
            }
        }

        fclose(readerFile);
        return false;
    }
    else 
    {
        fprintf(stderr, "%s can not opened\n", this->fileName);
        return false;
    }
}
void searchSectionInFile(const char *sectionName, int *position, int *fileCursorPos)
{
    bool findSectionFlag = false;
    FILE *readerFile = fopen(this->fileName, "r");

    if (readerFile != NULL)
    {
        char lineStr[SECTION_SIZE];
        fseek(readerFile, 0, SEEK_SET);

        while (!feof(readerFile))
        {
            fgets(lineStr, SECTION_SIZE, readerFile);
            char *foundPosition = strstr(lineStr, sectionName);

            if (foundPosition != NULL)
            {
                strcpy(this->sectionList[this->counterList], sectionName);
                this->counterList++;
                *position = foundPosition - lineStr;

                if (fileCursorPos != NULL)
                {
                    *fileCursorPos = ftell(readerFile);
                }
                fclose(readerFile);
                return; 
            }
        }
    }

    fclose(readerFile);
}

void writeSetForSection(const char *key, const char *value, const char *sectionName, uint32_t sizeSeciton, int fileCursorPos)
{
    if (checkSet(key, value, sectionName))
    {
        fprintf(stdout, "%s set in %s before\n", key, sectionName);
        return;
    }

    FILE *readerFile = fopen(this->fileName, "r");
    FILE *writerFile = fopen("./newConfig.ini", "w");

    if (readerFile != NULL && writerFile != NULL)
    {
        char lineStr[SECTION_SIZE];
        char dirName[SECTION_SIZE];
        char *section = (char*) malloc(sizeof(char) * strlen(sectionName));

        fseek(readerFile, 0, SEEK_SET);
        bool flag = true;
        bool keysFound = false;

        if (section != NULL)
        {
            sprintf(section, "[%s]", sectionName);
        }
        else
        {
            fprintf(stderr, "Can not allocate Memory");
            exit(-1);
        }

        while (fgets(lineStr, SECTION_SIZE, readerFile) != NULL)
        { 
            if (strstr(lineStr, section) != NULL)
            {

                fprintf(writerFile, "%s", lineStr);
                
                if (strstr(value, "\"") != NULL)
                    fprintf(writerFile, "\t%s = %s\n", key, value);
                
                else
                    fprintf(writerFile, "\t%s = \"%s\"\n", key, value);
                flag = true;
            }
            else
            { 
               fprintf(writerFile, "%s", lineStr);
            }
        }

        free(section);

        fclose(readerFile);
        fclose(writerFile);

        getcwd(dirName, SECTION_SIZE);

        if (dirName != 0)
        {
            char newDirName[SECTION_SIZE + 100];
            sprintf(newDirName, "%s\\%s", dirName, this->fileName);

            printf("%s\n", newDirName);
            if (remove(newDirName) != 0)
                perror("Can not remove the file");

            if (rename(strcat(dirName, "\\newConfig.ini"), newDirName) != 0)
                perror("Can not rename newConfig.ini");
        }
        
    }
    else 
    {
        fprintf(stderr, "Can not open %s file\n", this->fileName);
        exit(-1);
    }
}


static void setFileName(const char *configFileName)
{
    strcpy(this->fileName, configFileName);
}

static void addSection(const char *sectionName)
{
    char *section = NULL;
    uint32_t sectionLength = strlen(sectionName);

    section = (char *)malloc(sizeof(char) * sectionLength + BRACET_NUMBER);

    if (section != NULL)
    {
        if (checkSectionInFile(sectionName))
        {
            fprintf(stderr, "%s section already exists in %s file\n", sectionName, this->fileName);
        }

        else if (checkSectionList(sectionName))
        {
            fprintf(stderr, "[%s] Section is repeated section and not write again\n", sectionName);
            return; // later we add this section name to config file
        }
        else
        {
            sprintf(section, "[%s]\n", sectionName);
            this->writeConfig(section, "a+");

            free(section);
            section = NULL;
        }
    }
    else
    {
        fprintf(stderr, "Can not allocate Memory for write section %s", sectionName);
        exit(-1);
    }
}

static void set(const char *sectionName, const char *key, const char *value)
{
    if (sectionName != NULL)
    {
        bool findSectionFlag = false;
        for (uint32_t index = 0; index < this->counterList; index++)
        {
            if (strcmp(this->sectionList[index], sectionName) == 0)
            {
                findSectionFlag = true;
                break;
            }
        }

        if (findSectionFlag)
        {
            if (key != NULL && value != NULL)
            {
                int pos = -1;
                int fileCursorPos = -1;

                searchSectionInFile(sectionName, &pos, &fileCursorPos);
                
                if (pos >= 0)
                {
                    uint32_t sizeSection = strlen(sectionName) + BRACET_NUMBER;
                    
                    writeSetForSection(key, value, sectionName, sizeSection, fileCursorPos);
                }
            }
        }
    }
    else
    {
        fprintf(stderr, "SectionName %s is NULL\n", sectionName);
        exit(-1);
    }
}

static void readFile()
{
    FILE *readerFile = NULL;
    readerFile = fopen(this->fileName, "r");

    if (readerFile != NULL)
    {
        char lineStr[SECTION_SIZE];
        fseek(readerFile, 0, SEEK_SET);

        while (fgets(lineStr, SECTION_SIZE, readerFile) != NULL)
        {
            fprintf(stdout, "%s", lineStr);
        }

        fclose(readerFile);
    }
    else
    {
        fprintf(stderr, "Can not open %s file for read\n", this->fileName);
        exit(-1);
    }
}

static void writeFile(const char *config, const char *fileMode)
{
    FILE *writerFile = NULL;
    writerFile = fopen(this->fileName, fileMode);

    if (writerFile != NULL)
    {
        uint32_t sizeConfig = strlen(config);

        fwrite(config, 1, sizeConfig, writerFile);
        fclose(writerFile);
    }
    else
    {
        fprintf(stderr, "Can not open or Create %s file\n", this->fileName);
        exit(-1);
    }
}

char* get(const char *sectionName, const char *key)
{
    FILE *readerFile = fopen(this->fileName, "r");

    if (readerFile != NULL)
    {
        char lineStr[SECTION_SIZE + SECTION_SIZE];
        char *section = (char *) malloc(sizeof(char) * strlen(sectionName) + 3);

        if (section != NULL)
        {
            sprintf(section, "[%s]", sectionName);
            bool flag = false;
            uint32_t counter = 0;

            while (fgets(lineStr, SECTION_SIZE + SECTION_SIZE, readerFile) != NULL)
            {
                if (counter == 2 && flag)
                    break;
                if (strstr(lineStr, section) != NULL)
                {
                    flag = true;
                    counter++;
                }
                else if (strstr(lineStr, "[") != NULL && flag)
                    counter++;
                else if(flag && strstr(lineStr, key) != NULL)
                {
                    char *findKey = strstr(lineStr, "=");
                    int pos = (findKey - lineStr) + 2;
                    char *subText = substr(lineStr, pos + 1, strlen(lineStr) - 2);

                    return subText;
                }
            }

            fclose(readerFile);
            free(section);
        }
        else 
        {
            fprintf(stderr, "Can not allocate memeory\n");
            fclose(readerFile);
            exit(-1);
        }

    }
    else 
    {
        fprintf(stderr, "Can not open %s file\n", this->fileName);
        exit(-1);
    }
}

static void deleteSection(const char *sectionName)
{
    FILE *readerFile = fopen(this->fileName, "r");
    FILE *writerFile = fopen("./tempConfig.ini", "w");
    
    if (readerFile != NULL && writerFile != NULL)
    {
        char lineStr[SECTION_SIZE];
        char section[SECTION_SIZE];

        sprintf(section, "[%s]", sectionName);

        bool insideSection = false;

        while (fgets(lineStr, SECTION_SIZE, readerFile) != NULL)
        {
            if (strstr(lineStr, section) != NULL)
            {
                insideSection = true;
                continue;
            }
            
            if (insideSection && strstr(lineStr, "[") != NULL)
            {
                insideSection = false;
            }

            if (!insideSection)
            {
                fprintf(writerFile, "%s", lineStr);
            }
        }

        fclose(readerFile);
        fclose(writerFile);

        if (remove(this->fileName) != 0)
        {
            perror("Error deleting file");
        }

        if (rename("./tempConfig.ini", this->fileName) != 0)
        {
            perror("Error renaming file");
        }
    }
    else 
    {
        fprintf(stderr, "Can not open %s file\n", this->fileName);
        exit(-1);
    }
}

static void deleteKey(const char *sectionName, const char *key) 
{
    if (sectionName == NULL || key == NULL) 
    {
        fprintf(stderr, "Section name or key is NULL\n");
        exit(-1);
    }

    FILE *readerFile = fopen(this->fileName, "r");
    FILE *tempFile = fopen("temp.ini", "w");

    if (readerFile == NULL || tempFile == NULL) 
    {
        fprintf(stderr, "Cannot open file\n");
        exit(-1);
    }

    char lineStr[SECTION_SIZE + SECTION_SIZE];
    char *section = (char *)malloc(sizeof(char) * strlen(sectionName) + 3);
    sprintf(section, "[%s]", sectionName);
    bool isInSection = false;

    while (fgets(lineStr, SECTION_SIZE + SECTION_SIZE, readerFile) != NULL) 
    {
        if (strstr(lineStr, section) != NULL)
        {
            isInSection = true;
        }
        else if (strstr(lineStr, "[") != NULL && isInSection)
        {
            isInSection = false;
        }

        if (!isInSection || (isInSection && strstr(lineStr, key) == NULL))
        {
            fprintf(tempFile, "%s", lineStr);
        }
    }

    free(section);
    fclose(readerFile);
    fclose(tempFile);

    // Replace the original file with the temp file
    remove(this->fileName);
    rename("temp.ini", this->fileName);
}

static void update(const char* sectionName, const char *key, const char *value) 
{
    FILE *readerFile = fopen(this->fileName, "r");
    FILE *tempFile = fopen("temp.ini", "w");

    if (readerFile != NULL && tempFile != NULL) 
    {
        char lineStr[2 * SECTION_SIZE];
        char *section = (char *) malloc(sizeof(char) * strlen(sectionName) + 3);
        sprintf(section, "[%s]", sectionName);
        bool inSection = false;

        while (fgets(lineStr, 2 * SECTION_SIZE, readerFile) != NULL) 
        {
            // Check if we're in the section we're interested in
            if (strstr(lineStr, section) != NULL) 
            {
                inSection = true;
            } 
            else if (strstr(lineStr, "[") != NULL) 
            {
                inSection = false;
            }

            // If we're in the section, check if the line contains the key
            if (inSection && strstr(lineStr, key) != NULL) 
            {
                // If it does, replace the value
                char* equalsSignPos = strchr(lineStr, '=');
                if (equalsSignPos != NULL) 
                {
                    // Write the key and new value to the line
                    sprintf(equalsSignPos + 1, " \"%s\"\n", value);
                }
            }

            // Write the line (possibly updated) to the temp file
            fputs(lineStr, tempFile);
        }

        free(section);
        fclose(readerFile);
        fclose(tempFile);

        // Replace the old file with the updated one
        remove(this->fileName);
        rename("temp.ini", this->fileName);
    } 
    else 
    {
        fprintf(stderr, "Could not open file for reading/updating\n");
        exit(-1);
    }
}

static bool keyExists(const char *sectionName, const char *key) 
{
    bool keyFound = false; 
    FILE *readerFile = fopen(this->fileName, "r");
    if (readerFile == NULL) 
    {
        fprintf(stderr, "Can not open %s file\n", this->fileName);
        return false;
    }

    char lineStr[SECTION_SIZE + SECTION_SIZE];
    char *section = (char *) malloc(sizeof(char) * (strlen(sectionName) + 3));

    if (section == NULL) 
    {
        fprintf(stderr, "Can not allocate memory\n");
        fclose(readerFile);
        return false;
    }

    sprintf(section, "[%s]", sectionName);
    bool flag = false;
    uint32_t counter = 0;

    while (fgets(lineStr, SECTION_SIZE + SECTION_SIZE, readerFile) != NULL) 
    {
        if (counter == 2 && flag)
            break;
        if (strstr(lineStr, section) != NULL) 
        {
            flag = true;
            counter++;
        }
        else if (strstr(lineStr, "[") != NULL && flag)
            counter++;
        else if(flag && strstr(lineStr, key) != NULL) 
        {
            keyFound = true;
            break;
        }
    }

    free(section);
    fclose(readerFile);
    return keyFound;
}

static bool isEmpty() 
{
    printf("in isEmpty");
    FILE *readerFile = fopen(this->fileName, "r");
    if (readerFile == NULL) 
    {
        fprintf(stderr, "Cannot open file\n");
        exit(-1);
    }

    // Move the file pointer to the end of the file
    fseek(readerFile, 0, SEEK_END);

    // Get the position of the file pointer which is also the size of the file
    long fileSize = ftell(readerFile);

    fclose(readerFile);

    // If the file size is 0, then the file is empty
    return fileSize == 0;
}

static void getKeysFromSection(const char *sectionName) 
{
    FILE *readerFile = fopen(this->fileName, "r");
    char lineStr[SECTION_SIZE];
    char *section = (char *) malloc(sizeof(char) * strlen(sectionName) + 3);

    if (readerFile == NULL || section == NULL) 
    {
        fprintf(stderr, "Can not open file or allocate memory\n");
        return;
    }

    sprintf(section, "[%s]", sectionName);
    bool isInSection = false;

    while (fgets(lineStr, SECTION_SIZE, readerFile) != NULL) 
    {
        // Remove trailing newline
        lineStr[strcspn(lineStr, "\n")] = 0;

        if (strstr(lineStr, section) != NULL)
            isInSection = true;
        else if (lineStr[0] == '[')
            isInSection = false;
        else if (isInSection && lineStr[0] != 0 && lineStr[0] != ';') 
        {
            // Found a key in the section. Print it out.
            char* key = strtok(lineStr, "=");
            if(key) 
            {
                printf("Key: %s\n", key);
            }
        }
    }

    fclose(readerFile);
    free(section);
}

static bool sectionExists(const char *sectionName) 
{
    FILE *readerFile = fopen(this->fileName, "r");

    if (readerFile != NULL) 
    {
        char lineStr[SECTION_SIZE + SECTION_SIZE];
        char *section = (char *) malloc(sizeof(char) * strlen(sectionName) + 3);

        if (section != NULL) 
        {
            sprintf(section, "[%s]", sectionName);
            
            while (fgets(lineStr, SECTION_SIZE + SECTION_SIZE, readerFile) != NULL) 
            {
                if (strstr(lineStr, section) != NULL) 
                {
                    printf("Section found \n");

                    fclose(readerFile);
                    free(section);

                    return true;
                }
            }

            fclose(readerFile);
            free(section);
        } 
        else 
        {
            fprintf(stderr, "Cannot allocate memory\n");
            fclose(readerFile);
            return false;
        }
    } 
    else 
    {
        fprintf(stderr, "Cannot open %s file\n", this->fileName);
        return false;
    }

    return false;
}

static uint32_t keyCount(const char *sectionName) 
{
    FILE *readerFile = fopen(this->fileName, "r");
    uint32_t keyCount = 0;
    bool inSection = false;
    
    if (readerFile != NULL) 
    {
        char lineStr[SECTION_SIZE + SECTION_SIZE];
        char *section = (char *) malloc(sizeof(char) * strlen(sectionName) + 3);

        if (section != NULL) 
        {
            sprintf(section, "[%s]", sectionName);

            while (fgets(lineStr, SECTION_SIZE + SECTION_SIZE, readerFile) != NULL) 
            {
                // If the current line is the section we're looking for
                if (strstr(lineStr, section) != NULL) 
                {
                    inSection = true;
                    continue;
                }
                
                // If the current line is a section but not the section we're looking for
                if (strstr(lineStr, "[") != NULL && inSection) 
                    break;
         

                // If we're inside the section and the line contains a '=' sign (indicating it's a key)
                if(inSection && strstr(lineStr, "=") != NULL) 
                    keyCount++;
            }

            fclose(readerFile);
            free(section);
            return keyCount;
        } 
        else 
        {
            fprintf(stderr, "Cannot allocate memory\n");
            fclose(readerFile);
            return 0;
        }
    } 
    else 
    {
        fprintf(stderr, "Cannot open %s file\n", this->fileName);
        return 0;
    }
}

static uint32_t sectionCount() 
{
    FILE *readerFile = fopen(this->fileName, "r");
    uint32_t sectionCount = 0;

    if (readerFile != NULL) 
    {
        char lineStr[SECTION_SIZE + SECTION_SIZE];

        while (fgets(lineStr, SECTION_SIZE + SECTION_SIZE, readerFile) != NULL) 
        {
            // If the current line is a section
            if (strstr(lineStr, "[") != NULL) 
                sectionCount++;
        }

        fclose(readerFile);
        return sectionCount;
    } 
    else 
    {
        fprintf(stderr, "Cannot open %s file\n", this->fileName);
        return 0;
    }
}

static void clearConfig() 
{
    FILE *file = fopen(this->fileName, "w");
    if (file != NULL) 
        fclose(file);
    else 
        fprintf(stderr, "Cannot open %s file\n", this->fileName);
}

static void backupConfig() 
{
    char backupFileName[256];
    sprintf(backupFileName, "%s.bak", this->fileName);

    FILE *originalFile = fopen(this->fileName, "r");
    FILE *backupFile = fopen(backupFileName, "w");

    if (originalFile == NULL) 
    {
        fprintf(stderr, "Cannot open original file %s\n", this->fileName);
        return;
    }

    if (backupFile == NULL) 
    {
        fprintf(stderr, "Cannot open backup file %s\n", backupFileName);
        fclose(originalFile);
        return;
    }

    char ch;
    while ((ch = fgetc(originalFile)) != EOF) 
        fputc(ch, backupFile);

    fclose(originalFile);
    fclose(backupFile);
}

static void loadBackup() 
{
    char backupFileName[256];
    sprintf(backupFileName, "%s.bak", this->fileName);

    FILE *originalFile = fopen(this->fileName, "w");
    FILE *backupFile = fopen(backupFileName, "r");

    if (originalFile == NULL) 
    {
        fprintf(stderr, "Cannot open original file %s\n", this->fileName);
        return;
    }

    if (backupFile == NULL) 
    {
        fprintf(stderr, "Cannot open backup file %s\n", backupFileName);
        fclose(originalFile);
        return;
    }

    char ch;
    while ((ch = fgetc(backupFile)) != EOF) 
        fputc(ch, originalFile);
    

    fclose(originalFile);
    fclose(backupFile);
}

static void clearSection(const char* sectionName) 
{
    if (!checkSectionList(sectionName)) 
    {
        fprintf(stderr, "Section %s does not exist\n", sectionName);
        return;
    }

    int position, fileCursorPos;
    searchSectionInFile(sectionName, &position, &fileCursorPos);

    FILE *file = fopen(this->fileName, "r");
    FILE *tempFile = fopen("temp.ini", "w");

    if (file == NULL || tempFile == NULL) 
    {
        fprintf(stderr, "Cannot open the file\n");
        return;
    }

    char lineStr[SECTION_SIZE + SECTION_SIZE];
    bool isInSection = false;

    while (fgets(lineStr, SECTION_SIZE + SECTION_SIZE, file) != NULL) 
    {
        if (strstr(lineStr, sectionName) != NULL) 
        {
            isInSection = true;
            fputs(lineStr, tempFile);
            continue;
        }

        if (strstr(lineStr, "[") != NULL) 
            isInSection = false;
        
        if (!isInSection) 
            fputs(lineStr, tempFile);
        
    }

    fclose(file);
    fclose(tempFile);
    remove(this->fileName);
    rename("temp.ini", this->fileName);
}

static void renameSection(const char *oldSectionName, const char *newSectionName) 
{
    FILE *readerFile = fopen(this->fileName, "r+");
    if (readerFile != NULL) 
    {
        char lineStr[SECTION_SIZE * 2];
        char *section = (char *) malloc(sizeof(char) * strlen(oldSectionName) + 3);
        if (section != NULL) 
        {
            sprintf(section, "[%s]", oldSectionName);
            while (fgets(lineStr, SECTION_SIZE * 2, readerFile) != NULL) 
            {
                if (strstr(lineStr, section) != NULL) 
                {
                    fseek(readerFile, -(long)strlen(lineStr), SEEK_CUR);
                    fprintf(readerFile, "%s]\n", newSectionName);
                    break;
                }
            }
            free(section);
        } 
        else 
        {
            fprintf(stderr, "Can not allocate memory\n");
        }
        fclose(readerFile);
    } 
    else 
    {
        fprintf(stderr, "Can not open %s file\n", this->fileName);
    }
}

static void renameKey(const char *sectionName, const char *oldKeyName, const char *newKeyName) 
{
    if (!this->sectionExists(sectionName)) 
    {
        fprintf(stderr, "Section %s does not exist\n", sectionName);
        return;
    }

    if (!this->keyExists(sectionName, oldKeyName)) 
    {
        fprintf(stderr, "Key %s does not exist in section %s\n", oldKeyName, sectionName);
        return;
    }

    if (this->keyExists(sectionName, newKeyName)) 
    {
        fprintf(stderr, "Key %s already exists in section %s\n", newKeyName, sectionName);
        return;
    }

    FILE *file = fopen(this->fileName, "r");
    FILE *tempFile = fopen("temp.ini", "w");

    if (file == NULL || tempFile == NULL) 
    {
        fprintf(stderr, "Cannot open the file\n");
        return;
    }

    char lineStr[SECTION_SIZE + SECTION_SIZE];
    bool isInSection = false;

    while (fgets(lineStr, SECTION_SIZE + SECTION_SIZE, file) != NULL) 
    {
        // Check if it's the start of the section
        if (strstr(lineStr, sectionName) != NULL) 
        {
            isInSection = true;
            fputs(lineStr, tempFile);
            continue;
        }

        // Check if it's the end of the section
        if (strstr(lineStr, "[") != NULL && isInSection) 
            isInSection = false;

        // If it's in the section, check if the line contains the old key
        if (isInSection && strstr(lineStr, oldKeyName) != NULL) 
        {
            fprintf(tempFile, "\t%s =%s\n", newKeyName, strchr(lineStr, '=') + 1);
        } 
        else 
        {
            fputs(lineStr, tempFile);
        }
    }

    fclose(file);
    fclose(tempFile);
    remove(this->fileName);
    rename("temp.ini", this->fileName);
}

static uint32_t totalKeyCount()
{
    uint32_t totalCount = 0;
    FILE *file = fopen(this->fileName, "r");
    
    if (file == NULL) 
    {
        fprintf(stderr, "Cannot open the file %s\n", this->fileName);
        return 0;
    }

    char lineStr[SECTION_SIZE + SECTION_SIZE];
    while (fgets(lineStr, SECTION_SIZE + SECTION_SIZE, file) != NULL) 
    {
        if (strchr(lineStr, '=') != NULL) 
        {
            totalCount++;
        }
    }

    fclose(file);
    return totalCount;
}

static void merge(const char* importFileName) 
{
    // Open the import file
    FILE* importFile = fopen(importFileName, "r");
    if (!importFile) 
    {
        fprintf(stderr, "Cannot open import file: %s\n", importFileName);
        return;
    }

    char line[SECTION_SIZE * 2];
    char currentSection[SECTION_SIZE] = "";

    // Read the import file line by line
    while (fgets(line, sizeof(line), importFile)) 
    {
        // If the line defines a section
        if (line[0] == '[') 
        {
            strncpy(currentSection, line + 1, strcspn(line + 1, "]"));
            currentSection[strcspn(line + 1, "]")] = '\0'; // Ensuring null termination

            // If the section doesn't exist, add it
            if (!sectionExists(currentSection)) 
            {
                addSection(currentSection);
            }
        }
        // If the line defines a key-value pair
        else if (strchr(line, '=') != NULL) 
        {
            char* key = strtok(line, "=");
            char* value = strtok(NULL, "=");
            // Remove leading and trailing whitespaces
            key = trim(key);
            value = trim(value);

            // If the key doesn't exist, add the key-value pair
            if (!keyExists(currentSection, key)) 
            {
                set(currentSection, key, value);
            }
        }
    }
    fclose(importFile);
}


