/*
 * rpihw.c
 *
 * Copyright (c) 2014 Jeremy Garff <jer @ jers.net>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 *     1.  Redistributions of source code must retain the above copyright notice, this list of
 *         conditions and the following disclaimer.
 *     2.  Redistributions in binary form must reproduce the above copyright notice, this list
 *         of conditions and the following disclaimer in the documentation and/or other materials
 *         provided with the distribution.
 *     3.  Neither the name of the owner nor the names of its contributors may be used to endorse
 *         or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "rpihw.h"


#define LINE_WIDTH_MAX                           80
#define HW_VER_STRING                            "Revision"

#define PERIPH_BASE_RPI                          0x20000000
#define PERIPH_BASE_RPI2                         0x3f000000

#define VIDEOCORE_BASE_RPI                       0x40000000
#define VIDEOCORE_BASE_RPI2                      0xc0000000

#define RPI_MANUFACTURER_MASK                    (0xf << 16)
#define RPI_WARRANTY_MASK                        (0x3 << 24)

// Structure to store results of device-tree auto-detect
static rpi_hw_t board_info = {
    .hwver = 0x00,
    .type = 0x00,
    .periph_base = 0x00,
    .videocore_base = 0x00,
    .desc = "N/A"
};

static const rpi_hw_t rpi_hw_info[] = {
    //
    // Model B Rev 1.0
    //
    {
        .hwver  = 0x02,
        .type = RPI_HWVER_TYPE_PI1,
        .periph_base = PERIPH_BASE_RPI,
        .videocore_base = VIDEOCORE_BASE_RPI,
        .desc = "Model B",
    },
    {
        .hwver  = 0x03,
        .type = RPI_HWVER_TYPE_PI1,
        .periph_base = PERIPH_BASE_RPI,
        .videocore_base = VIDEOCORE_BASE_RPI,
        .desc = "Model B",
    },

    //
    // Model B Rev 2.0
    //
    {
        .hwver  = 0x04,
        .type = RPI_HWVER_TYPE_PI1,
        .periph_base = PERIPH_BASE_RPI,
        .videocore_base = VIDEOCORE_BASE_RPI,
        .desc = "Model B",
    },
    {
        .hwver  = 0x05,
        .type = RPI_HWVER_TYPE_PI1,
        .periph_base = PERIPH_BASE_RPI,
        .videocore_base = VIDEOCORE_BASE_RPI,
        .desc = "Model B",
    },
    {
        .hwver  = 0x06,
        .type = RPI_HWVER_TYPE_PI1,
        .periph_base = PERIPH_BASE_RPI,
        .videocore_base = VIDEOCORE_BASE_RPI,
        .desc = "Model B",
    },

    //
    // Model A
    //
    {
        .hwver  = 0x07,
        .type = RPI_HWVER_TYPE_PI1,
        .periph_base = PERIPH_BASE_RPI,
        .videocore_base = VIDEOCORE_BASE_RPI,
        .desc = "Model A",
    },
    {
        .hwver  = 0x08,
        .type = RPI_HWVER_TYPE_PI1,
        .periph_base = PERIPH_BASE_RPI,
        .videocore_base = VIDEOCORE_BASE_RPI,
        .desc = "Model A",
    },
    {
        .hwver  = 0x09,
        .type = RPI_HWVER_TYPE_PI1,
        .periph_base = PERIPH_BASE_RPI,
        .videocore_base = VIDEOCORE_BASE_RPI,
        .desc = "Model A",
    },

    //
    // Model B
    //
    {
        .hwver  = 0x0d,
        .type = RPI_HWVER_TYPE_PI1,
        .periph_base = PERIPH_BASE_RPI,
        .videocore_base = VIDEOCORE_BASE_RPI,
        .desc = "Model B",
    },
    {
        .hwver  = 0x0e,
        .type = RPI_HWVER_TYPE_PI1,
        .periph_base = PERIPH_BASE_RPI,
        .videocore_base = VIDEOCORE_BASE_RPI,
        .desc = "Model B",
    },
    {
        .hwver  = 0x0f,
        .type = RPI_HWVER_TYPE_PI1,
        .periph_base = PERIPH_BASE_RPI,
        .videocore_base = VIDEOCORE_BASE_RPI,
        .desc = "Model B",
    },

    //
    // Model B+
    //
    {
        .hwver  = 0x10,
        .type = RPI_HWVER_TYPE_PI1,
        .periph_base = PERIPH_BASE_RPI,
        .videocore_base = VIDEOCORE_BASE_RPI,
        .desc = "Model B+",
    },
    {
        .hwver  = 0x13,
        .type = RPI_HWVER_TYPE_PI1,
        .periph_base = PERIPH_BASE_RPI,
        .videocore_base = VIDEOCORE_BASE_RPI,
        .desc = "Model B+",
    },

    //
    // Compute Module
    //
    {
        .hwver  = 0x11,
        .type = RPI_HWVER_TYPE_PI1,
        .periph_base = PERIPH_BASE_RPI,
        .videocore_base = VIDEOCORE_BASE_RPI,
        .desc = "Compute Module",
    },
    {
        .hwver  = 0x14,
        .type = RPI_HWVER_TYPE_PI1,
        .periph_base = PERIPH_BASE_RPI,
        .videocore_base = VIDEOCORE_BASE_RPI,
        .desc = "Compute Module",
    },

    //
    // Pi Zero
    //
    {
        .hwver  = 0x900092,
        .type = RPI_HWVER_TYPE_PI1,
        .periph_base = PERIPH_BASE_RPI,
        .videocore_base = VIDEOCORE_BASE_RPI,
        .desc = "Pi Zero",
    },
    {
        .hwver  = 0x900093,
        .type = RPI_HWVER_TYPE_PI1,
        .periph_base = PERIPH_BASE_RPI,
        .videocore_base = VIDEOCORE_BASE_RPI,
        .desc = "Pi Zero",
    },

    //
    // Model A+
    //
    {
        .hwver  = 0x12,
        .type = RPI_HWVER_TYPE_PI1,
        .periph_base = PERIPH_BASE_RPI,
        .videocore_base = VIDEOCORE_BASE_RPI,
        .desc = "Model A+",
    },
    {
        .hwver  = 0x15,
        .type = RPI_HWVER_TYPE_PI1,
        .periph_base = PERIPH_BASE_RPI,
        .videocore_base = VIDEOCORE_BASE_RPI,
        .desc = "Model A+",
    },

    //
    // Pi 2 Model B
    //
    {
        .hwver  = 0xa01040,
        .type = RPI_HWVER_TYPE_PI2,
        .periph_base = PERIPH_BASE_RPI2,
        .videocore_base = VIDEOCORE_BASE_RPI2,
        .desc = "Pi 2",
    },
    {
        .hwver  = 0xa01041,
        .type = RPI_HWVER_TYPE_PI2,
        .periph_base = PERIPH_BASE_RPI2,
        .videocore_base = VIDEOCORE_BASE_RPI2,
        .desc = "Pi 2",
    },
    {
        .hwver  = 0xa21041,
        .type = RPI_HWVER_TYPE_PI2,
        .periph_base = PERIPH_BASE_RPI2,
        .videocore_base = VIDEOCORE_BASE_RPI2,
        .desc = "Pi 2",
    },

    //
    // Pi 3 Model B
    //
    {
        .hwver  = 0xa02082,
        .type = RPI_HWVER_TYPE_PI2,
        .periph_base = PERIPH_BASE_RPI2,
        .videocore_base = VIDEOCORE_BASE_RPI2,
        .desc = "Pi 3",
    },
    {
        .hwver  = 0xa22082,
        .type = RPI_HWVER_TYPE_PI2,
        .periph_base = PERIPH_BASE_RPI2,
        .videocore_base = VIDEOCORE_BASE_RPI2,
        .desc = "Pi 3",
    },

};


static unsigned get_dt_ranges(const char *filename, unsigned offset)
{
    unsigned address = ~0;
    FILE *fp = fopen(filename, "rb");
    if (fp)
    {
        unsigned char buf[4];
        fseek(fp, offset, SEEK_SET);
        if (fread(buf, 1, sizeof buf, fp) == sizeof buf)
        address = buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3] << 0;
        fclose(fp);
    }
    return address;
}


unsigned bcm_host_get_peripheral_address(void)
{
    return get_dt_ranges("/proc/device-tree/soc/ranges", 4);
}


unsigned bcm_host_get_peripheral_size(void)
{
    return get_dt_ranges("/proc/device-tree/soc/ranges", 8);
}


unsigned bcm_host_get_sdram_address(void)
{
    return get_dt_ranges("/proc/device-tree/axi/vc_mem/reg", 8);
}

const rpi_hw_t *rpi_dt_hw_detect(void){
    // Attempt to auto-detect addresses from device-tree
    unsigned videocore_base = bcm_host_get_sdram_address();
    unsigned peripheral_base = bcm_host_get_peripheral_address();
    
    if(videocore_base != ~0 && peripheral_base != ~0){
       
        board_info.periph_base = peripheral_base;
        board_info.videocore_base = videocore_base;
        return &board_info;
        
    }
    
    return NULL;
}

const rpi_hw_t *rpi_hw_detect(void)
{

    // Fall back to old method
    FILE *f = fopen("/proc/cpuinfo", "r");
    char line[LINE_WIDTH_MAX];
    const rpi_hw_t *result = NULL;

    if (!f)
    {
        return NULL;
    }

    while (fgets(line, LINE_WIDTH_MAX - 1, f))
    {
        if (strstr(line, HW_VER_STRING))
        {
            uint32_t rev;
            char *substr;
            unsigned i;

            substr = strstr(line, ": ");
            if (!substr)
            {
                continue;
            }

            errno = 0;
            rev = strtoul(&substr[1], NULL, 16);  // Base 16
            if (errno)
            {
                continue;
            }

            for (i = 0; i < (sizeof(rpi_hw_info) / sizeof(rpi_hw_info[0])); i++)
            {
                uint32_t hwver = rpi_hw_info[i].hwver;

                // Take out warranty and manufacturer bits
                hwver &= ~(RPI_WARRANTY_MASK | RPI_MANUFACTURER_MASK);
                rev &= ~(RPI_WARRANTY_MASK | RPI_MANUFACTURER_MASK);
                
                if (rev == hwver)
                {
                    result = &rpi_hw_info[i];

                    goto done;
                }
            }
        }
    }

done:
    fclose(f);

    return result;
}

