#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>
#include "sources\win32\libraries\base_types.h"
#include "sources\win32\libraries\basic_defines.h"

#include "string_list.h"

void PushStringNode(string_node **List, char *String)
{
    string_node *NewNode = (string_node *)malloc(sizeof(string_node));
    ZeroMemory(NewNode->String, MAX_STRING_LENGTH);
    StringCchCat(NewNode->String, ArrayCount(NewNode->String), String);
    NewNode->NextString = *List;
    *List = NewNode;
}

void FreeStringList(string_node *RootNode)
{
    string_node *CurrentNode = RootNode;
    string_node *ChildNode;

    while (CurrentNode)
    {
        ChildNode = CurrentNode->NextString;
        free(CurrentNode);
        CurrentNode = ChildNode;
    }
}

void FlattenStringList(string_node *ListNode, char *Output, u32 OutputSize)
{
    while (ListNode)
    {
        StringCchCatA(Output, OutputSize, ListNode->String);
        StringCchCatA(Output, OutputSize, " ");
        ListNode = ListNode->NextString;
    }
}