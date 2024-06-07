#pragma once

#define MAX_STRING_LENGTH 512

struct string_node
{
    char String[MAX_STRING_LENGTH];
    string_node *NextString;
};

string_node *PushStringNode(string_node **List);
void FreeStringList(string_node *RootNode);
void FlattenStringList(string_node *ListNode, char *Output, u32 OutputSize);