#include "raylib.h"

struct TextNode;

typedef struct TextNode
{
    char *line;
    struct TextNode *next;
} TextNode;

typedef struct TextLog
{
    TextNode *head;
    TextNode *tail;
    size_t lineCount;
    size_t maxLines;
} TextLog;

TextLog *TextLogCreate(size_t maxLines)
{
    TextLog *log = (TextLog *)MemAlloc(sizeof(TextLog));
    if (!log)
    {
        return NULL;
    }
    log->head = NULL;
    log->tail = NULL;
    log->lineCount = 0;
    log->maxLines = maxLines;
    return log;
}

void TextLogClear(TextLog *log)
{
    TextNode *current = log->head;
    while (current)
    {
        TextNode *next = current->next;
        MemFree(current->line);
        MemFree(current);
        current = next;
    }
    log->head = NULL;
    log->tail = NULL;
    log->lineCount = 0;
}

void DrawLogs(const TextLog *log, Vector2 position, float lineHeight, Color color, float logHeight)
{
    if (!log->head)
        return;

    size_t count = log->lineCount;

    // start drawing from bottom of log area
    float yOffset = logHeight - lineHeight * count;

    for (TextNode *cur = log->head; cur != NULL; cur = cur->next)
    {
        DrawText(cur->line, position.x, position.y + yOffset, 12, color);
        yOffset += lineHeight;
    }
}

void TextLogAddLine(TextLog *log, const char *line)
{
    TextNode *newNode = (TextNode *)MemAlloc(sizeof(TextNode));
    if (!newNode)
    {
        return;
    }

    newNode->line = (char *)MemAlloc((TextLength(line) + 1) * sizeof(char));
    TextCopy(newNode->line, line);
    newNode->next = NULL;

    if (log->tail)
    {
        log->tail->next = newNode;
    }
    log->tail = newNode;
    if (!log->head)
    {
        log->head = newNode;
    }
    log->lineCount++;

    // Remove oldest line if exceeding maxLines
    if (log->lineCount > log->maxLines)
    {
        TextNode *oldHead = log->head;
        log->head = oldHead->next;
        MemFree(oldHead->line);
        MemFree(oldHead);
        log->lineCount--;
    }
}