// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "Channel.h"

#include "../Common/Macros/Macros.h"
#include SM_FLOW
#include SM_DEFAULT_CHECK

#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// INIT ============================================================================================

SM_RESULT sm_initChannel(
    sm_Channel *Channel_p)
{
SM_BEGIN()

#if defined(__linux__) || defined(__APPLE__)
    Channel_p->rw_p[0] = 0;
    Channel_p->rw_p[1] = 0;
#endif

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

SM_RESULT sm_openChannel(
    sm_Channel *Channel_p)
{
SM_BEGIN()

#if defined(__linux__) || defined(__APPLE__)
    pipe(Channel_p->rw_p);
#endif

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

void sm_closeChannelReadAccess(
    sm_Channel *Channel_p)
{
SM_BEGIN()

#if defined(__linux__) || defined(__APPLE__)
    close(Channel_p->rw_p[0]);
#endif

SM_SILENT_END()
}

void sm_closeChannelWriteAccess(
    sm_Channel *Channel_p)
{
SM_BEGIN()

#if defined(__linux__) || defined(__APPLE__)
    close(Channel_p->rw_p[1]);
#endif

SM_SILENT_END()
}

int sm_writeToChannel(
    sm_Channel *Channel_p, char *bytes_p, int byteCount)
{
SM_BEGIN()

#if defined(__linux__) || defined(__APPLE__)
    int result = write(Channel_p->rw_p[1], bytes_p, byteCount);
#endif

SM_END(result)
}

char *sm_readFromChannel(
    sm_Channel *Channel_p, size_t *size_p)
{
SM_BEGIN()

#if defined(__linux__) || defined(__APPLE__)

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1;

    int r = Channel_p->rw_p[0];

    fd_set set;
    FD_ZERO(&set);
    FD_SET(r, &set);

    int rv = select(r + 1, &set, NULL, NULL, &timeout);
    if (rv > 0)
    {
        int offset = 0;
        char *buff_p = malloc(128);
        memset(buff_p, '\0', 128);
        int result;
        while (result = read(r, buff_p + offset, 128)) {
            if (size_p != NULL) {*size_p += result;}
            if (result < 128) {break;}
            offset += 128;
            buff_p = realloc(buff_p, offset + 128);
            memset(buff_p + offset, '\0', 128);
        }
        SM_END(buff_p);
    }

#endif

SM_END(NULL)
}

