/** @file
    @brief Implementation

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

/*
// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)
*/

/* Internal Includes */
#include <osvr/ClientKit/ContextC.h>
#include <osvr/ClientKit/ParametersC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>

int main() {
    OSVR_ClientContext context =
        osvrClientInit("org.opengoggles.exampleclients.DisplayParameter", 0);

    const char *path = "/display";

    size_t length;
    osvrClientGetStringParameterLength(context, path, &length);

    char * displayDescription = malloc(length);
    osvrClientGetStringParameter(context, path, displayDescription, length);

    printf("Got value of %s:\n%s\n", path, displayDescription);

    free(displayDescription);
    osvrClientShutdown(context);
    printf("Library shut down, exiting.\n");
    return 0;
}
