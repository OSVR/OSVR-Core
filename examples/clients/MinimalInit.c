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
#include <osvr/ClientKit/ClientKitC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
#include <stdio.h>

int main() {
    OSVR_ClientContext context =
        osvrClientInit("org.opengoggles.exampleclients.MinimalInit", 0);
    printf("OK, library initialized.\n");

    /* Pretend that this is your application's mainloop. */
    int i;
    for (i = 0; i < 1000000; ++i) {
        osvrClientUpdate(context);
    }

    printf("Library shut down, exiting.\n");
    return 0;
}
