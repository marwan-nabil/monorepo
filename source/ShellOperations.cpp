
#if 0
inline void
DeleteDirectoryOperation()
{
    AbsoluteFilePathBuffer[StringLength(AbsoluteFilePathBuffer)] = '\0';
    AbsoluteFilePathBuffer[StringLength(AbsoluteFilePathBuffer) + 1] = '\0';

    SHFILEOPSTRUCTA ShellFileOperation;
    ShellFileOperation.hwnd = 0;
    ShellFileOperation.wFunc = FO_DELETE;
    ShellFileOperation.pFrom = AbsoluteFilePathBuffer;
    ShellFileOperation.pTo = 0;
    ShellFileOperation.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT | FOF_ALLOWUNDO;
    ShellFileOperation.fAnyOperationsAborted = 0;
    ShellFileOperation.hNameMappings = 0;
    ShellFileOperation.lpszProgressTitle = 0;
    SHFileOperationA(&ShellFileOperation);
    // TODO: wait until shell operation finishes
}
#endif