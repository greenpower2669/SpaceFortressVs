// Android-only bridge between SDLActivity and the historical C++ entry point.
//
// The old source defines `int main()`; SDL's main macro turns that into a
// C++-mangled `SDL_main()` symbol. SDLActivity resolves an unmangled C symbol
// with the usual argc/argv signature, so expose a tiny stable bridge instead
// of rewriting the historical game source.

extern int SDL_main();

extern "C" int SpaceFortressMain(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return SDL_main();
}
