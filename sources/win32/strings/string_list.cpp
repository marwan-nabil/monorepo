#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>
#include "sources\win32\base_types.h"
#include "sources\win32\basic_defines.h"

#include "string_list.h"

string_node *PushStringNode(string_node **List)
{
    string_node *NewNode = (string_node *)malloc(sizeof(string_node));
    ZeroMemory(NewNode->String, MAX_STRING_LENGTH);
    if (List)
    {
        NewNode->NextString = *List;
        *List = NewNode;
    }
    return NewNode;
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