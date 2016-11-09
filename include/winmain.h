#ifndef WINMAIN_H
#define WINMAIN_H
#ifdef HAVE_WINDOWS

int main(int, char**);

int CALLBACK WinMain(
    __in  HINSTANCE hInstance,
    __in  HINSTANCE hPrevInstance,
    __in  LPSTR lpCmdLine,
    __in  int nCmdShow)
{
    wchar_t **wargv;//wargv is an array of LPWSTR (wide-char string)
    char **argv;
    int argc;
    // we have to convert each arg to a char*
    wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    // here and then we allocate one more cell to make it a NULL-end array
    argv = (char **)calloc(argc + 1, sizeof(char *));
    for(int i = 0; i < argc; i++) {
        size_t origSize = wcslen(wargv[i]) + 1;
        size_t converted = 0;
        argv[i] = (char *)calloc(origSize + 1, sizeof(char));
        // we use the truncate strategy, it won't handle non latin chars correctly
        // but hey we don't even use args, right?
        wcstombs_s(&converted, argv[i], origSize, wargv[i], _TRUNCATE);
    }
    return main(argc, argv);
}
#endif
#endif
