#pragma once

#include <stdint.h>
class BDFFont
{
   public:
    BDFFont(uint8_t* data, uint32_t sz);

    struct Character
    {
        uint32_t index;
        uint32_t width;
        uint32_t height;
        int32_t x_offset;
        int32_t y_offset;
        uint8_t* data;
    };
    Character** characters = 0;

    uint32_t font_ascent;

   private:
    uint32_t major_version;
    uint32_t minor_version;
    uint32_t character_count;

    const char* STARTFONT = "STARTFONT ";
    const char* CHARS = "CHARS ";
    const char* ENDFONT = "ENDFONT";
    const char* ENCODING = "ENCODING ";
    const char* BBX = "BBX ";
    const char* BITMAP = "BITMAP";
    const char* ENDCHAR = "ENDCHAR";
    const char* FONT_ASCENT = "FONT_ASCENT ";
};
