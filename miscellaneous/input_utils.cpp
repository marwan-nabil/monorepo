inline void
ProcessWindowsMessage(MSG Message, user_input *UserInput)
{
    u32 VirtualKeyCode = (u32)Message.wParam;
    b8 KeyWasDown = (Message.lParam & (1 << 30)) != 0;
    b8 KeyIsDown = (Message.lParam & (1ll << 31)) == 0;
    b8 AltKeyIsDown = (Message.lParam & (1 << 29)) != 0;

    if (KeyIsDown != KeyWasDown)
    {
        if (VirtualKeyCode == 'W')
        {
            UserInput->Up = KeyIsDown;
        }
        else if (VirtualKeyCode == 'A')
        {
            UserInput->Left = KeyIsDown;
        }
        else if (VirtualKeyCode == 'S')
        {
            UserInput->Down = KeyIsDown;
        }
        else if (VirtualKeyCode == 'D')
        {
            UserInput->Right = KeyIsDown;
        }
    }

    if ((VirtualKeyCode == VK_F4) && KeyIsDown && AltKeyIsDown)
    {
        UserInput->ExitSignal = true;
    }
}