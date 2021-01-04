# TodokanaiTL subtitle patch

![screenshot](screenshot.png)

## Setting up the project
- obviously you need to configure this project with VS community (I use 2019)
- You need to install DirectX 9.0 June 2010 SDK from Microsoft.

Configure the compilation settings to output a single DLL titled `d3d9.dll` and place this in the root of the White Album 2 install folder.

Additionally, create a folder called `todokanai` in the root of the White Album 2 install folder and put `font.png` from the `wa2/` source directory and your subtitle file simply called `subtitles` inside of it.

## Subtitle Format

In the `White Album 2\todokanai` folder, there needs to be two files, the first being the font atlas `font.png` and the second being simply titled `subtitles` (no extension).

The trigger for any given sub track is the audio ID of the resource loaded for the background audio track. The debug menu will tell you whatever the most recently loaded audio track is which you can use to write your subs. This will be some 0-4 digit ID.

Please note that <b>ALL</b> subtitles must be ASCII. That means that MS elipse characters, MS single quotes, and MS double quotes must be replaced with `...`, `'`, and `"` respectively.

The carat character `^` is a linebreak, to make it look right, don't use any spaces surrounding it.

Any line that is not `{`, `}`, or inside of a sub block will be ignored and can be used for commenting.

At this time there is no support for multiple subtitles being on screen at the same time, this could be added if it turns out to be necessary.

Subtitles take on the following format:

```
{
9805 1002 307
500 5000 This text will be displayed from 500 ms from the^trigger until 5500ms from the start
5500 7000 the trigger is the 3 numbers at the top: audio trigger^end_file end_line
7500 8500 There's no hard limit on subtitle length etc.
}

{
9703 1003 410
500 5000 etc etc etc
5500 7000 more text more text more text^now on a new line.
}
```

Each line is `[start ms] [duration ms] text until newline`

## Adding new functionality

Although the majority of this patch is dedicated to adding subtitle support to White Album 2's engine, the core is a relatively abstract hooking solution which takes advantage of simple [trampolining](https://en.wikipedia.org/wiki/Trampoline_(computing)) and [DLL proxying](https://kevinalmansa.github.io/application%20security/DLL-Proxying/) (targeting d3d9.dll).

There's a function in `wa2/dllmain.cpp` called `setupHooks()` that installs all of the hooks for the project. It takes 3 arguments: the address of the function to patch, the callback for the hook (see below), and the size (in bytes) that will be overwritten and thus must be reallocated inside of the trampoline. Both of these values can be found pretty quickly using IDA and Cheat Engine in conjunction using `Find out what [...] this address`.

As it stands, all of the state is just being stored at the top of the file for simplicity's sake but if this project expands, that might need to be put into a state construct.

### Hook function:

If the hook function is to run _before_ the original, it should return _void_ and call the original as the last step with no return being called. If the function is to run _after_ the original, it should return the type of the original function, call the function _first_, store the return value, perform hooked actions, and then finally return the original return value.

The function _must_ take the arguments of the original function and _must_ call into the original. It should not do any returning because when the `ret` instruction is called, a second return in _your_ function will cause the pointer it returns to to be messed up. See both `audioFunctionHook`, and `endSceneHook` for examples of this done right. 

Also note that MS calling conventions must be adhered to for the function you're calling into to make sure the call stack is organized in the manner you expect. It should be pretty easy to tell with a debugger and most disassemblers will outright tell you the full signature (calling convention and all).

`installHook` returns a `char *` which should be cast to a typedef of a callback with the signature of the original function. For example: `typedef HRESULT(APIENTRY* endscene_t)(LPDIRECT3DDEVICE9 device);` is a typedef for a function pointer matching the signature of DirectX's `EndScene`

## Debug Information

There is some information about the game context as well as subtitle playback that can be displayed on screen if the project is compiled using the `_TTL_DEBUG` preprocessor definition.