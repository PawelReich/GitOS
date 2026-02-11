//
// Created by Paweł Reich on 08/02/26.
//

#include "BDFFont.hpp"
extern "C"
{
#include <stdint.h>
#include <string.h>
#include "stdio.h"
}

BDFFont::BDFFont(uint8_t* data, uint32_t sz)
{
    char* data_ptr = (char*)data;
    if (strcmp(STARTFONT, data_ptr) != 0)
    {
        debug_printf("Incorrect BDF file.\n");
        return;
    }
    data_ptr += strlen(STARTFONT);
    data_ptr += scanf(data_ptr, "%d.%d", &major_version, &minor_version);

    if (major_version != 2 || minor_version != 1)
    {
        debug_printf("Unsupported version!\n");
    }

    Character* current_character = 0;
    size_t chars_idx = 0;

    while ((uint32_t)data_ptr - (uint32_t)data < sz)
    {
        if (strcmp(CHARS, data_ptr) == 0)
        {
            data_ptr += strlen(CHARS);
            data_ptr += scanf(data_ptr, "%d", &character_count);
            characters = new Character*[character_count];
            continue;
        }

        if (strcmp(ENCODING, data_ptr) == 0)
        {
            if (current_character != 0)
            {
                debug_printf("stale character! %d\n", chars_idx);
            }
            data_ptr += strlen(ENCODING);

            current_character = new Character;
            data_ptr += scanf(data_ptr, "%d", &current_character->index);
            // debug_printf("ENCODING: %d\n", current_character->index);
            continue;
        }

        if (strcmp(ENDCHAR, data_ptr) == 0)
        {
            data_ptr += strlen(ENDCHAR);
            if (chars_idx >= character_count)
            {
                debug_printf("toop many");
            }
            // debug_printf("ENDCHAR: chars_idx = %d, current_character->index = %d\n", chars_idx, current_character->index);
            characters[chars_idx++] = current_character;
            current_character = 0;
            continue;
        }

        if (strcmp(BBX, data_ptr) == 0)
        {
            data_ptr += strlen(BBX);
            data_ptr += scanf(data_ptr, "%d %d %d %d", &current_character->width, &current_character->height, &current_character->x_offset, &current_character->y_offset);
            uint32_t sz = (current_character->width / 8 + 1) * (current_character->height / 8 + 1);
            current_character->data = new uint8_t[sz];
            // debug_printf("BBX(%d): %d %d %d %d, calculated size = %d\n", current_character->index, current_character->height, current_character->width, current_character->x_offset, current_character->y_offset, sz);
            continue;
        }

        if (strcmp(BITMAP, data_ptr) == 0)
        {
            data_ptr += strlen(BITMAP) + 1;  // +1 for \n
            // debug_printf("BITMAP, height = %d\n", current_character->height);
            for (size_t line_idx = 0; line_idx < current_character->height; line_idx++)
            {
                uint32_t line = 0;
                data_ptr += scanf(data_ptr, "%x\n", &line);
                current_character->data[line_idx] = line;
            }
            continue;
        }

        if (strcmp(FONT_ASCENT, data_ptr) == 0)
        {
            data_ptr += strlen(FONT_ASCENT);
            data_ptr += scanf(data_ptr, "%d", &font_ascent);
        }

        if (strcmp(ENDFONT, data_ptr) == 0)
        {
            // debug_printf("ENDFONT\n");
            break;
        }
        data_ptr++;
    }
    debug_printf("Major: %d, Minor: %d\nCharacter Count: %d\n",
                 major_version, minor_version, character_count);
}
