/**
* This file has been modified from its orginal sources.
*
* Copyright (c) 2012 Software in the Public Interest Inc (SPI)
* Copyright (c) 2012 David Pratt
* 
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
***
* Copyright (c) 2008-2012 Appcelerator Inc.
* 
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
**/

#include "posix_pipe.h"

namespace ti
{
    PosixPipe::PosixPipe(bool isReader) :
        NativePipe(isReader),
        readHandle(-1),
        writeHandle(-1)
    {
    }

    void PosixPipe::CreateHandles()
    {
        int fds[2];
        int rc = pipe(fds);
        if (rc == 0)
        {
            readHandle = fds[0];
            writeHandle = fds[1];
        }
        else
        {
            throw ValueException::FromFormat(
                "Error creating pipe: %s (%d)", strerror(errno), errno);
        }
    }

    void PosixPipe::Close()
    {
        // Close should only be called if the process (or whatever)
        // it is attached to closes. Thus we want to make sure all
        // the output is cleared from the pipe before closing them.
        // A widowed pipe will eventually stop reading data.
        NativePipe::Close();
    }

    int PosixPipe::RawRead(char *buffer, int size)
    {
        int n;
        do
        {
            n = read(readHandle, buffer, size);
        }
        while (n < 0 && errno == EINTR);

        if (n >= 0)
        {
            return n;
        }
        else
        {
            throw ValueException::FromString("Error reading from anonymous pipe");
        }
    }

    int PosixPipe::RawWrite(const char *buffer, int size)
    {
        int n;
        do
        {
            n = write(writeHandle, buffer, size);
        }
        while (n < 0 && errno == EINTR);
        if (n >= 0)
        {
            return n;
        }
        else
        {
            throw ValueException::FromString("Error writing Bytes data to pipe");
        }
    }

    void PosixPipe::CloseNativeRead()
    {
        if (readHandle != -1)
        {
            close(this->readHandle);
            this->readHandle = -1;
        }
    }

    void PosixPipe::CloseNativeWrite()
    {
        if (writeHandle != -1)
        {
            close(writeHandle);
            this->writeHandle = -1;
        }
    }
}