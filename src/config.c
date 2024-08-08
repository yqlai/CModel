# include "headers/utils.h"

enum ConfigType
{
    PACKET_NAME,
    FIELD_SETTING,
    COMMAND
};

struct Field
{
    char *FieldName;
    int *available;
};

struct PacketConfig
{
    struct Field *fields;
};

int iscommand(char *line)
{
    if (line[0] == '\n') return 1;
    return 0;
}

enum ConfigType identify_type(char *line)
{
    if (iscommand(line)) return COMMAND;
    if (line[0] == '[') return PACKET_NAME;
    return FIELD_SETTING;
}

int is_valid_config_value(char *line)
{
    if (line[0] != '{') return 0;
    if (line[strlen(line)-1] != '}' && (line[strlen(line)-1] == '\n' && line[strlen(line)-2] != '}')) return 0;
    return 1;
}

void get_available_array(int *available, char *line)
{
    printf("Line in get_available_array: %s\n", line);
    if (is_valid_config_value(line) == 0)
    {
        fprintf(stderr, "Invalid config value\n");
        exit(1);
    }

    line = line + 1;
    line[strlen(line)-1] = '\0';

    // if ':' is in line, then it is a range
    if (strchr(line, ':') != NULL)
    {
        char *token = strtok(line, ":");
        int start = (int)strtol(token, NULL, 10);
        token = strtok(NULL, ":");
        int end = (int)strtol(token, NULL, 10);

        available = (int*)malloc((end-start+1)*sizeof(int));
        for (int i = start; i <= end; i++)
        {
            *available = i;
            available++;
        }
    }
    else
    {
        char *token = strtok(line, " ,");
        int i = 0;
        while(token != NULL)
        {
            int *tmp = available + i;
            tmp = (int*)malloc(sizeof(int));
            *tmp = strtol(token, NULL, 10);
            i++;

            token = strtok(NULL, " ");
        }
    }
}

int main()
{
    const char *filename = "config.txt";
    const char *folder = "../data/";
    char filepath[100];
    sprintf(filepath, "%s%s", folder, filename);

    FILE *file = fopen(filepath, "r");
    if(file == NULL)
    {
        fprintf(stderr, "Error reading config file.\n");
        exit(1);
    }

    struct PacketConfig *packetconfigs;
    int packet_num = 0;
    int field_num = 0;

    char line[300];
    while(fgets(line, 300, file))
    {
        printf("Line: %s\n", line);
        enum ConfigType type = identify_type(line);
        if (type == COMMAND) continue;

        if(type == PACKET_NAME)
        {
            struct PacketConfig *tmp = packetconfigs + packet_num;
            tmp = (struct PacketConfig*)malloc(sizeof(struct PacketConfig));
            packet_num++;
            field_num = 0;
        }
        else if(type == FIELD_SETTING)
        {
            char *token = strtok(line, " ");

            struct Field *tmp = packetconfigs[packet_num-1].fields + field_num;
            tmp = (struct Field*)malloc(sizeof(struct Field));
            tmp->FieldName = strdup(token);
            
            token = strtok(NULL, "");
            get_available_array(tmp->available, token);
        }
    }
    
    return 0;
}