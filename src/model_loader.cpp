#include "model_loader.h"
#include "utils.h"

#define MODEL_MAX_BUFFER 30000
#define WAVEFRONT_VERTEX (char*)"v"
#define WAVEFRONT_TEXTURE (char*)"vt"
#define WAVEFRONT_NORMAL (char*)"vn"
#define WAVEFRONT_FACE (char*)"f"
#define WAVEFRONT_TYPE_BUFFER_SIZE 2
#define WAVEFRONT_DATA_BUFFER_SIZE 128
bool
LoadModel(char* filename, Model* out_model)
{
    bool Result = false;

    if(!out_model)
    {
        return Result;
    }

    // Clear data
    out_model = { 0 };
    int vertexIndex = 0;
    int textureIndex = 0;
    int normalIndex = 0;
    int faceIndex = 0;

    // Load in file
    Win32ReadFileResult FileContents = win32ReadFile(filename);
    if(!FileContents.contents || FileContents.filesize < 1)
    {
        return Result;
    }

    int contentLength = FileContents.filesize / sizeof(char);
    LineReadBuffer lrb;
    char type[WAVEFRONT_TYPE_BUFFER_SIZE];
    char data[WAVEFRONT_DATA_BUFFER_SIZE];
    int currentPosition = 0;
    while(ReadNextLine((char*)FileContents.contents, contentLength, &lrb))
    {
        if(!ReadNextElement(lrb.line, lrb.lineSize, type, &currentPosition))
        {
            Result = false;
            break;
        }

        if(stringEqual(type, WAVEFRONT_VERTEX))
        {
            float x, y, z, w = 0.f;
            if(ReadNextElement(lrb.line, lrb.lineSize, data, &currentPosition))
            {
                stringToFloat(data, WAVEFRONT_DATA_BUFFER_SIZE, &x);
            }
            if(ReadNextElement(lrb.line, lrb.lineSize, data, &currentPosition))
            {
                stringToFloat(data, WAVEFRONT_DATA_BUFFER_SIZE, &y);
            }
            if(ReadNextElement(lrb.line, lrb.lineSize, data, &currentPosition))
            {
                stringToFloat(data, WAVEFRONT_DATA_BUFFER_SIZE, &z);
            }
            if(ReadNextElement(lrb.line, lrb.lineSize, data, &currentPosition))
            {
                stringToFloat(data, WAVEFRONT_DATA_BUFFER_SIZE, &w);
            }
            else
            {
                w = 1.0f;
            }

            out_model[vertexIndex++] = { x, y, z, w };
        }
        else if(stringEqual(type, WAVEFRONT_TEXTURE))
        {
            float x, y;
            if(ReadNextElement(lrb.line, lrb.lineSize, data, &currentPosition))
            {
                stringToFloat(data, WAVEFRONT_DATA_BUFFER_SIZE, &x);
            }
            if(ReadNextElement(lrb.line, lrb.lineSize, data, &currentPosition))
            {
                stringToFloat(data, WAVEFRONT_DATA_BUFFER_SIZE, &y);
            }

            out_model[textureIndex++] = { x, y };
        }
        else if(stringEqual(type, WAVEFRONT_NORMAL))
        {
            float x, y, z = 0.f;
            if(ReadNextElement(lrb.line, lrb.lineSize, data, &currentPosition))
            {
                stringToFloat(data, WAVEFRONT_DATA_BUFFER_SIZE, &x);
            }
            if(ReadNextElement(lrb.line, lrb.lineSize, data, &currentPosition))
            {
                stringToFloat(data, WAVEFRONT_DATA_BUFFER_SIZE, &y);
            }

            out_model[normalIndex++] = { x, y, z };
        }
        else if(stringEqual(type, WAVEFRONT_FACE))
        {
            while(ReadNextElement(lrb.line, lrb.lineSize, data, &currentPosition))
            {
                char fb[WAVEFRONT_DATA_BUFFER_SIZE];
                char c;
               
                
                int index = 0;
                int dataLength = stringLength(data);
                while(index < dataLength && (c = data[index]))
                {
                    if(c == '/')
                    {
                        fb[index++] = '\0';
                        
                    }
                }
            }
        }
    }

    return(Result);
}

bool ReadNextLine(char* buffer, int buffer_size, LineReadBuffer* line_buffer)
{
    bool Result = false;

    if(!buffer || !line_buffer)
    {
        return Result;
    }

    if(line_buffer->currentPosition >= buffer_size)
    {
        return Result;
    }

    line_buffer->line = 0;
    line_buffer->lineSize = 0;

    char c;
    for(int i = line_buffer->currentPosition; i < buffer_size; ++i)
    {
        c = buffer[i];
        if(c == '\n')
        {
            line_buffer->line[line_buffer->currentPosition++] = '\0';
            line_buffer->lineSize = i;
            Result = true;
            break;
        }

        line_buffer->line[line_buffer->currentPosition++] = c;
    }

    return Result;
}

bool ReadNextElement(char* buffer, char* element, int ebuffer_size, int* currentPosition)
{
    int Result = false;

    if(!buffer || !element)
    {
        return Result;
    }

    int blength = stringLength(buffer);
    char c;
    int ei = 0;
    while(*currentPosition < blength && (c = buffer[*currentPosition]) && ei < ebuffer_size)
    {
        if(c == ' ')
        {
            element[ei] = '\0';
            ++(*currentPosition);
            break;
        }

        element[ei++] = c;
    }

    return(Result);
} 
