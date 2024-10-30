/*═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════*/

typedef struct
{
    uint8_t sw1_state;
    uint8_t sw2_state;
    char command_bot[256];
    char chat_id[256];
    int last_update_id;
}DATA_UPDATE_t;

typedef struct
{
    char *project_name;
    char *build_date;
    char *version_fwr;
    char *esp_idf_v;
    char *cpu_info;
    char *mem_size_info;
    char *flash_info;
    float temp;
}SYSTEM_INFO_t;

/*═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════*/
