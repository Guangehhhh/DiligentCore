/*     Copyright 2019 Diligent Graphics LLC
 *  
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF ANY PROPRIETARY RIGHTS.
 *
 *  In no event and under no legal theory, whether in tort (including negligence), 
 *  contract, or otherwise, unless required by applicable law (such as deliberate 
 *  and grossly negligent acts) or agreed to in writing, shall any Contributor be
 *  liable for any damages, including any direct, indirect, special, incidental, 
 *  or consequential damages of any character arising as a result of this License or 
 *  out of the use or inability to use the software (including but not limited to damages 
 *  for loss of goodwill, work stoppage, computer failure or malfunction, or any and 
 *  all other commercial damages or losses), even if such Contributor has been advised 
 *  of the possibility of such damages.
 */

#include "Win32Debug.h"
#include "FormatString.h"
#include <csignal>
#include <iostream>
#include <Windows.h>

using namespace Diligent;

void WindowsDebug :: AssertionFailed( const Diligent::Char *Message, const char *Function, const char *File, int Line )
{
    auto AssertionFailedMessage = FormatAssertionFailedMessage(Message, Function, File, Line);
    OutputDebugMessage( DebugMessageSeverity::Error, AssertionFailedMessage.c_str(), nullptr, nullptr, 0);

    int nCode = MessageBoxA(NULL,
                            AssertionFailedMessage.c_str(),
                            "Runtime assertion failed",
                            MB_TASKMODAL|MB_ICONHAND|MB_ABORTRETRYIGNORE|MB_SETFOREGROUND);

    // Abort: abort the program
    if (nCode == IDABORT)
    {
        // raise abort signal
        raise(SIGABRT);

        // We usually won't get here, but it's possible that
        //  SIGABRT was ignored.  So exit the program anyway.
        exit(3);
    }

    // Retry: call the debugger
    if (nCode == IDRETRY)
    {
        DebugBreak();
        // return to user code
        return;
    }

    // Ignore: continue execution
    if (nCode == IDIGNORE)
        return;
};

void WindowsDebug::OutputDebugMessage( DebugMessageSeverity Severity, const Char *Message, const char *Function, const char *File, int Line)
{
    auto msg = FormatDebugMessage(Severity, Message, Function, File, Line);
    OutputDebugStringA(msg.c_str());

    if( Severity == DebugMessageSeverity::Error || Severity == DebugMessageSeverity::FatalError )
        std::cerr << msg;
    else
        std::cout << msg;
}

void DebugAssertionFailed(const Diligent::Char* Message, const char* Function, const char* File, int Line)
{
    WindowsDebug :: AssertionFailed( Message, Function, File, Line );
}

namespace Diligent
{
DebugMessageCallbackType DebugMessageCallback = WindowsDebug::OutputDebugMessage;
}
