/*******************************************************************************
 * error_messages -- Collection of constant error messages.
 ******************************************************************************/

#include <assert.h>
#include "error_messages.h"


// #############################################################################
// declarations
// #############################################################################

// Constant error messages
static const char* const cb_error_messages[CB_ERR_CODE_END] = {
    "No error",
    "Division by zero is not allowed"
};

// Unknown error
static const char* cb_error_unknown = "Unknown error";


// #############################################################################
// interface functions
// #############################################################################

const char* cb_error_get_message_by_code(CbErrorCode code)
{
    if (code >= CB_ERR_CODE_END)
        // treat everything above CB_ERR_CODE_END as "unknown error"
        return cb_error_unknown;
    else
        return cb_error_messages[code];
}
