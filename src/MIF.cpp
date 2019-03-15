#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "MIF.h"

inline void
PrintUsage()
{
    printf("Usage MIF [OPTION...] [FILE]\nWhere FILE is required and OPTION can be any combination of the followiing:\n"
           "-width [NUMBER]   Where NUMBER (must be a multiple of 3) is the number of bits per pixel (default 12)\n");
}

inline bool
IsNumeric(char c)
{
    bool result = (c >= '0' && c <= '9');

    return result;
}

inline u32
CharsToU32(char *c)
{
    u32 result = 0;

    while (IsNumeric(*c))
    {
        result = (result * 10) + (*c - '0');
        ++c;
    }

    return result;
}

inline u32
TruncPixel(u8 red, u8 green, u8 blue, Config *config)
{
    u32 channelWidth = config->width / 3;

    u8 mask = 0xff << (8 - channelWidth);

    u8 r = (red   & mask) >> (8 - channelWidth);
    u8 g = (green & mask) >> (8 - channelWidth);
    u8 b = (blue  & mask) >> (8 - channelWidth);

    u32 packed = (((r) << (channelWidth * 2)) | 
                  ((g) << (channelWidth)) | 
                  ((b)));

    return packed;
}

inline void
WriteHeader(FILE *outputFile, Config *config)
{
    fprintf(outputFile, "WIDTH = %d;\nDEPTH = %d;\nADDRESS_RADIX=UNS;\nDATA_RADIX=UNS;\nCONTENT BEGIN\n", config->width, config->depth);
}

inline void
WriteBody(FILE *outputFile, Config *config, u8* image)
{
    for (u32 addr = 0; addr < config->depth; ++addr)
    {
        u8 red   = *image++;
        u8 green = *image++;
        u8 blue  = *image++;

        if (config->inputPixelFormat == 4)
        {
            ++image;
        }

        fprintf(outputFile, "%d : %d;\n", addr, TruncPixel(red, green, blue, config));
    }
}

inline void
WriteFooter(FILE *outputFile, Config *config)
{
    fprintf(outputFile, "\nEND;\n");
}

int
main(int argc, char **argv)
{
    if (argc <= 1)
    {
        PrintUsage();

        exit(0);
    }

    Config config = {};

    size_t currentIndex = 1;
    while (argc > 2)
    {
        // We have args to parse that aren't the file
        char *arg = argv[currentIndex];

        char *sw = arg + 1;

        switch (*sw)
        {
            // -width [NUMBER]
            case 'w':
            {
                --argc;

                char *widthStart = argv[++currentIndex];

                if (!IsNumeric(*widthStart))
                {
                    printf("Error parsing width value of \"%s\", terminating...\n", widthStart);
                    exit(1);
                }

                config.width = CharsToU32(widthStart);

                printf("Using a word width of %u bits\n", config.width);

                --argc;
                currentIndex += 1;
                
            } break;

            default:
            {
                printf("Unknown arg %s, ignoring...\n", arg);

                currentIndex += 1;
                --argc;
            } break;
        };
    }

    if (config.width == 0)
    {
        printf("No word width specified, assuming default value of 12...\n");
        config.width = 12;
    }

    char *filePath = argv[currentIndex];
    printf("Using file: %s\n", filePath);

    int imageWidth;
    int imageHeight;
    int bytePerChannel;

    u8* image = stbi_load(filePath, &imageWidth, &imageHeight, &bytePerChannel, 0);

    if (image == NULL)
    {
        printf("Error opening file %s, terminating...\n", filePath);
        exit(1);
    }

    config.depth = imageWidth * imageHeight;
    config.inputPixelFormat = bytePerChannel;

    if(!(config.inputPixelFormat == 3 || config.inputPixelFormat == 4))
    {
        printf("Error, unsupported pixel fomat (I need RGB or RGBA)\n");
        exit(1);
    }

    printf("Found image dimensions to be %d by %d pixels\n", imageWidth, imageHeight);
    printf("Using a data depth of %u words\n", config.depth);
    printf("Using an input pixel format of %d\n", config.inputPixelFormat);

    u32 oldLength = strlen(filePath);
    char *outputFile = (char*) malloc(oldLength + 5);

    strcpy(outputFile, filePath);
    strcpy(outputFile + oldLength, ".mif");

    FILE *outputFD = fopen(outputFile, "w");

    if (outputFD == NULL)
    {
        printf("Error creating output file %s, terminating...\n", outputFile);
        exit(1);
    }

    printf("Using output file %s\n", outputFile);

    WriteHeader(outputFD, &config);
    WriteBody(outputFD, &config, image);
    WriteFooter(outputFD, &config);

    stbi_image_free(image);
    free(outputFile);
    fclose(outputFD);

    return 0;
}

