#include "../inc/HAL.h"
#include "nvs.h"
#include "nvs_flash.h"

namespace HAL
{
    void FileSystem_Init(void)
    {
        esp_err_t err = nvs_flash_init();
        if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
            err == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
            FS_PRINTF("nvsInit failed");
            const esp_partition_t *partition = esp_partition_find_first(
                ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
            if (partition != NULL)
            {
                err = esp_partition_erase_range(partition, 0, partition->size);
                if (!err)
                {
                    err = nvs_flash_init();
                }
            }
        }
        
        FS_PRINTF("FileSystem_Init success\n");
    }
}